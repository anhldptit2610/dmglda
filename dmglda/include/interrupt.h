#pragma once

#include "gb.h"
#include "cpu.h"

bool is_any_interrupt_pending(gb_t *gb);
void interrupt_write(gb_t *gb, uint16_t addr, uint8_t val);
uint8_t interrupt_read(gb_t *gb, uint16_t addr);
void interrupt_handler(gb_t *gb);
bool interrupt_check(gb_t *gb);
void interrupt_request(gb_t *gb, interrupt_source_t source);