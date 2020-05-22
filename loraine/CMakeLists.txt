cmake_minimum_required(VERSION 3.15)

set(LIBRARY_SOURCES
        ${LORAINE_SRC_DIR}/dummy.cpp

        ${LORAINE_INCLUDE_DIR}/types.h
        )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

add_library(loraine STATIC ${LIBRARY_SOURCES})
target_include_directories(loraine
        PUBLIC
        ${LORAINE_INCLUDE_DIR}
        )

set_target_properties(loraine PROPERTIES
        CXX_STANDARD 17
        )

target_compile_features(loraine PRIVATE cxx_std_17)

# set(SANITIZE_OPTIONS -fsanitize=address -fsanitize=undefined)
#set(SANITIZE_OPTIONS )
#add_compile_options(${SANITIZE_OPTIONS})
#add_link_options(${SANITIZE_OPTIONS})
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
# add_link_options(--verbose)

