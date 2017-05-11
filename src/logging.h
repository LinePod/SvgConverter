#ifndef SVG_CONVERTER_LOGGING_H_H
#define SVG_CONVERTER_LOGGING_H_H

#include <spdlog/spdlog.h>

constexpr const char* const kLoggerName = "console";

/**
 * Setup the global logger. Should only be called once.
 */
spdlog::logger& setup_global_logger();

/**
 * Retrieves a reference to the global logger.
 *
 * This is somewhat expensive and should only be used sparingly (passing around
 * the returned reference is much cheaper).
 */
spdlog::logger& get_global_logger();

#endif  // SVG_CONVERTER_LOGGING_H_H
