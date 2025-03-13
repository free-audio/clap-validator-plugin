
include(FetchContent)

# Grab clap, clap-helpers and clap-wrapper
FetchContent_Declare(
        clap
        GIT_REPOSITORY https://github.com/free-audio/clap.git
        GIT_TAG main
)
FetchContent_Declare(
        clap-helpers
        GIT_REPOSITORY https://github.com/free-audio/clap-helpers.git
        GIT_TAG main
)
FetchContent_Declare(
        clap-wrapper
        GIT_REPOSITORY https://github.com/free-audio/clap-wrapper.git
        GIT_TAG main
)
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 11.1.4
)


set(CLAP_WRAPPER_DOWNLOAD_DEPENDENCIES TRUE CACHE BOOL "Get em")
set(CLAP_WRAPPER_DONT_ADD_TARGETS TRUE CACHE BOOL "I'll targetize")
set(CLAP_WRAPPER_BUILD_AUV2 TRUE CACHE BOOL "It's only logical")
if (APPLE)
    # BaconPaul has jack kinda installed
    set(RTAUDIO_API_JACK FALSE CACHE BOOL "Not on apple")
endif()

FetchContent_MakeAvailable(clap clap-helpers clap-wrapper fmt)
