PROJECT(main)

aux_source_directory(. SRC)

add_executable(main ${SRC})

target_link_libraries(main mytest)
