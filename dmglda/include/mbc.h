#pragma once

#include "gb.h"
#include "rom.h"

uint8_t mbc0_read(gb_t *gb, uint16_t addr);
uint8_t mbc1_read(gb_t *gb, uint16_t addr);
void mbc1_write(gb_t *gb, uint16_t addr, uint8_t val);
