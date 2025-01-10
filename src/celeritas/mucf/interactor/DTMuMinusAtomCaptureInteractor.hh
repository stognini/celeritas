//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/mucf/interactor/DTMuMinusAtomCaptureInteractor.hh
//---------------------------------------------------------------------------//
#pragma once

#include "corecel/Macros.hh"
#include "corecel/data/StackAllocator.hh"
#include "celeritas/mat/ElementView.hh"
#include "celeritas/mat/MaterialView.hh"
#include "celeritas/mucf/data/DTMuMinusAtomCaptureData.hh"
#include "celeritas/phys/Interaction.hh"
#include "celeritas/phys/ParticleTrackView.hh"
#include "celeritas/phys/Secondary.hh"

#include "mat/IsotopeSelector.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Muonic atom capture interactor for deuteron or triton atoms.
 *
 * This is an `at-rest` interaction where an incoming muon track is absorbed
 * and the resulting secondary is a muonic deuteron or triton atom with no
 * kinetic energy.
 */
class DTMuMinusAtomCaptureInteractor
{
  public:
    //!@{
    //! \name Type aliases
    using AtomicMassNumber = AtomicNumber;
    //!@}

    // Construct with defaults
    inline CELER_FUNCTION
    DTMuMinusAtomCaptureInteractor(DTMuMinusAtomCaptureData const& shared,
                                   ParticleTrackView const& particle,
                                   MaterialView const& material,
                                   ElementView const& element,
                                   StackAllocator<Secondary>& allocate);

    // Sample an interaction with the given RNG
    template<class Engine>
    inline CELER_FUNCTION Interaction operator()(Engine& rng);

  private:
    // Shared constant physics properties
    DTMuMinusAtomCaptureData const& shared_;
    // Material properties
    MaterialView const& material_;
    // Element properties
    ElementView const& element_;
    // Allocate space for secondary particle (one muonic d or t)
    StackAllocator<Secondary>& allocate_;

    // Select d or t spin (make it a separate helper)
    template<class Engine>
    inline CELER_FUNCTION real_type select_spin(AtomicMassNumber atomic_mass,
                                                Engine& rng);

    // Select muonic atom
    template<class Engine>
    inline CELER_FUNCTION ParticleId
    select_muonic_atom(ElementId hidrogen_isotope, Engine& rng);
};

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//
/*!
 * Construct with shared and state data.
 */
DTMuMinusAtomCaptureInteractor::DTMuMinusAtomCaptureInteractor(
    DTMuMinusAtomCaptureData const& shared,
    ParticleTrackView const& particle,
    MaterialView const& material,
    ElementView const& element,
    StackAllocator<Secondary>& allocate)
    : shared_(shared)
    , material_(material)
    , element_(element)
    , allocate_(allocate)
{
    CELER_EXPECT(particle.particle_id() == shared_.muon);
    // TODO: Expect correct material and composition
}

//---------------------------------------------------------------------------//
/*!
 * Sample a muon capture by a deuteron or triton in the material. The final
 * muonic atom is also at rest.
 */
template<class Engine>
CELER_FUNCTION Interaction DTMuMinusAtomCaptureInteractor::operator()(Engine& rng)
{
    // Allocate space for the final muonic d or t atom
    Secondary* secondary = allocate_(1);
    if (secondaries == nullptr)
    {
        // Failed to allocate space for two secondaries
        return Interaction::from_failure();
    }

    Interaction result = Interaction::from_absorption();
    result.secondaries = {secondary};

    // Select d or t isotope
    // TODO: Looks not used in G4???
    IsotopeSelector select(element_);
    auto const isotope_compid = select(rng);
    auto const isotope_id = element_.isotope_id(isotope_compid);
    CELER_ASSERT(isotope_id == shared_.deuteron
                 || isotope_id == shared_.triton);

    // FIGURE OUT ELEMENT VS ISOTOPE ID
    // ACTUALLY NEED ELEMENTRECORD -> ELISOTOPECOMPONENT

    // TODO: Apply electromagnetic cascade to the formed atom
    secondary->particle_id = this->select_muonic_atom(isotope_id, rng);
    secondary->spin = this->select_spin(element_.atomic_mass_number(), rng);
    return result;
}

//---------------------------------------------------------------------------//
/*!
 * Select muonic atom spin.
 *
 * If the atom is deuteron, the muonic atom has a spin of 3/2 or 1/2.
 * If the atom is triton, the muonic atom has a spin of 1 or 0.
 *
 * \note
 * Yamashita, T., et al. Sci Rep 12, 6393 (2022).
 * https://doi.org/10.1038/s41598-022-09487-0
 */
template<class Engine>
CELER_FUNCTION real_type DTMuMinusAtomCaptureInteractor::select_spin(
    AtomicMassNumber atomic_mass, Engine& rng)
{
    CELER_EXPECT(atomic_mass == AtomicMassNumber{2}
                 || atomic_mass == AtomicMassNumber{3});

    bool const is_deuteron = (atomic_mass == AtomicMassNumber{2});
    real_type const up_frac = is_deuteron ? (2 / 3) : (3 / 4);
    real_type const spin_up = is_deuteron ? (3 / 2) : (1 / 2);
    real_type const spin_down = is_deuteron ? 1 : 0;

    UniformRealDistribution<real_type> uniform;
    if (uniform(rng) < up_frac)
    {
        return spin_up;
    }

    return spin_down;
}

//---------------------------------------------------------------------------//
/*!
 * Select muonic atom (deuteron or triton).
 *
 * Selection is defined based on relative mole fractions and the relative
 * probability of having a muonic deuteron or triton at the end of the
 * deexcitation cascade using Q1S formula.
 *
 * \note
 * Bom, V.R., et al. J. Exp. Theor. Phys. 100, 663–687 (2005).
 * https://doi.org/10.1134/1.1926428
 */
template<class Engine>
CELER_FUNCTION ParticleId DTMuMinusAtomCaptureInteractor::select_muonic_atom(
    IsotopeId hidrogen_isotope, Engine& rng)
{
    // Calculate number fraction (numerically equivalent to mole fraction)
    auto const deuteron_elid;
    auto const triton_elid;

    // LOOP OVER, SELECT CORRECT FRACTIONS FOR D and T OF
    // ElementRecord.isotopes

    real_type const deuteron_mole_frac
        = material_.element_mole_fraction(deuteron_elid);
    real_type const triton_mole_frac
        = material_.element_mole_fraction(triton_elid);

    // Compute probability and return (d)mu or (t)mu via the Q1S formula
    real_type const q1s = real_type{1}
                          / (real_type{1} + 2.9 * triton_mole_frac);
    real_type const deuteron_prob = deuteron_mole_frac * q1s;

    UniformRealDistribution<real_type> uniform;
    if (uniform(rng) <= deuteron_prob)
    {
        return shared_.muonic_deuteron;
    }

    return shared_.muonic_triton;
}
//---------------------------------------------------------------------------//
}  // namespace celeritas
