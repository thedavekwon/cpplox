add_executable(parser_test parser_test.cpp)

target_include_directories(parser_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(parser_test PRIVATE parser diagnostic Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(parser_test)
