include(FetchContent)

FetchContent_Declare(
    fmt
    DOWNLOAD_EXTRACT_TIMESTAMP FALSE
    URL https://github.com/fmtlib/fmt/archive/refs/tags/11.1.2.tar.gz
    URL_HASH SHA256=d8773cf062cc806d4dd4df658111f15ba7a2c9c65db5084d2491696828b1eb97)

FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
    # We do not require os features of fmt
    set(FMT_OS OFF CACHE BOOL "Disable FMT_OS" FORCE)
    FetchContent_MakeAvailable(fmt)
    set_target_properties(fmt PROPERTIES FOLDER "third-party")
endif()
