add_library(prefix_mgmt STATIC prefix_mgmt.c)

target_include_directories(prefix_mgmt PUBLIC 
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(prefix_mgmt
    PRIVATE
        -Wall -Wextra -Wpedantic -Werror
        "$<$<CONFIG:Release>:-O3>"
        "$<$<CONFIG:Debug>:-O0>"
        "$<$<CONFIG:Debug>:-g>"
)
if(ENABLE_COVERAGE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(prefix_mgmt PRIVATE --coverage)
    target_link_options(prefix_mgmt PUBLIC --coverage)
    
    target_compile_options(prefix_mgmt PRIVATE 
        -fprofile-arcs 
        -ftest-coverage
    )
endif()
