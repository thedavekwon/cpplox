add_library(expr expr.cpp)

target_include_directories(expr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(expr PUBLIC scanner)

add_library(statement statement.cpp)

target_include_directories(statement PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(statement PUBLIC expr)
