//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file corecel/sys/TracingSession.cc
//! \brief RAII class for managing a perfetto session and its resources.
//---------------------------------------------------------------------------//
#include "TracingSession.hh"

#include <fcntl.h>
#include <perfetto.h>

#include "Environment.hh"
#include "ScopedProfiling.hh"

#include "detail/TrackEvent.perfetto.hh"

PERFETTO_TRACK_EVENT_STATIC_STORAGE();

namespace celeritas
{
namespace
{
//---------------------------------------------------------------------------//
//! Supported tracing mode
enum class TracingMode : uint32_t
{
    in_process,  //!< Record in-process, writting to a file
    system  //!< Record in a system daemon
};

//---------------------------------------------------------------------------//
/*!
 * Initialize the session for the given mode if profiling is enabled.
 */
std::unique_ptr<perfetto::TracingSession>
initialize_session(TracingMode mode) noexcept
{
    if (!celeritas::use_profiling())
    {
        return nullptr;
    }
    perfetto::TracingInitArgs args;
    args.backends |= [&] {
        switch (mode)
        {
            case TracingMode::in_process:
                return perfetto::kInProcessBackend;
            case TracingMode::system:
                return perfetto::kSystemBackend;
            default:
                return perfetto::kSystemBackend;
        }
    }();
    perfetto::Tracing::Initialize(args);
    perfetto::TrackEvent::Register();
    return perfetto::Tracing::NewTrace();
}

//---------------------------------------------------------------------------//
/*!
 * Configure the session to record Celeritas track events.
 */
perfetto::TraceConfig configure_session() noexcept
{
    perfetto::protos::gen::TrackEventConfig track_event_cfg;
    track_event_cfg.add_disabled_categories("*");
    track_event_cfg.add_enabled_categories(
        celeritas::detail::perfetto_track_event_category);

    perfetto::TraceConfig cfg;
    constexpr int mb_kb = 1024;
    uint32_t buffer_size_kb = 20 * mb_kb;
    if (std::string var = celeritas::getenv("CELER_PERFETTO_BUFFER_SIZE_MB");
        !var.empty())
    {
        buffer_size_kb = std::stoul(var) * mb_kb;
    }
    cfg.add_buffers()->set_size_kb(buffer_size_kb);
    auto* ds_cfg = cfg.add_data_sources()->mutable_config();
    ds_cfg->set_name("track_event");
    ds_cfg->set_track_event_config_raw(track_event_cfg.SerializeAsString());
    return cfg;
}

//---------------------------------------------------------------------------//
}  // namespace

//---------------------------------------------------------------------------//
/*!
 * Start a system tracing session.
 */
TracingSession::TracingSession() noexcept
    : session_{initialize_session(TracingMode::system).release()}
{
    if (session_)
    {
        session_->Setup(configure_session());
    }
}

//---------------------------------------------------------------------------//
/*!
 * Start an in-process tracing session.
 */
TracingSession::TracingSession(std::string_view filename) noexcept
    : session_{initialize_session(filename.empty() ? TracingMode::system
                                                   : TracingMode::in_process)
                   .release()}
{
    if (session_)
    {
        if (!filename.empty())
        {
            fd_ = open(filename.data(), O_RDWR | O_CREAT | O_TRUNC, 0660);
        }
        session_->Setup(configure_session(), fd_);
    }
}

//---------------------------------------------------------------------------//
/*!
 * Block until the current session is closed.
 */
TracingSession::~TracingSession()
{
    if (session_)
    {
        if (started_)
        {
            session_->StopBlocking();
        }
        if (fd_ != system_fd_)
        {
            close(fd_);
        }
    }
}

//---------------------------------------------------------------------------//
/*!
 * Start the profiling session.
 */
void TracingSession::start() noexcept
{
    if (session_)
    {
        started_ = true;
        session_->StartBlocking();
    }
}

//---------------------------------------------------------------------------//
/*!
 * Define the deleter where the TracingSession definition is accessible.
 */
void TracingSession::Deleter::operator()(perfetto::TracingSession* p)
{
    delete p;
}

//---------------------------------------------------------------------------//
}  // namespace celeritas
