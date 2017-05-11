#include "logging.h"

spdlog::logger& setup_global_logger() {
    // Because this program is completely single threaded, we can afford to use
    // normal references instead of shared pointers.
    auto logger_ptr = spdlog::stderr_logger_st(kLoggerName);
    logger_ptr->set_level(spdlog::level::debug);
    logger_ptr->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    return *logger_ptr;
}

/**
 * Retrieves a reference to the global logger.
 *
 * This is somewhat expensive and should only be used sparingly (passing around
 * the returned reference is much cheaper).
 */
spdlog::logger& get_global_logger() { return *spdlog::get(kLoggerName); }
