find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME}-test
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

target_compile_features(${PROJECT_NAME}-test PRIVATE
    cxx_std_20
)

target_link_libraries(${PROJECT_NAME}-test
    PRIVATE ${PROJECT_NAME}-lib
    PRIVATE GTest::gmock
    PRIVATE GTest::gtest
)

if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(${PROJECT_NAME}-test PRIVATE
        -Wall -Wextra -Werror -Wfatal-errors
    )
else()
    message(SEND_ERROR "Unknown compiler!")
endif()

message(STATUS
    "[${PROJECT_NAME}] Target include directories:\n"
    "      GTest_INCLUDE_DIRS: ${GTest_INCLUDE_DIRS}\n"
)

gtest_discover_tests(${PROJECT_NAME}-test
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
