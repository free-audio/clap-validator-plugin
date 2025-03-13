cmake_minimum_required(VERSION 3.28)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15 CACHE STRING "Build for 10.14")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


project(clap-validator-plugin VERSION 1.1.0.0 LANGUAGES C CXX)

option(USE_SANITIZER "Build and link with ASAN" FALSE)
option(COPY_AFTER_BUILD "Will copy after build" TRUE)
option(USE_CLAP_FROM_NEXT "Use 'next' versions of clap, clap-wrapper and clap-helpers" FALSE)

include(cmake/compile-options.cmake)
include(cmake/fetch-deps.cmake)

add_library(${PROJECT_NAME}-impl STATIC src/cvp-entry-impl.cpp src/cvp-plugin.cpp)
target_include_directories(${PROJECT_NAME}-impl PUBLIC src)
target_link_libraries(${PROJECT_NAME}-impl PUBLIC clap)
target_link_libraries(${PROJECT_NAME}-impl PRIVATE clap-helpers fmt::fmt-header-only)

if (WIN32)
    target_compile_definitions(${PROJECT_NAME}-impl PRIVATE WIN32_GUI)
    target_sources(${PROJECT_NAME}-impl PRIVATE src/gui/gui-unimpl.cpp)
elseif (APPLE)
    target_compile_definitions(${PROJECT_NAME}-impl PRIVATE COCOA_GUI)
    target_sources(${PROJECT_NAME}-impl PRIVATE src/gui/gui-macos.mm)
    target_link_libraries(${PROJECT_NAME}-impl PRIVATE "-framework Cocoa" "-framework CoreFoundation")
else()
    target_compile_definitions(${PROJECT_NAME}-impl PRIVATE X11_GUI)
    target_sources(${PROJECT_NAME}-impl PRIVATE src/gui/gui-unimpl.cpp)
endif()

make_clapfirst_plugins(
        TARGET_NAME ${PROJECT_NAME}
        IMPL_TARGET ${PROJECT_NAME}-impl

        OUTPUT_NAME "${PRODUCT_NAME}"

        ENTRY_SOURCE src/cvp-entry.cpp

        BUNDLE_IDENTIFER "org.clever-audio.clap-validator-plugin"
        BUNDLE_VERSION ${PROJECT_VERSION}

        COPY_AFTER_BUILD ${COPY_AFTER_BUILD}

        PLUGIN_FORMATS CLAP # VST3 AUV2

        ASSET_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_assets

        # STANDALONE_CONFIGURATIONS
        #    standalone-stereo-generator "ClapValidatorStandalone" "org.free-audio.cvp.stereo-generator"
)


