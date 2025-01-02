//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file accel/detail/HitProcessor.hh
//---------------------------------------------------------------------------//
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <G4TouchableHandle.hh>

#include "corecel/Macros.hh"
#include "celeritas/Types.hh"
#include "celeritas/geo/GeoFwd.hh"
#include "celeritas/user/DetectorSteps.hh"
#include "celeritas/user/StepData.hh"

#include "TouchableUpdaterInterface.hh"

class G4LogicalVolume;
class G4Step;
class G4ParticleDefinition;
class G4Track;
class G4VSensitiveDetector;

namespace celeritas
{
struct StepSelection;
struct DetectorStepOutput;

namespace detail
{
//---------------------------------------------------------------------------//
/*!
 * Transfer Celeritas sensitive detector hits to Geant4.
 *
 * This serves a similar purpose to the \c G4FastSimHitMaker class for
 * generating hit objects.
 *
 * \warning This class \b must be thread-local because the sensitive
 * detectors it points to are thread-local objects. Furthermore, Geant4
 * thread-local object allocators for the navigation state and tracks mean this
 * class \b must be destroyed on the same thread on which it was created.
 *
 * Call operator:
 * - Loop over detector steps
 * - Update step attributes based on hit selection for the detector (TODO:
 *   selection is global for now)
 * - Call the local detector (based on detector ID from map) with the step
 *
 * Compare to Geant4 updating step/track info:
 * - \c G4VParticleChange::UpdateStepInfo
 * - \c G4ParticleChangeForTransport::UpdateStepForAlongStep
 * - \c G4ParticleChangeForTransport::UpdateStepForPostStep
 * - \c G4StackManager::PrepareNewEvent
 * - \c G4SteppingManager::ProcessSecondariesFromParticleChange
 * - \c G4Step::UpdateTrack
 */
class HitProcessor
{
  public:
    //!@{
    //! \name Type aliases
    using StepStateHostRef = HostRef<StepStateData>;
    using StepStateDeviceRef = DeviceRef<StepStateData>;
    using SPConstGeo = std::shared_ptr<GeoParams const>;
    using SPConstVecLV
        = std::shared_ptr<std::vector<G4LogicalVolume const*> const>;
    using VecParticle = std::vector<G4ParticleDefinition const*>;
    //!@}

  public:
    // Construct from volumes that have SDs and step selection
    HitProcessor(SPConstVecLV detector_volumes,
                 SPConstGeo const& geo,
                 VecParticle const& particles,
                 StepSelection const& selection,
                 bool locate_touchable);

    // Log on destruction
    ~HitProcessor();
    CELER_DEFAULT_MOVE_DELETE_COPY(HitProcessor);

    // Process CPU-generated hits
    void operator()(StepStateHostRef const&);

    // Process device-generated hits
    void operator()(StepStateDeviceRef const&);

    // Generate and call hits from a detector output (for testing)
    void operator()(DetectorStepOutput const& out) const;

    // Access detector volume corresponding to an ID
    inline G4LogicalVolume const* detector_volume(DetectorId) const;

    // Access thread-local SD corresponding to an ID
    inline G4VSensitiveDetector* detector(DetectorId) const;

  private:
    //! Detector volumes for navigation updating
    SPConstVecLV detector_volumes_;
    //! Map detector IDs to sensitive detectors
    std::vector<G4VSensitiveDetector*> detectors_;
    //! Temporary CPU hit information
    DetectorStepOutput steps_;

    //! Temporary step
    std::unique_ptr<G4Step> step_;
    //! Tracks for each particle type
    std::vector<std::unique_ptr<G4Track>> tracks_;
    //! Geant4 reference-counted pointer to a G4VTouchable
    G4TouchableHandle touch_handle_;
    //! Navigator for finding points
    std::unique_ptr<TouchableUpdaterInterface> update_touchable_;

    //! Post-step selection for copying to track
    StepPointSelection post_step_selection_;

    void update_track(ParticleId id) const;
};

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//
/*!
 * Access detector volume corresponding to an ID.
 */
G4LogicalVolume const* HitProcessor::detector_volume(DetectorId did) const
{
    CELER_EXPECT(did < detector_volumes_->size());
    return (*detector_volumes_)[did.unchecked_get()];
}

//---------------------------------------------------------------------------//
/*!
 * Access thread-local sensitive detector corresponding to an ID.
 */
G4VSensitiveDetector* HitProcessor::detector(DetectorId did) const
{
    CELER_EXPECT(did < detectors_.size());
    return detectors_[did.unchecked_get()];
}

//---------------------------------------------------------------------------//
}  // namespace detail
}  // namespace celeritas
