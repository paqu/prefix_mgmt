add_library(prefix_mgmt STATIC prefix_mgmt.c)
target_include_directories(prefix_mgmt PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_compile_options(prefix_mgmt
    PRIVATE
        -Wall -Wextra -Wpedantic
        "$<$<CONFIG:Release>:-O3>"
)

