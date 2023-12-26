add_executable(cpu_test cpu_test.c)
target_link_libraries(cpu_test PRIVATE cjson
                                       dmgldacore 
                                       SDL2)