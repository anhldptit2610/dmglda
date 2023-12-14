#pragma once

#include "gb.h"
#include "interrupt.h"
#include "timer.h"
#include "ppu.h"
#include "rom.h"
#include "dma.h"

uint8_t mmu_read(gb_t *gb, uint16_t addr);
void mmu_write(gb_t *gb, uint16_t addr, uint8_t val);