

set(TEST_SOURCES
        )

set(TARGET_TEST_SOURCES
        dummy.cpp
        )

add_executable(tests ${TEST_SOURCES} ${TARGET_TEST_SOURCES})

target_include_directories(tests
        PRIVATE
        ${LORAINE_INCLUDE_DIR}
        )

set_target_properties(tests PROPERTIES
        CXX_STANDARD 17
        )

target_link_libraries(tests gtest_main)
#target_link_libraries(tests "${LORAINE_LIBRARY}")

add_test(NAME google_testing COMMAND ./tests)
