// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <string>
#include <chrono>
#include <exception>
#include <mutex>
#include "spdlog/common.h"

// by default, prints the error to stderr, at max rate of 1/sec thread safe
namespace spdlog {
namespace details {
class SPDLOG_API err_helper {
    err_handler custom_err_handler_;
    std::chrono::steady_clock::time_point last_report_time_;
    std::mutex mutex_;
public:
    err_helper() = default;
    err_helper(const err_helper& other);
    err_helper(err_helper&& other);
    void handle_ex(const std::string& origin, const source_loc& loc, const std::exception& ex) noexcept;
    void handle_unknown_ex(const std::string& origin, const source_loc& loc) noexcept;
    void set_err_handler(err_handler handler);
};
} // namespace details
} // namespace spdlog
