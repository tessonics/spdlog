include(FetchContent)

FetchContent_Declare(
    fmt
    DOWNLOAD_EXTRACT_TIMESTAMP FALSE
    URL https://github.com/fmtlib/fmt/archive/refs/tags/11.1.3.tar.gz
    URL_HASH SHA256=67cd23ea86ccc359693e2ce1ba8d1bab533c02d743c09b15f3131102d0c2fc1c)

FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
    # We do not require os features of fmt
    set(FMT_OS OFF CACHE BOOL "Disable FMT_OS" FORCE)
    FetchContent_MakeAvailable(fmt)
    set_target_properties(fmt PROPERTIES FOLDER "third-party")
endif()
