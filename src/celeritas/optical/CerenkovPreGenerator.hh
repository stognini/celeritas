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
    CerenkovPreGenerator pre_cerenkov(particle_view,
                                      properties,
                                      cerenkov_data,
                                      material_id,
                                      optical_step_collector.get(tid));

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
    // Construct with particle and material data
    inline CELER_FUNCTION
    CerenkovPreGenerator(ParticleTrackView const& particle_view,
                         NativeCRef<OpticalPropertyData> const& properties,
                         NativeCRef<CerenkovData> const& shared,
                         OpticalMaterialId mat_id,
                         OpticalStepCollectorData const& optical_step);

    // Return a populated distribution data for the Cerenkov Generator
    template<class Generator>
    inline CELER_FUNCTION OpticalDistributionData operator()(Generator& rng);

  private:
    units::ElementaryCharge charge_;
    OpticalMaterialId mat_id_;
    OpticalStepCollectorData step_collector_data_;
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
    ParticleTrackView const& particle_view,
    NativeCRef<OpticalPropertyData> const& properties,
    NativeCRef<CerenkovData> const& shared,
    OpticalMaterialId mat_id,
    OpticalStepCollectorData const& optical_step)
    : charge_(particle_view.charge())
    , mat_id_(mat_id)
    , step_collector_data_(optical_step)
{
    CELER_EXPECT(charge_.value() != 0);
    CELER_EXPECT(mat_id_);
    CELER_EXPECT(step_collector_data_);

    auto const& pre = step_collector_data_.points[StepPoint::pre];
    auto const& post = step_collector_data_.points[StepPoint::post];
    units::LightSpeed beta(real_type{0.5}
                           * (pre.speed.value() + post.speed.value()));

    CerenkovDndxCalculator dndx_calculator(
        properties, shared, mat_id_, charge_);
    num_photons_per_len_ = dndx_calculator(beta);
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
        num_photons_per_len_ * step_collector_data_.step_length)(rng);

    if (sampled_num_photons == 0)
    {
        // Not an optical material or this step is below production threshold
        return {};
    }

    // Populate optical distribution data
    OpticalDistributionData data;
    data.num_photons = sampled_num_photons;
    data.charge = charge_;
    data.step_length = step_collector_data_.step_length;
    data.time = step_collector_data_.time;
    data.points = step_collector_data_.points;
    data.material = mat_id_;

    return data;
}

//---------------------------------------------------------------------------//
}  // namespace celeritas
