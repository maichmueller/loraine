cmake_minimum_required(VERSION 3.15)

set(LIBRARY_SOURCES
        ${LORAINE_SRC_DIR}/deck.cpp
        ${LORAINE_SRC_DIR}/event_listener.cpp
        ${LORAINE_SRC_DIR}/state.cpp
        ${LORAINE_SRC_DIR}/game.cpp
        ${LORAINE_SRC_DIR}/board.cpp
        ${LORAINE_SRC_DIR}/target.cpp

        ${LORAINE_INCLUDE_DIR}/all.h

        ${LORAINE_INCLUDE_DIR}/types.h
        ${LORAINE_INCLUDE_DIR}/cards/card.h
        ${LORAINE_INCLUDE_DIR}/agent.h
        ${LORAINE_INCLUDE_DIR}/board.h
        ${LORAINE_INCLUDE_DIR}/deck.h
        ${LORAINE_INCLUDE_DIR}/event/event.h
        ${LORAINE_INCLUDE_DIR}/event/event_listener.h
        ${LORAINE_INCLUDE_DIR}/cards/keywords.h
        ${LORAINE_INCLUDE_DIR}/region.h
        ${LORAINE_INCLUDE_DIR}/uuid_gen.h
        ${LORAINE_INCLUDE_DIR}/action.h
        ${LORAINE_INCLUDE_DIR}/cards/effect.h
        ${LORAINE_INCLUDE_DIR}/rng_machine.h
        ${LORAINE_INCLUDE_DIR}/game.h
        ${LORAINE_INCLUDE_DIR}/event/active_event.h
        ${LORAINE_INCLUDE_DIR}/cards/common_effects.h
        ${LORAINE_INCLUDE_DIR}/cards/grant.h
        ${LORAINE_INCLUDE_DIR}/cards/target.h
        ${LORAINE_INCLUDE_DIR}/utils.h)

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

add_executable(makeitraine ${LORAINE_DIR}/app/exec.cpp)
target_include_directories(makeitraine PRIVATE ${LORAINE_INCLUDE_DIR} PUBLIC ${CONAN_INCLUDE_DIRS_BOOST})
target_link_libraries(makeitraine loraine)
