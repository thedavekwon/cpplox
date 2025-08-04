cmake_minimum_required(VERSION 3.10)
project(cpplox)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(cpplox/driver)
add_subdirectory(cpplox/scanner)
add_subdirectory(cpplox/diagnostic)
add_subdirectory(cpplox/ast)
add_subdirectory(cpplox/parser)
add_subdirectory(cpplox/env)

add_executable(cpplox_run cpplox/cpplox.cpp)
target_link_libraries(cpplox_run PRIVATE driver)
target_include_directories(cpplox_run PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/cpplox)

option(CPPLOX_BUILD_TESTS "Build cpplox tests" ON)

include(FetchContent)

# Fetch all dependencies
# Catch2
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
)
FetchContent_MakeAvailable(Catch2)

# magic_enum
FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.7
)
FetchContent_MakeAvailable(magic_enum)

if(CPPLOX_BUILD_TESTS)
    enable_testing()
    add_subdirectory(cpplox/scanner/test)
    # add_subdirectory(cpplox/ast/test)
    add_subdirectory(cpplox/parser/test)
    add_subdirectory(cpplox/env/test)
endif()
