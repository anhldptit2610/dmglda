project(dmgldacore)

add_library(${PROJECT_NAME} SHARED src/cpu.c
                                    src/mmu.c
                                    src/rom.c
                                    src/system.c
                                    src/interrupt.c
                                    src/timer.c
                                    src/ppu.c
                                    src/sdl.c
                                    src/disassembler.c
                                    src/dma.c
                                    src/joypad.c
                                    src/mbc.c)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(dmglda src/main.c)

target_link_libraries(dmglda PRIVATE ${PROJECT_NAME})
target_link_libraries(dmglda PRIVATE SDL2)