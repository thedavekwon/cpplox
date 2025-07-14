add_library(value value.cpp)

target_include_directories(value PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(value PUBLIC expr)

add_library(interpreter interpreter.cpp)

target_include_directories(interpreter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(interpreter PUBLIC expr value diagnostic)
