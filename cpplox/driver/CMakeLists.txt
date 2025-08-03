add_library(driver driver.cpp)

target_link_libraries(driver PUBLIC expr diagnostic interpreter parser resolver scanner)
