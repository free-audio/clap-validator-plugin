cmake_minimum_required(VERSION 3.28)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14 CACHE STRING "Build for 10.14")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


project(clap-validator-plugin VERSION 1.1.0.0 LANGUAGES C CXX)

option(USE_SANITIZER "Build and link with ASAN" FALSE)
option(COPY_AFTER_BUILD "Will copy after build" TRUE)
option(USE_CLAP_FROM_NEXT "Use 'next' versions of clap, clap-wrapper and clap-helpers" FALSE)

include(cmake/compile-options.cmake)

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


set(CLAP_WRAPPER_DOWNLOAD_DEPENDENCIES TRUE CACHE BOOL "Get em")
set(CLAP_WRAPPER_DONT_ADD_TARGETS TRUE CACHE BOOL "I'll targetize")
set(CLAP_WRAPPER_BUILD_AUV2 TRUE CACHE BOOL "It's only logical")
if (APPLE)
    # BaconPaul has jack kinda installed
    set(RTAUDIO_API_JACK FALSE CACHE BOOL "Not on apple")
endif()

FetchContent_MakeAvailable(clap clap-helpers clap-wrapper)

add_library(${PROJECT_NAME}-impl STATIC src/cvp-entry-impl.cpp)
target_include_directories(${PROJECT_NAME}-impl PUBLIC src)
target_link_libraries(${PROJECT_NAME}-impl PUBLIC clap)
target_link_libraries(${PROJECT_NAME}-impl PRIVATE clap-helpers)


make_clapfirst_plugins(
        TARGET_NAME ${PROJECT_NAME}
        IMPL_TARGET ${PROJECT_NAME}-impl

        OUTPUT_NAME "${PRODUCT_NAME}"

        ENTRY_SOURCE src/cvp-entry.cpp

        BUNDLE_IDENTIFER "org.clever-audio.clap-validator-plugin"
        BUNDLE_VERSION ${PROJECT_VERSION}

        COPY_AFTER_BUILD ${COPY_AFTER_BUILD}

        PLUGIN_FORMATS CLAP VST3 AUV2

        ASSET_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_assets
)


