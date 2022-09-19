PROJECT(mytest)

aux_source_directory(. SRC)

add_library(mytest ${SRC})
