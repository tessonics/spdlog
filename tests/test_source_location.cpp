#ifndef SPDLOG_NO_SOURCE_LOC

#include "includes.h"
#include "spdlog/sinks/ostream_sink.h"
#include "test_sink.h"


using spdlog::details::os::default_eol;

TEST_CASE("test_source_location", "[source_location]") {
    std::ostringstream oss;
    auto oss_sink = std::make_shared<spdlog::sinks::ostream_sink_st>(oss);
    auto oss_logger = std::make_shared<spdlog::logger>("oss", oss_sink);
    //spdlog::logger oss_logger("oss", oss_sink);
    oss_logger->set_pattern("%s:%# %v");

    SPDLOG_LOGGER_CALL(oss_logger, spdlog::level::info, "Hello {}", "source location");
    REQUIRE(oss.str() == std::string("test_source_location.cpp:17 Hello source location") + default_eol);
}

#endif
