cmake_minimum_required(VERSION 3.22)

if (POLICY CMP0102)
    cmake_policy(SET CMP0102 NEW)
endif()
if (POLICY CMP0126)
    cmake_policy(SET CMP0126 NEW)
endif()
cmake_policy(SET CMP0128 NEW)
if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

project("zorita"
    VERSION 1.0
	DESCRIPTION "A RISC machine implementation"
    HOMEPAGE_URL https://github.com/rturrado/zorita.git
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_SHARED_LIBS
    "Whether the library should be built as a shared object or as a static library"
    OFF
)
option(ZORITA_BUILD_TESTS
    "Whether the tests should be built and added to `make test`"
    OFF
)

add_subdirectory(src)

if(ZORITA_BUILD_TESTS)
    enable_testing()
    include(CTest)
    include(GoogleTest)
    set(CMAKE_CTEST_ARGUMENTS "--output-on-failure")
    add_subdirectory(test)
endif()
