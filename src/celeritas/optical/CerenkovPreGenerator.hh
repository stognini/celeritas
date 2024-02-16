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
    CerenkovPreGenerator::OpticalPreGenStepData step_data;
    // Populate step_data

   CerenkovPreGenerator pre_generate(particle_view,
                                     sim_view,
                                     material,
                                     properties->host_ref(),
                                     params->host_ref(),
                                     step_data);

    auto optical_dist_data = pre_generate(rng);
    if (optical_dist_data)
    {
        CerenkovGenerator cerenkov_generate(... , optical_dist_data, ...);
        cerenkov_generate(rng);
    }
 * \endcode
 */
class CerenkovPreGenerator
{
  public:
    // Placeholder for data that is not available through Views
    struct OpticalPreGenStepData
    {
        real_type time{};  //!< Pre-step time
        EnumArray<StepPoint, OpticalStepData> points;  //!< Pre- and post-steps

        //! Check whether the data are assigned
        explicit CELER_FUNCTION operator bool() const
        {
            return points[StepPoint::pre].speed > zero_quantity();
        }
    };

    // Construct with optical properties, Cerenkov, and step data
    inline CELER_FUNCTION
    CerenkovPreGenerator(ParticleTrackView const& particle_view,
                         SimTrackView const& sim_view,
                         OpticalMaterialId mat_id,
                         NativeCRef<OpticalPropertyData> const& properties,
                         NativeCRef<CerenkovData> const& shared,
                         OpticalPreGenStepData const& step_data);

    // Return a populated optical distribution data for the Cerenkov Generator
    template<class Generator>
    inline CELER_FUNCTION OpticalDistributionData operator()(Generator& rng);

  private:
    units::ElementaryCharge charge_;
    real_type step_len_;
    OpticalMaterialId mat_id_;
    OpticalPreGenStepData step_data_;
    real_type num_photons_per_len_;
};

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
/*!
 * Construct with optical properties, Cerenkov, and step information.
 */
CELER_FUNCTION CerenkovPreGenerator::CerenkovPreGenerator(
    ParticleTrackView const& particle_view,
    SimTrackView const& sim_view,
    OpticalMaterialId mat_id,
    NativeCRef<OpticalPropertyData> const& properties,
    NativeCRef<CerenkovData> const& shared,
    OpticalPreGenStepData const& step_data)
    : charge_(particle_view.charge())
    , step_len_(sim_view.step_length())
    , mat_id_(mat_id)
    , step_data_(step_data)
{
    CELER_EXPECT(charge_ != zero_quantity());
    CELER_EXPECT(step_len_ > 0);
    CELER_EXPECT(mat_id_);
    CELER_EXPECT(step_data_);

    auto const& pre = step_data_.points[StepPoint::pre];
    auto const& post = step_data_.points[StepPoint::post];
    units::LightSpeed beta(real_type{0.5}
                           * (pre.speed.value() + post.speed.value()));

    CerenkovDndxCalculator calculate_dndx(properties, shared, mat_id_, charge_);
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
        num_photons_per_len_ * step_len_)(rng);

    OpticalDistributionData data;
    data.num_photons = sampled_num_photons;
    data.time = step_data_.time;
    data.step_length = step_len_;
    data.charge = charge_;
    data.material = mat_id_;
    data.points = step_data_.points;

    return data;
}

//---------------------------------------------------------------------------//
}  // namespace celeritas
