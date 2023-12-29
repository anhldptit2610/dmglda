project(dmgldacore)

add_library(${PROJECT_NAME} SHARED src/cpu.c
                                    src/mmu.c
                                    src/rom.c
                                    src/system.c
                                    src/interrupt.c
                                    src/timer.c
                                    src/ppu.c
                                    src/sdl.c
                                    src/dma.c
                                    src/joypad.c
                                    src/mbc.c)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)