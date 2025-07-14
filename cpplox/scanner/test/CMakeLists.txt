add_executable(scanner_test scanner_test.cpp)

target_include_directories(scanner_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(scanner_test PRIVATE scanner diagnostic Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(scanner_test)
