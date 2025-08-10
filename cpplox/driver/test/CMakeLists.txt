add_executable(sample_test sample_test.cpp)

target_include_directories(sample_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(sample_test PRIVATE driver Catch2::Catch2WithMain)
target_compile_definitions(sample_test PRIVATE SAMPLE_DIR="${CMAKE_SOURCE_DIR}/sample")

include(CTest)
include(Catch)
catch_discover_tests(sample_test)
