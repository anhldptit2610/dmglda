#pragma once

#include "gb.h"
#include <SDL2/SDL.h>

int sdl_init(gb_t *gb);
void sdl_drawframe(gb_t *gb);
void sdl_cleanup(void);
void sdl_draw_bg_map(gb_t *gb);