#pragma once

#include "gb.h"
#include "rom.h"

uint8_t mbc0_read(gb_t *gb, uint16_t addr);
uint8_t mbc1_read(gb_t *gb, uint16_t addr);
void mbc1_write(gb_t *gb, uint16_t addr, uint8_t val);
void mbc_write(gb_t *gb, mbc_type_t mbc_type, uint16_t addr, uint8_t val);
uint8_t mbc_read(gb_t *gb, mbc_type_t mbc_type, uint16_t addr);
void mbc_init(gb_t *gb);
void mbc_save_data(gb_t *gb);