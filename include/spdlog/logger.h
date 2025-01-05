// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

// Thread-safe logger, with exceptions for these non-thread-safe methods:
//   set_pattern()       - Modifies the log pattern.
//   set_formatter()     - Sets a new formatter.
//   set_error_handler() - Assigns a new error handler.
//   sinks() (non-const) - Accesses and potentially modifies the sinks directly.
// By default, the logger does not throw exceptions during logging.
// To enable exception throwing for logging errors, set a custom error handler.

#include <cassert>
#include <iterator>
#include <vector>

#include "common.h"
#include "details/err_helper.h"
#include "details/log_msg.h"
#include "sinks/sink.h"

namespace spdlog {

class SPDLOG_API logger {
public:
    // Empty logger
    explicit logger(std::string name)
        : name_(std::move(name)) {}

    // Logger with range on sinks
    template <typename It>
    logger(std::string name, It begin, It end)
        : name_(std::move(name)),
          sinks_(begin, end) {}

    // Logger with single sink
    logger(std::string name, sink_ptr single_sink)
        : logger(std::move(name), {std::move(single_sink)}) {}

    // Logger with sinks init list
    logger(std::string name, sinks_init_list sinks)
        : logger(std::move(name), sinks.begin(), sinks.end()) {}

    logger(const logger &other) noexcept;
    logger(logger &&other) noexcept;

    ~logger() = default;

    template <typename... Args>
    void log(source_loc loc, level lvl, format_string_t<Args...> fmt, Args &&...args) {
        if (should_log(lvl)) {
            log_with_format_(loc, lvl, fmt, std::forward<Args>(args)...);
        }
    }

    template <typename... Args>
    void log(level lvl, format_string_t<Args...> fmt, Args &&...args) {
        if (should_log(lvl)) {
            log_with_format_(source_loc{}, lvl, fmt, std::forward<Args>(args)...);
        }
    }
    // log with no format string, just string message
    void log(source_loc loc, level lvl, string_view_t msg) {
        if (should_log(lvl)) {
            sink_it_(details::log_msg(loc, name_, lvl, msg));
        }
    }

    void log(level lvl, string_view_t msg) {
        if (should_log(lvl)) {
            sink_it_(details::log_msg(source_loc{}, name_, lvl, msg));
        }
    }

    // support for custom time
    void log(log_clock::time_point log_time, source_loc loc, level lvl, string_view_t msg) {
        if (should_log(lvl)) {
            sink_it_(details::log_msg(log_time, loc, name_, lvl, msg));
        }
    }

    template <typename... Args>
    void trace(format_string_t<Args...> fmt, Args &&...args) {
        log(level::trace, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void debug(format_string_t<Args...> fmt, Args &&...args) {
        log(level::debug, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(format_string_t<Args...> fmt, Args &&...args) {
        log(level::info, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(format_string_t<Args...> fmt, Args &&...args) {
        log(level::warn, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(format_string_t<Args...> fmt, Args &&...args) {
        log(level::err, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void critical(format_string_t<Args...> fmt, Args &&...args) {
        log(level::critical, fmt, std::forward<Args>(args)...);
    }

    // log functions with no format string, just string
    void trace(string_view_t msg) { log(level::trace, msg); }
    void debug(string_view_t msg) { log(level::debug, msg); }
    void info(string_view_t msg) { log(level::info, msg); }
    void warn(string_view_t msg) { log(level::warn, msg); }
    void error(string_view_t msg) { log(level::err, msg); }
    void critical(string_view_t msg) { log(level::critical, msg); }

    // return true if logging is enabled for the given level.
    [[nodiscard]] bool should_log(level msg_level) const { return msg_level >= level_.load(std::memory_order_relaxed); }

    // set the level of logging
    void set_level(level level);

    // return the active log level
    [[nodiscard]] level log_level() const;

    // return the name of the logger
    [[nodiscard]] const std::string &name() const;

    // set formatting for the sinks in this logger.
    // each sink will get a separate instance of the formatter object.
    void set_formatter(std::unique_ptr<formatter> f);

    // set formatting for the sinks in this logger.
    // equivalent to
    //     set_formatter(make_unique<pattern_formatter>(pattern, time_type))
    // Note: each sink will get a new instance of a formatter object, replacing the old one.
    void set_pattern(std::string pattern, pattern_time_type time_type = pattern_time_type::local);

    // flush functions
    void flush();
    void flush_on(level level);
    [[nodiscard]] level flush_level() const;

    // sinks
    [[nodiscard]] const std::vector<sink_ptr> &sinks() const;
    [[nodiscard]] std::vector<sink_ptr> &sinks();

    // error handler
    void set_error_handler(err_handler);

    // create new logger with same sinks and configuration.
    std::shared_ptr<logger> clone(std::string logger_name);

private:
    std::string name_;
    std::vector<sink_ptr> sinks_;
    atomic_level_t level_{level::info};
    atomic_level_t flush_level_{level::off};
    details::err_helper err_helper_;

    // common implementation for after templated public api has been resolved to format string and
    // args
    template <typename... Args>
    void log_with_format_(source_loc loc, const level lvl, const format_string_t<Args...> &format_string, Args &&...args) {
        assert(should_log(lvl));
        try {
            memory_buf_t buf;
            fmt::vformat_to(std::back_inserter(buf), format_string, fmt::make_format_args(args...));
            sink_it_(details::log_msg(loc, name_, lvl, string_view_t(buf.data(), buf.size())));
        } catch (const std::exception &ex) {
            err_helper_.handle_ex(name_, loc, ex);
        } catch (...) {
            err_helper_.handle_unknown_ex(name_, loc);
        }
    }

    // log the given message (if the given log level is high enough)
    void sink_it_(const details::log_msg &msg) {
        assert(should_log(msg.log_level));
        for (auto &sink : sinks_) {
            if (sink->should_log(msg.log_level)) {
                try {
                    sink->log(msg);
                } catch (const std::exception &ex) {
                    err_helper_.handle_ex(name_, msg.source, ex);
                } catch (...) {
                    err_helper_.handle_unknown_ex(name_, msg.source);
                }
            }
        }

        if (should_flush_(msg)) {
            flush_();
        }
    }
    void flush_();
    [[nodiscard]] bool should_flush_(const details::log_msg &msg) const;
};

}  // namespace spdlog
