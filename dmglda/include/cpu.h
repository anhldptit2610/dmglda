#pragma once

#include "gb.h"
#include "mmu.h"
#include "interrupt.h"
#include "timer.h"
#include "ppu.h"
#include "dma.h"
#include "sdl.h"

void cpu_tick(gb_t *gb);
void cpu_cycle(gb_t *gb);
void cpu_step(gb_t *gb);
void stack_push(gb_t *gb, uint16_t val);
uint16_t stack_pop(gb_t *gb);
void cpu_init(gb_t *gb);