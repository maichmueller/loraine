cmake_minimum_required(VERSION 3.15)

set(LIBRARY_SOURCES
        ${LORAINE_SRC_DIR}/dummy.cpp
        ${LORAINE_SRC_DIR}/deck.cpp
        ${LORAINE_SRC_DIR}/event.cpp
        ${LORAINE_SRC_DIR}/event_register.cpp

        ${LORAINE_INCLUDE_DIR}/all.h

        ${LORAINE_INCLUDE_DIR}/types.h
        ${LORAINE_INCLUDE_DIR}/card.h
        ${LORAINE_INCLUDE_DIR}/agent.h
        ${LORAINE_INCLUDE_DIR}/board.h
        ${LORAINE_INCLUDE_DIR}/deck.h
        ${LORAINE_INCLUDE_DIR}/event.h
        ${LORAINE_INCLUDE_DIR}/event_register.h
        ${LORAINE_INCLUDE_DIR}/keywords.h
        ${LORAINE_INCLUDE_DIR}/region.h
        ${LORAINE_INCLUDE_DIR}/uuid_gen.h include/uuid_gen.h)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
add_library(loraine STATIC ${LIBRARY_SOURCES})

target_include_directories(loraine
        PUBLIC
        ${LORAINE_INCLUDE_DIR}
        ${CONAN_INCLUDE_DIRS_BOOST}
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

add_executable(goloraine ${LORAINE_DIR}/app/exec.cpp)
target_include_directories(goloraine PRIVATE ${LORAINE_INCLUDE_DIR} PUBLIC ${CONAN_INCLUDE_DIRS_BOOST})
target_link_libraries(goloraine loraine)
