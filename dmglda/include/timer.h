#pragma once

#include "gb.h"
#include "cpu.h"
#include "interrupt.h"

void timer_tick(gb_t *gb);
void timer_write(gb_t *gb, uint16_t addr, uint8_t val);
uint8_t timer_read(gb_t *gb, uint16_t addr);