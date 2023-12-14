#pragma once

#include "gb.h"
#include "mmu.h"
#include <stdio.h>

#define Disassembler_Log(...)   fprintf(file, __VA_ARGS__)

void disassembler_cb_write(FILE *file, gb_t *gb);
void disassembler_write(gb_t *gb);