#pragma once

#include "gb.h"
#include "interrupt.h"

void ppu_tick(gb_t *gb);
void ppu_write(gb_t *gb, uint16_t addr, uint8_t val);
uint8_t ppu_read(gb_t *gb, uint16_t addr);
void ppu_init(gb_t *gb);

/* only use for CPU */
void vram_write(gb_t *gb, uint16_t addr, uint8_t val);
uint8_t vram_read(gb_t *gb, uint16_t addr);

void clear_fifo(gb_t *gb, fifo_type_t fifo_type);