add_executable(${PROJECT_NAME}-unit-test
    "${CMAKE_CURRENT_SOURCE_DIR}/Condition.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Decoder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Instruction.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Machine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Memory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Registers.cpp"
)

target_compile_features(${PROJECT_NAME}-unit-test PRIVATE
    cxx_std_20
)

target_link_libraries(${PROJECT_NAME}-unit-test
    PRIVATE ${PROJECT_NAME}-lib
    PRIVATE GTest::gmock
    PRIVATE GTest::gtest_main
)

if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(${PROJECT_NAME}-unit-test PRIVATE
        -Wall -Wextra -Werror -Wfatal-errors
    )
else()
    message(SEND_ERROR "Unknown compiler!")
endif()

message(STATUS
    "[${PROJECT_NAME}] Target include directories:\n"
    "      GTest_INCLUDE_DIRS: ${GTest_INCLUDE_DIRS}\n"
)

gtest_discover_tests(${PROJECT_NAME}-unit-test
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
