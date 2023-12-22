#include "gb.h"
#include "ppu.h"
#include "rom.h"

gb_t *gb_init(void)
{
    gb_t *gb = malloc(sizeof(gb_t));
    if (!gb) {
        GB_Error("[ERROR] can't initialize the system.\n");
        exit(EXIT_FAILURE);
    }
    gb->quit = false;
    return gb;
}

void gb_deinit(gb_t *gb)
{
    clear_fifo(gb, FIFO_TYPE_BG_WIN);
    clear_fifo(gb, FIFO_TYPE_SPRITE);
    rom_unload(gb);
    free(gb->ppu.frame_buffer);
    free(gb);
}

void gb_init_state_after_bootrom(gb_t *gb)
{
    gb->cpu.regs.a = 0x01;
    gb->cpu.regs.f = 0xb0;
    gb->cpu.regs.b = 0x00;
    gb->cpu.regs.c = 0x13;
    gb->cpu.regs.d = 0x00;
    gb->cpu.regs.e = 0xd8;
    gb->cpu.regs.h = 0x01;
    gb->cpu.regs.l = 0x4d;
    gb->cpu.regs.sp = 0xfffe;
    gb->cpu.regs.pc = 0x0100;

    gb->intr.ime = false;
    gb->intr.intr_enable = 0;
    gb->intr.intr_flag = 0;

    gb->timer.div = 0xabcc;
    gb->timer.tima = 0;
    gb->timer.tma = 0;
    gb->timer.tac = 0;

    gb->ppu.tick = 0;
    gb->ppu.lcdc = 0x91;
    gb->ppu.scy = 0x00;
    gb->ppu.stat = 0x85;
    gb->ppu.scx = 0x00;
    gb->ppu.ly = 0;
    gb->ppu.lyc = 0;
    gb->ppu.dma_reg = 0xff;
    gb->ppu.bgp = 0xfc;
    gb->ppu.wy = 0;
    gb->ppu.wx = 0;

    gb->rom.boot_rom_unmapped = false;
    gb->rom.rom_loaded = false;

    gb->joypad.p1 = 0xff;
    gb->joypad.a = 1;
    gb->joypad.b = 1;
    gb->joypad.select = 1;
    gb->joypad.start = 1;
    gb->joypad.right = 1;
    gb->joypad.left = 1;
    gb->joypad.up = 1;
    gb->joypad.down = 1;
}