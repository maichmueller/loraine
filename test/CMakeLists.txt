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
#add_library(catch_main STATIC catch_main.cpp)
#target_link_libraries(catch_main PUBLIC CONAN_PKG::gtest)
#target_link_libraries(catch_main PRIVATE project_options)

add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE project_warnings project_options
                            PUBLIC CONAN_PKG::gtest)


# Add a file containing a set of constexpr tests
add_executable(constexpr_tests constexpr_tests.cpp)
target_link_libraries(constexpr_tests PRIVATE project_options project_warnings
                                              catch_main)

# Disable the constexpr portion of the test, and build again this allows us to
# have an executable that we can debug when things go wrong with the constexpr
# testing
add_executable(relaxed_constexpr_tests constexpr_tests.cpp)
target_link_libraries(
  relaxed_constexpr_tests PRIVATE project_options project_warnings
                                  catch_main)
target_compile_definitions(
  relaxed_constexpr_tests PRIVATE
                                  -DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE)

