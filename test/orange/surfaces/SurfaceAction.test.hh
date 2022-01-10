//----------------------------------*-C++-*----------------------------------//
// Copyright 2021-2022 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file SurfaceAction.test.hh
//---------------------------------------------------------------------------//
#include <vector>
#include "base/CollectionBuilder.hh"
#include "orange/Data.hh"
#include "orange/surfaces/Surfaces.hh"
#include "orange/surfaces/SurfaceAction.hh"

namespace celeritas_test
{
using celeritas::MemSpace;
using celeritas::Ownership;
using celeritas::Real3;
using celeritas::real_type;
using celeritas::ThreadId;

using celeritas::Sense;
using celeritas::SurfaceState;

//---------------------------------------------------------------------------//
/*!
 * Small subset of state for testing.
 */
template<Ownership W, MemSpace M>
struct OrangeMiniStateData
{
    template<class T>
    using StateItems = celeritas::StateCollection<T, W, M>;

    StateItems<Real3>     pos;
    StateItems<Real3>     dir;
    StateItems<Sense>     sense;
    StateItems<real_type> distance;

    //! True if sizes are consistent and nonzero
    explicit CELER_FUNCTION operator bool() const
    {
        // clang-format off
        return pos.size() > 0
            && dir.size() == pos.size()
            && sense.size() == pos.size()
            && distance.size() == pos.size();
        // clang-format on
    }

    //! Assign from another set of data
    template<Ownership W2, MemSpace M2>
    OrangeMiniStateData& operator=(OrangeMiniStateData<W2, M2>& other)
    {
        CELER_EXPECT(other);
        pos      = other.pos;
        dir      = other.dir;
        sense    = other.sense;
        distance = other.distance;
        CELER_ENSURE(*this);
        return *this;
    }

    //! State size
    CELER_FUNCTION ThreadId::size_type size() const { return pos.size(); }
};

//---------------------------------------------------------------------------//
/*!
 * Resize states in host code.
 */
template<MemSpace M>
inline void
resize(OrangeMiniStateData<Ownership::value, M>* data,
       const celeritas::SurfaceData<Ownership::const_reference, MemSpace::host>&,
       celeritas::size_type size)
{
    CELER_EXPECT(data);
    CELER_EXPECT(size > 0);
    make_builder(&data->pos).resize(size);
    make_builder(&data->dir).resize(size);
    make_builder(&data->sense).resize(size);
    make_builder(&data->distance).resize(size);

    CELER_ENSURE(*data);
}

//---------------------------------------------------------------------------//
// HELPER FUNCTIONS
//---------------------------------------------------------------------------//
//! Calculate the sense of a surface at a given position.
struct CalcSenseDistance
{
    const Real3& pos;
    const Real3& dir;
    Sense*       sense;
    real_type*   distance;

    template<class S>
    CELER_FUNCTION void operator()(S&& surf)
    {
        // Calculate sense
        auto signed_sense = surf.calc_sense(this->pos);
        *this->sense      = to_sense(signed_sense);

        // Calculate nearest distance
        auto intersect = surf.calc_intersections(
            this->pos, this->dir, to_surface_state(signed_sense));
        for (real_type distance : intersect)
        {
            CELER_ASSERT(distance > 0);
        }
        *this->distance
            = *celeritas::min_element(intersect.begin(), intersect.end());
    }
};

//! Calculate distance to a single surface
template<MemSpace M = MemSpace::native>
struct CalcSenseDistanceLauncher
{
    celeritas::SurfaceData<Ownership::const_reference, M> params;
    OrangeMiniStateData<Ownership::reference, M>          states;

    CELER_FUNCTION void operator()(ThreadId tid) const
    {
        celeritas::Surfaces surfaces(this->params);

        auto calc_sense_dist = celeritas::make_surface_action(
            surfaces,
            CalcSenseDistance{this->states.pos[tid],
                              this->states.dir[tid],
                              &this->states.sense[tid],
                              &this->states.distance[tid]});

        celeritas::SurfaceId sid{tid.get() % surfaces.num_surfaces()};
        calc_sense_dist(sid);
    }
};

//---------------------------------------------------------------------------//
// TESTING INTERFACE
//---------------------------------------------------------------------------//
//! Input data
struct SATestInput
{
    using ParamsRef
        = celeritas::SurfaceData<Ownership::const_reference, MemSpace::device>;
    using StateRef
        = OrangeMiniStateData<Ownership::reference, MemSpace::device>;

    ParamsRef params;
    StateRef  states;
};

//---------------------------------------------------------------------------//
//! Run on device and return results
void sa_test(SATestInput);

#if !CELERITAS_USE_CUDA
inline void sa_test(SATestInput)
{
    CELER_NOT_CONFIGURED("CUDA");
}
#endif

//---------------------------------------------------------------------------//
} // namespace celeritas_test