//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
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

#include "random/IsotopeSelector.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Muonic atom capture interactor for deuteron or triton atoms.
 *
 * This is an \em at-rest interaction where an incoming muon track is
 * absorbed and the resulting secondary is a muonic deuteron or triton atom
 * with no kinetic energy.
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
    DTMuMinusAtomCaptureInteractor(DTMuMinusAtomCaptureData const& data,
                                   ParticleTrackView const& particle,
                                   MaterialView const& material,
                                   ElementView const& element,
                                   StackAllocator<Secondary>& allocate);

    // Sample an interaction with the given RNG
    template<class Engine>
    inline CELER_FUNCTION Interaction operator()(Engine& rng);

  private:
    // Shared constant physics properties
    DTMuMinusAtomCaptureData const& data_;
    // Material properties
    MaterialView const& material_;
    // Element properties
    ElementView const& element_;
    // Allocate space for secondary particle (one muonic d or t)
    StackAllocator<Secondary>& allocate_;
    // Deuteron number fraction
    real_type deuteron_frac_{-1};
    // Triton number fraction
    real_type triton_frac_{-1};

    // Select d or t spin (make it a separate helper)
    template<class Engine>
    inline CELER_FUNCTION real_type select_spin(AtomicMassNumber atomic_mass,
                                                Engine& rng);

    // Select muonic atom
    template<class Engine>
    inline CELER_FUNCTION ParticleId select_muonic_atom(Engine& rng);
};

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//
/*!
 * Construct with shared and state data.
 */
DTMuMinusAtomCaptureInteractor::DTMuMinusAtomCaptureInteractor(
    DTMuMinusAtomCaptureData const& data,
    ParticleTrackView const& particle,
    MaterialView const& material,
    ElementView const& element,
    StackAllocator<Secondary>& allocate)
    : data_(data), material_(material), element_(element), allocate_(allocate)
{
    // Expect hydrogen-only material, with deuteron or triton isotopes
    CELER_EXPECT(particle.particle_id() == data_.muon);
    CELER_EXPECT(material_.num_elements() == 1);
    CELER_EXPECT(material_.element_id(ElementComponentId{0}) == data_.hydrogen);

    auto const& el_view = material_.element_record(ElementComponentId{0});
    CELER_EXPECT(el_view.atomic_number() == AtomicNumber{1});

    // Store isotopic number fractions for the muonic atom selection
    for (auto const& iso : el_view.isotopes())
    {
        if (iso.isotope == data_.deuteron)
        {
            deuteron_frac_ = iso.fraction;
        }
        if (iso.isotope == data_.triton)
        {
            triton_frac_ = iso.fraction;
        }
    }

    // At least one of the isotopes must be present
    CELER_ENSURE(deuteron_frac_ >= 0 || triton_frac_ >= 0);
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
    secondary->particle_id = this->select_muonic_atom(isotope_id, rng);
    //! \todo Apply electromagnetic cascade to the formed atom
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
 * The fraction is hardcoded as 2/3 for deuteron and 3/4 for triton.
 *
 * \note
 * Yamashita, T., et al. Sci Rep 12, 6393 (2022).
 * https://doi.org/10.1038/s41598-022-09487-0
 *
 * \todo
 * Use \citet{yamashita-mucf-2022, https://doi.org/10.1038/s41598-022-09487-0}
 * and add ref to zotero.
 */
template<class Engine>
CELER_FUNCTION real_type DTMuMinusAtomCaptureInteractor::select_spin(
    AtomicMassNumber atomic_mass, Engine& rng)
{
    CELER_EXPECT(atomic_mass == AtomicMassNumber{2}
                 || atomic_mass == AtomicMassNumber{3});

    bool const is_deuteron = (atomic_mass == AtomicMassNumber{2});
    real_type const spin_fraction = is_deuteron ? (2 / 3) : 0.75;
    real_type const selected = UniformRealDistribution<real_type>(rng);

    real_type result;
    if (is_deuteron)
    {
        // Deuteron spin is either 3/2 or 1/2
        result = (selected < spin_fraction) ? 1.5 : 0.5;
    }
    else
    {
        // Triton spin is either 1 or 0
        result = (selected < spin_fraction) ? 1 : 0;
    }

    return result;
}

//---------------------------------------------------------------------------//
/*!
 * Select muonic atom (deuteron or triton).
 *
 * Selection is defined based on relative mole or number fractions (numerically
 * equivalent) and the relative probability of having a muonic deuteron or
 * triton at the end of the deexcitation cascade using the q1s formula
 * \citet{bom-dtmucf-2005, https://doi.org/10.1134/1.1926428}.
 *
 * The probability of d(mu) or t(mu) formation is calculated using
 * \f[
 * q_{1s} = \frac{1}{1 + 2.9 f_\text{triton}} \\
 * P(\text{d}\mu) = f_\text{deuteron} q_{1s}.
 * \f]
 *
 * A uniform random number is directly compared against \f$ P(\text{d}\mu) \f$,
 * to define which muonic atom is formed.
 *
 * \todo add a more descriptive calculation from the paper
 *
 * \note
 * Bom, V.R., et al. J. Exp. Theor. Phys. 100, 663–687 (2005).
 * https://doi.org/10.1134/1.1926428
 *
 * \todo
 * Use \citet{bom-dtmucf-2005, https://doi.org/10.1134/1.1926428}
 * and add ref to zotero.
 */
template<class Engine>
CELER_FUNCTION ParticleId
DTMuMinusAtomCaptureInteractor::select_muonic_atom(Engine& rng)
{
    CELER_EXPECT(deuteron_frac >= 0 || triton_frac >= 0);

    real_type const q1s = real_type{1} / (real_type{1} + 2.9 * triton_frac);
    real_type const deuteron_prob = deuteron_frac * q1s;

    UniformRealDistribution<real_type> uniform;
    if (uniform(rng) <= deuteron_prob)
    {
        return data_.muonic_deuteron;
    }
    return data_.muonic_triton;
}
//---------------------------------------------------------------------------//
}  // namespace celeritas
