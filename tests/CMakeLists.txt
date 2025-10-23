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
)

target_link_libraries(test_runner
    PRIVATE
    prefix_mgmt
    GTest::gtest
    GTest::gtest_main
)

target_compile_options(test_runner PRIVATE -Wall -Wextra)

# Discover tests
include(GoogleTest)
gtest_discover_tests(test_runner)
