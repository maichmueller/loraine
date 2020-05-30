cmake_minimum_required(VERSION 3.15)

project(LORAINE VERSION 0.1 LANGUAGES CXX)
include(cmake/StandardProjectSettings.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message("C++ Compiler: ${CMAKE_CXX_COMPILER}")
message("C++ Compiler Info: ${CMAKE_CXX_COMPILER_ID}, version ${CMAKE_CXX_COMPILER_VERSION}")
message("CURRENT CMAKE BINARY DIR: ${CMAKE_CURRENT_BINARY_DIR}")
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
    if (ENABLE_BUILD_WITH_TIME_TRACE)
        add_compile_definitions(project_options INTERFACE -ftime-trace)
    endif()
endif()

# Later on it might become interesting to add a gui to the project
set(LORAINE_USE_IMGUI FALSE)

set(CMAKE_CONFIG_FOLDER "cmake")

# Set this path if the conan install is not in the standard directory
set(CONAN_PATH "~/anaconda3/bin/conan")


# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)
# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# enable cache system
include(${CMAKE_CONFIG_FOLDER}/Cache.cmake)

# standard compiler warnings
include(${CMAKE_CONFIG_FOLDER}/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(${CMAKE_CONFIG_FOLDER}/Sanitizers.cmake)
enable_sanitizers(project_options)

# enable doxygen
include(${CMAKE_CONFIG_FOLDER}/Doxygen.cmake)
enable_doxygen()

# allow for static analysis options
include(${CMAKE_CONFIG_FOLDER}/StaticAnalyzers.cmake)

option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
option(ENABLE_TESTING "Enable Test Builds" ON)
option(ENABLE_FUZZING "Enable Fuzzing Builds" OFF)

# Very basic PCH example
option(ENABLE_PCH "Enable Precompiled Headers" OFF)
if (ENABLE_PCH)
    # This sets a global PCH parameter, each project will build its own PCH, which
    # is a good idea if any #define's change
    #
    # consider breaking this out per project as necessary
    target_precompile_headers(project_options
            INTERFACE <vector> <string> <map> <utility> <memory> <array> <list> <stack>)
endif()

# Set up some extra Conan dependencies based on our needs
# before loading Conan
set(CONAN_EXTRA_REQUIRES "")
set(CONAN_EXTRA_OPTIONS "")

if(LORAINE_USE_IMGUI)
    set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES}
            imgui-sfml/2.1@bincrafters/stable)

    # set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sfml:shared=False
    # sfml:graphics=True sfml:audio=False sfml:window=True
    # libalsa:disable_python=True)
endif()


include(cmake/Conan.cmake)
run_conan()

if(ENABLE_TESTING)
    enable_testing()
    message(
            "Building Tests. Be sure to check out test/constexpr_tests for constexpr testing"
    )
    add_subdirectory(test)
endif()


set(LORAINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/loraine")
set(LORAINE_SRC_DIR "${LORAINE_DIR}/cppsrc")
set(LORAINE_INCLUDE_DIR ${LORAINE_DIR}/include)

message(STATUS "loraine project directory: ${LORAINE_DIR}")
message(STATUS "loraine include directory: ${LORAINE_INCLUDE_DIR}")
message(STATUS "loraine src directory: ${LORAINE_SRC_DIR}")

add_subdirectory(loraine)

set(LORAINE_LIBRARY ${CMAKE_SOURCE_DIR}/build/debug/${LORAINE_DIR}/libloraine.a)


