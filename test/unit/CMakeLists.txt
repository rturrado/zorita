find_package(GTest REQUIRED)

set(TARGET ${PROJECT_NAME}-unit-test)

add_executable(${TARGET}
    "${CMAKE_CURRENT_SOURCE_DIR}/Condition.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Decoder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Machine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Memory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Registers.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/StatusRegister.cpp"
)

target_compile_features(${TARGET} PRIVATE
    cxx_std_20
)


target_link_libraries(${TARGET}
    PRIVATE ${PROJECT_NAME}-lib
    PRIVATE GTest::gmock
    PRIVATE GTest::gtest_main
)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${TARGET} PRIVATE
        -Wall -Wextra -Werror -Wfatal-errors
    )
elseif(MSVC)
    target_compile_options(${TARGET} PRIVATE
        /WX
    )
else()
    message(SEND_ERROR "Unknown compiler!")
endif()

gtest_discover_tests(${TARGET}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
