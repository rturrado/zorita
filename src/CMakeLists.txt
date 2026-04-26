find_package(fmt REQUIRED)

# Library
add_library(${PROJECT_NAME}-lib
    "${CMAKE_CURRENT_SOURCE_DIR}/Condition.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Decoder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Machine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Memory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Registers.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/StatusRegister.cpp"
)

target_compile_features(${PROJECT_NAME}-lib PRIVATE
    cxx_std_20
)

target_link_libraries(${PROJECT_NAME}-lib
    PUBLIC fmt::fmt
)

target_include_directories(${PROJECT_NAME}-lib
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
)

if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(${PROJECT_NAME}-lib PRIVATE
        -Wall -Wextra -Werror -Wfatal-errors
    )
else()
    message(SEND_ERROR "Unknown compiler!")
endif()

message(STATUS
    "[${PROJECT_NAME}] Target include directories:\n"
    "      CMAKE_CURRENT_SOURCE_DIR/include: ${CMAKE_CURRENT_SOURCE_DIR}/include\n"
    "      fmt_INCLUDE_DIRS: ${fmt_INCLUDE_DIRS}\n"
)

# Executable
add_executable(${PROJECT_NAME}
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE ${PROJECT_NAME}-lib
)
