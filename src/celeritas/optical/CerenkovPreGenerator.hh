//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/optical/CerenkovPreGenerator.hh
//---------------------------------------------------------------------------//
#pragma once

#include "corecel/Assert.hh"
#include "corecel/Macros.hh"
#include "celeritas/phys/ParticleTrackView.hh"
#include "celeritas/random/distribution/PoissonDistribution.hh"
#include "celeritas/track/SimTrackView.hh"

#include "CerenkovData.hh"
#include "CerenkovDndxCalculator.hh"
#include "OpticalDistributionData.hh"
#include "OpticalPropertyData.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Sample the number of Cerenkov photons to be generated by
 * \c CerenkovGenerator and populate \c OpticalDistributionData values using
 * Param and State data.
 * \code
    CerenkovPreGenerator pre_cerenkov(
        properties, cerenkov_data, input_step_data);

    auto optical_dist_data = pre_cerenkov(rng);
    if (optical_dist_data)
    {
        CerenkovGenerator cerenkov_gen(... , optical_dist_data, ...);
        cerenkov_gen(rng);
    }
 * \endcode
 */
class CerenkovPreGenerator
{
  public:
    struct InputStepStateData
    {
        real_type time{};  //!< Pre-step time
        real_type step_length{};  //!< Step length
        units::ElementaryCharge charge;
        OpticalMaterialId material;
        EnumArray<StepPoint, OpticalStepData> points;  //!< Pre- and post-steps

        //! Check whether the data are assigned
        explicit CELER_FUNCTION operator bool() const
        {
            return step_length > 0 && charge != zero_quantity() && material
                   && points[StepPoint::pre].speed > zero_quantity();
        }
    };

    // Construct with particle and material data
    inline CELER_FUNCTION
    CerenkovPreGenerator(NativeCRef<OpticalPropertyData> const& properties,
                         NativeCRef<CerenkovData> const& shared,
                         InputStepStateData const& input_step_data);

    // Return a populated distribution data for the Cerenkov Generator
    template<class Generator>
    inline CELER_FUNCTION OpticalDistributionData operator()(Generator& rng);

  private:
    InputStepStateData step_data_;
    real_type num_photons_per_len_;
};

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
/*!
 * Construct from particle, material, and optical step collection data.
 */
CELER_FUNCTION CerenkovPreGenerator::CerenkovPreGenerator(
    NativeCRef<OpticalPropertyData> const& properties,
    NativeCRef<CerenkovData> const& shared,
    InputStepStateData const& input_step_data)
    : step_data_(input_step_data)
{
    CELER_EXPECT(step_data_);

    auto const& pre = step_data_.points[StepPoint::pre];
    auto const& post = step_data_.points[StepPoint::post];
    units::LightSpeed beta(real_type{0.5}
                           * (pre.speed.value() + post.speed.value()));

    CerenkovDndxCalculator calculate_dndx(
        properties, shared, step_data_.material, step_data_.charge);
    num_photons_per_len_ = calculate_dndx(beta);
}

//---------------------------------------------------------------------------//
/*!
 * Return an \c OpticalDistributionData object. If no photons are sampled, an
 * empty object is returned and can be verified via its own operator bool.
 *
 * The number of photons is sampled from a Poisson distribution with a mean
 * \f[
   \langle n \rangle = \ell_\text{step} \frac{dN}{dx}
 * \f]
 * where \f$ \ell_\text{step} \f$ is the step length.
 */
template<class Generator>
CELER_FUNCTION OpticalDistributionData
CerenkovPreGenerator::operator()(Generator& rng)
{
    if (num_photons_per_len_ == 0)
    {
        return {};
    }

    // Sample number of photons from a Poisson distribution
    auto sampled_num_photons = PoissonDistribution<real_type>(
        num_photons_per_len_ * step_data_.step_length)(rng);

    if (sampled_num_photons == 0)
    {
        // Not an optical material or this step is below production threshold
        return {};
    }

    // Populate optical distribution data
    OpticalDistributionData data;
    data.num_photons = sampled_num_photons;
    data.time = step_data_.time;
    data.step_length = step_data_.step_length;
    data.charge = step_data_.charge;
    data.material = step_data_.material;
    data.points = step_data_.points;

    return data;
}

//---------------------------------------------------------------------------//
}  // namespace celeritas
