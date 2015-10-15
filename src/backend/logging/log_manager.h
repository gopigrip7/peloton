/*-------------------------------------------------------------------------
 *
 * logmanager.h
 * file description
 *
 * Copyright(c) 2015, CMU
 *
 * /peloton/src/backend/logging/logmanager.h
 *
 *-------------------------------------------------------------------------
 */

#pragma once

#include "backend/logging/logger.h"
#include <mutex>
#include <map>
#include <vector>

#include "backend_logger.h"
#include "frontend_logger.h"

namespace peloton {
namespace logging {

//===--------------------------------------------------------------------===//
// Log Manager
//===--------------------------------------------------------------------===//

// Logging basically refers to the PROTOCOL -- like aries or peloton
// Logger refers to the implementation -- like frontend or backend
// Transition diagram :: standby -> recovery -> logging -> terminate -> sleep

/**
 * Global Log Manager
 */
class LogManager{

  public:
    LogManager(const LogManager &) = delete;
    LogManager &operator=(const LogManager &) = delete;
    LogManager(LogManager &&) = delete;
    LogManager &operator=(LogManager &&) = delete;

    // global singleton
    static LogManager& GetInstance(void);

    // Wait for the system to begin
    void StartStandbyMode(LoggingType logging_type = LOGGING_TYPE_INVALID );

    // Start recovery
    void StartRecoveryMode(LoggingType logging_type = LOGGING_TYPE_INVALID);

    // Check whether the frontend logger is in logging mode
    bool IsInLoggingMode(LoggingType logging_type = LOGGING_TYPE_INVALID);

    // Used to
    void WaitForSleepMode(LoggingType logging_type = LOGGING_TYPE_INVALID);

    // End the actual logging
    bool EndLogging(LoggingType logging_type =  LOGGING_TYPE_INVALID);

    //===--------------------------------------------------------------------===//
    // Accessors
    //===--------------------------------------------------------------------===//

    // Logging status associated with the front end logger of given type
    void SetLoggingStatus(LoggingType logging_type, LoggingStatus logging_status);

    LoggingStatus GetStatus(LoggingType logging_type = LOGGING_TYPE_INVALID);

    void ResetLoggingStatusMap(LoggingType logging_type);

    // Default protocol or logging type for member function calls
    void SetDefaultLoggingType(LoggingType logging_type);

    LoggingType GetDefaultLoggingType(void);

    // Whether to enable or disable synchronous commit ?
    void SetSyncCommit(bool sync_commit) { syncronization_commit = sync_commit;}

    bool GetSyncCommit(void) const { return syncronization_commit;}

    size_t ActiveFrontendLoggerCount(void) const;

    BackendLogger* GetBackendLogger(LoggingType logging_type = LOGGING_TYPE_INVALID);

    bool RemoveBackendLogger(BackendLogger* backend_logger,
                             LoggingType logging_type = LOGGING_TYPE_INVALID);

    FrontendLogger* GetFrontendLogger(LoggingType logging_type);

  private:

    LogManager(){};

    //===--------------------------------------------------------------------===//
    // Utility Functions
    //===--------------------------------------------------------------------===//

    bool RemoveFrontendLogger(LoggingType logging_type);

    //===--------------------------------------------------------------------===//
    // Data members
    //===--------------------------------------------------------------------===//

    // default logging type for new logging
    // this is used instead when the argument passed to member functions
    // is LOGGING_TYPE_INVALID
    LoggingType default_logging_type = LOGGING_TYPE_INVALID;

    // There is only one frontend_logger for each type of logging
    // like -- stdout, aries, peloton
    std::vector<FrontendLogger*> frontend_loggers;

    std::map<LoggingType, LoggingStatus> logging_statuses;

    bool syncronization_commit = false;

    // To manage frontend loggers
    std::mutex frontend_logger_mutex;

    // To synch the status map
    std::mutex logging_status_mutex;

};


}  // namespace logging
}  // namespace peloton
