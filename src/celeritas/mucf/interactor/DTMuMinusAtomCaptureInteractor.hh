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
#include "celeritas/mucf/data/DTMuMinusAtomCaptureData.hh"
#include "celeritas/phys/Interaction.hh"
#include "celeritas/phys/ParticleTrackView.hh"
#include "celeritas/phys/Secondary.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Muonic atom capture interactor for deuteron or triton atoms.
 *
 * This is an \em at-rest interaction where an incoming muon is absorbed and
 * the resulting secondary is a muonic deuteron or triton atom with no kinetic
 * energy.
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
                                   ElementView const& element,
                                   StackAllocator<Secondary>& allocate);

    // Sample an interaction with the given RNG
    template<class Engine>
    inline CELER_FUNCTION Interaction operator()(Engine& rng);

  private:
    // Shared constant physics properties
    DTMuMinusAtomCaptureData const& data_;
    // Allocate space for secondary particle (one muonic d or t)
    StackAllocator<Secondary>& allocate_;
    // Store if it is a deuteron target
    real_type deuteron_frac_{};
    // Triton number fraction
    real_type triton_frac_{};

    // Select deuteron or triton isotope to form muonic atom
    template<class Engine>
    inline CELER_FUNCTION IsotopeId select_isotope(Engine& rng);

    // Select muonic atom spin
    template<class Engine>
    inline CELER_FUNCTION ParticleId select_atom_spin(IsotopeId isotope_id,
                                                      Engine& rng);
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
    ElementView const& element,
    StackAllocator<Secondary>& allocate)
    : data_(data), allocate_(allocate)
{
    // Expect hydrogen-only material, with deuteron or triton isotopes
    CELER_EXPECT(particle.particle_id() == data_.muon);

    // Store isotopic number fractions for the muonic atom selection
    for (auto const& iso : element.isotopes())
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
    CELER_ENSURE(deuteron_frac_ + triton_frac_ > 0);
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

    // Select d or t isotope
    auto isotope_id = this->select_isotope(rng);
    // Form muonic atom at rest with a given spin
    secondary->particle_id = this->select_atom_spin(isotope_id, rng);
    //! \todo Apply electromagnetic cascade to the formed atom

    Interaction result = Interaction::from_absorption();
    result.secondaries = {secondary};
    return result;
}

//---------------------------------------------------------------------------//
/*!
 * Select muonic atom spin.
 *
 * Deuteron muonic atoms have a spin of 3/2 or 1/2.
 * Triton muonic atoms have a spin of 1 or 0.
 *
 * Fractions of atoms with each spin are set as 2/3 for deuterons and 3/4 for
 * tritons.
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
CELER_FUNCTION ParticleId DTMuMinusAtomCaptureInteractor::select_atom_spin(
    IsotopeId isotope_id, Engine& rng)
{
    CELER_EXPECT(isotope_id == data_.deuteron || isotope_id == data_.triton);

    bool const is_deuteron = (isotope_id == data_.deuteron) ? true : false;
    auto const spin_fraction = is_deuteron
                                   ? data_.muonic_deuteron_spin_fraction()
                                   : data_.muonic_triton_spin_fraction();
    auto const selected = UniformRealDistribution<real_type>(rng);

    ParticleId result;
    if (is_deuteron)
    {
        // Deuteron spin is either 3/2 or 1/2
        result = (selected < spin_fraction)
                     ? data_.muonic_deuteron_spin_3_over_2
                     : data_.muonic_deuteron_spin_1_over_2;
    }
    else
    {
        // Triton spin is either 1 or 0
        result = (selected < spin_fraction) ? data_.muonic_triton_spin_1
                                            : data_.muonic_triton_spin_0;
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
CELER_FUNCTION IsotopeId
DTMuMinusAtomCaptureInteractor::select_isotope(Engine& rng)
{
    CELER_EXPECT(deuteron_frac_ + triton_frac_ > 0);

    real_type const q1s = real_type{1} / (real_type{1} + 2.9 * triton_frac_);
    real_type const deuteron_prob = deuteron_frac_ * q1s;

    UniformRealDistribution<real_type> uniform;
    if (uniform(rng) <= deuteron_prob)
    {
        return data_.deuteron;
    }
    return data_.triton;
}
//---------------------------------------------------------------------------//
}  // namespace celeritas
