# Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test executable
add_executable(test_runner
    test_add.cpp
    test_check.cpp
    test_del.cpp
    test_integration.cpp
    test_utils.cpp
)

target_include_directories(test_runner 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(test_runner
    PRIVATE
    prefix_mgmt
    GTest::gtest
    GTest::gtest_main
)

target_compile_options(test_runner 
    PRIVATE 
        -Wall -Wextra -Wpedantic
        "$<$<CONFIG:Debug>:-O0>"
        "$<$<CONFIG:Debug>:-g>"
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(test_runner)
