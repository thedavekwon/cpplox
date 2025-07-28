add_executable(env_test env_test.cpp)

target_include_directories(env_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(env_test PRIVATE interpreter object Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(env_test)
