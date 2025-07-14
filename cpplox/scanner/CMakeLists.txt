add_library(scanner scanner.cpp)

target_include_directories(scanner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(scanner PUBLIC magic_enum::magic_enum)
