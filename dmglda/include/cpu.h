#pragma once

#include "gb.h"
#include "mmu.h"
#include "interrupt.h"
#include "timer.h"
#include "ppu.h"
#include "dma.h"

void cpu_tick(gb_t *gb);
void cpu_tick4(gb_t *gb);
void cpu_step(gb_t *gb);
void cpu_stack_push(gb_t *gb, uint16_t val);
uint16_t cpu_stack_pop(gb_t *gb);