find_package(GTest REQUIRED)

function(add_zorita_test TARGET)
    add_executable(${TARGET} ${ARGN})

    target_compile_features(${TARGET} PRIVATE
        cxx_std_20
    )

    target_compile_definitions(${TARGET} PRIVATE
        RESOURCES_DIR="${PROJECT_SOURCE_DIR}/res"
    )

    target_link_libraries(${TARGET}
        PRIVATE ${PROJECT_NAME}-lib
        PRIVATE GTest::gmock
        PRIVATE GTest::gtest_main
    )

    if(CMAKE_COMPILER_IS_GNUCXX)
        target_compile_options(${TARGET} PRIVATE
            -Wall -Wextra -Werror -Wfatal-errors
    )
    else()
        message(SEND_ERROR "Unknown compiler!")
    endif()

    gtest_discover_tests(${TARGET}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
endfunction()

add_subdirectory(integration)
add_subdirectory(unit)
