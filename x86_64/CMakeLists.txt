add_executable(dmglda main.c)

target_link_libraries(dmglda PRIVATE dmgldacore)
target_link_libraries(dmglda PRIVATE SDL2)