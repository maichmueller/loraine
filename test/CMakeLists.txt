# automatically enable gtest to generate test targets
#if(CONAN_GTEST_ROOT_DEBUG)
#  include(${CONAN_GTEST_ROOT_DEBUG}/lib/cmake/GTest/GTest.cmake)
#else()
#  include(${CONAN_GTEST_ROOT}/lib/cmake/GTest/GTest.cmake)
#endif()

find_package("GTest")

if(GTEST_FOUND)
  include_directories(${GTEST_INCLUDE_DIRS})
#  target_link_libraries(enough ${GTEST_LIBRARIES})
ENDIF()

add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE project_warnings project_options
                            PUBLIC CONAN_PKG::gtest)

