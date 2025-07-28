add_library(object object.cpp)

target_include_directories(object PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(object PUBLIC expr)

add_library(interpreter interpreter.cpp)

target_include_directories(interpreter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(interpreter PUBLIC expr statement object diagnostic)
