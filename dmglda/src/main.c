#include "defs.h"
#include "cpu.h"
#include "rom.h"
#include "sdl.h"

int main(int argc, char **argv)
{
    gb_t *gb = gb_init();

    uint64_t start_frame_time, end_frame_time;
    float elapsed_ms;

    sdl_init(gb);
    ppu_init(gb);
    if (argc == 2) {
        rom_load(gb, argv[1]);
        rom_get_info(gb);
    }
    while (!gb->quit) {
        start_frame_time = SDL_GetTicks64();
        sdl_handle_input(gb);
        while (gb->ppu.need_refresh == false)
            cpu_step(gb);
        sdl_drawframe(gb);
        end_frame_time = SDL_GetTicks64();
        elapsed_ms = (float)(end_frame_time - start_frame_time);
        SDL_Delay(16.67f > elapsed_ms ? 16.67f - elapsed_ms : 0);
    }
    gb_deinit(gb);
    sdl_cleanup();
    SDL_Quit();
    return 0;
}