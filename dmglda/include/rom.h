#pragma once

#include "gb.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>

void rom_load(gb_t *gb, char *rom_path);
void rom_unload(gb_t *gb);
void rom_get_info(gb_t *gb);
uint8_t rom_read(gb_t *gb, uint16_t addr);