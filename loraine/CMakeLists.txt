cmake_minimum_required(VERSION 3.15)

set(LIBRARY_SOURCES
        ${LORAINE_SRC_DIR}/deck.cpp
        ${LORAINE_SRC_DIR}/action.cpp
        ${LORAINE_SRC_DIR}/action_invoker.cpp
        ${LORAINE_SRC_DIR}/event_types.cpp
        ${LORAINE_SRC_DIR}/event_listener.cpp
        ${LORAINE_SRC_DIR}/gamestate.cpp
        ${LORAINE_SRC_DIR}/config.cpp
        ${LORAINE_SRC_DIR}/nexus.cpp
        ${LORAINE_SRC_DIR}/toll.cpp
        ${LORAINE_SRC_DIR}/player.cpp
        ${LORAINE_SRC_DIR}/record.cpp

        ${LORAINE_SRC_DIR}/gamemode.cpp
        ${LORAINE_SRC_DIR}/logic.cpp
        ${LORAINE_SRC_DIR}/board.cpp
        ${LORAINE_SRC_DIR}/specific_effects.cpp
        ${LORAINE_SRC_DIR}/effectmap.cpp

        ${LORAINE_SRC_DIR}/card_defs.cpp
        ${LORAINE_SRC_DIR}/effect.cpp
        ${LORAINE_SRC_DIR}/targeting.cpp
        ${LORAINE_SRC_DIR}/concrete_effects.cpp

        ${LORAINE_SRC_DIR}/cardfactory.cpp

        ${LORAINE_SRC_DIR}/cardbase.cpp
        ${LORAINE_SRC_DIR}/fieldcard.cpp
        ${LORAINE_SRC_DIR}/unit.cpp
        ${LORAINE_SRC_DIR}/spell.cpp
        ${LORAINE_SRC_DIR}/landmark.cpp
        ${LORAINE_SRC_DIR}/trap.cpp

        ${LORAINE_SRC_DIR}/grant.cpp
        ${LORAINE_SRC_DIR}/grantmodifier.cpp

        )

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
target_sources(loraine PRIVATE ${LIBRARY_SOURCES})

target_include_directories(loraine
        PRIVATE
        ${LORAINE_INCLUDE_DIR}
        ${CONAN_INCLUDE_DIRS_STDUUID}
        ${CONAN_INCLUDE_DIRS_LIBUUID}
        ${CONAN_INCLUDE_DIRS_MS-GSL}
        )
target_include_directories(loraine
        SYSTEM INTERFACE
        ${LORAINE_INCLUDE_DIR}
        ${CONAN_INCLUDE_DIRS_STDUUID}
        ${CONAN_INCLUDE_DIRS_LIBUUID}
        ${CONAN_INCLUDE_DIRS_MS-GSL}
        )

set_target_properties(loraine PROPERTIES
        CXX_STANDARD 17
        )
target_compile_features(loraine PRIVATE cxx_std_17)

target_link_libraries(loraine PUBLIC project_options)
# set(SANITIZE_OPTIONS -fsanitize=address -fsanitize=undefined)
#set(SANITIZE_OPTIONS )
#add_compile_options(${SANITIZE_OPTIONS})
#add_link_options(${SANITIZE_OPTIONS})
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
# add_link_options(--verbose)

add_executable(makeitraine ${LORAINE_DIR}/app/makeitraine.cpp)
target_include_directories(makeitraine
        PRIVATE
        ${LORAINE_INCLUDE_DIR}
        ${CONAN_INCLUDE_DIRS_STDUUID}
        ${CONAN_INCLUDE_DIRS_MS-GSL}
        )
set_target_properties(makeitraine PROPERTIES
        CXX_STANDARD 17
        )
target_compile_features(makeitraine PRIVATE cxx_std_17)
target_link_libraries(makeitraine PRIVATE loraine project_options)
