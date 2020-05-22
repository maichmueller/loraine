cmake_minimum_required(VERSION 3.15)

project(LORAINE VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message("C++ Compiler: ${CMAKE_CXX_COMPILER}")
if (UNIX)
    execute_process (
            COMMAND bash -c "gcc -dumpfullversion"
            OUTPUT_VARIABLE outVar
    )
endif ()

message("C++ Compiler Info: ${CMAKE_CXX_COMPILER_ID}, version ${CMAKE_CXX_COMPILER_VERSION}")
message("CURRENT CMAKE BINARY DIR: ${CMAKE_CURRENT_BINARY_DIR}")

set(LORAINE_FOLDER "loraine")

set(LORAINE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${LORAINE_FOLDER}/cppsrc")
set(LORAINE_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/${LORAINE_FOLDER}/include)
add_subdirectory(loraine)
set(LORAINE_LIBRARY ${CMAKE_SOURCE_DIR}/build/debug/${LORAINE_FOLDER}/libloraine.a)

add_executable(goloraine ${LORAINE_FOLDER}/app/exec.cpp)
target_include_directories(goloraine PRIVATE loraine)
target_link_libraries(goloraine loraine)

# Download and unpack googletest at configure time
enable_testing()

find_package(GTest)
if (NOT GTEST_FOUND)
    configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download)
    if (result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif ()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download)
    if (result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif ()

    # Prevent overriding the parent project's compiler/linker
    # settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
            ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
            EXCLUDE_FROM_ALL)
ENDIF()

find_package(GTest REQUIRED)


add_subdirectory(test)