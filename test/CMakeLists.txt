# automatically enable gtest to generate test targets
#if(CONAN_GTEST_ROOT_DEBUG)
#  include(${CONAN_GTEST_ROOT_DEBUG}/lib/cmake/GTest/GTest.cmake)
#else()
#  include(${CONAN_GTEST_ROOT}/lib/cmake/GTest/GTest.cmake)
#endif()

find_package("GTest")


set(TEST_SOURCES
        tests.cpp
        test_board.cpp
        test_cards.h)

add_executable(tests ${TEST_SOURCES})
target_include_directories(tests
        PRIVATE ${LORAINE_INCLUDE_DIR})
#target_include_directories(tests
#        SYSTEM PRIVATE ${CONAN_INCLUDE_DIRS_STDUUID} ${CONAN_INCLUDE_DIRS_MS-GSL}
#        )
target_include_directories(tests PRIVATE ${GTEST_INCLUDE_DIRS})
  #  target_link_libraries(enough ${GTEST_LIBRARIES})


target_link_libraries(tests PRIVATE project_warnings project_options
                            SYSTEM PRIVATE CONAN_PKG::gtest loraine ${CONAN_LIBRARY_DIRS_MS-GSL})

