#pragma once

#include "gb.h"
#include "interrupt.h"

void joypad_write(gb_t *gb, uint8_t val);
uint8_t joypad_read(gb_t *gb);
void joypad_press_button(gb_t *gb, uint8_t key);
void joypad_release_button(gb_t *gb, uint8_t key);

