#include "mbc.h"

static uint8_t mbc1_bit_mask[] = {
    [2]   = 0b00000001,
    [4]   = 0b00000011,
    [8]   = 0b00000111,
    [16]  = 0b00001111,
    [32]  = 0b00011111,
    [64]  = 0b00011111,
    [128] = 0b00011111,
};

uint8_t mbc0_read(gb_t *gb, uint16_t addr)
{
    return rom_read(gb, addr);
}

// TODO: currently MBC1 not support ROMs whose ROM bank is over 128

uint8_t mbc1_read(gb_t *gb, uint16_t addr)
{
    uint8_t ret;

    if (addr >= 0x0000 && addr <= 0x3fff) {
        ret = gb->rom.content[addr];
    } else if (addr >= 0x4000 && addr <= 0x7fff) {
        ret = gb->rom.content[0x4000 * gb->mbc.mbc1.rom_bank_number + (addr - 0x4000)];
    } else if (addr >= 0xa000 && addr <= 0xbfff) {
        if (!gb->mbc.mbc1.ram_enable)
            ret = 0xff;
    }

    return ret;
}

void mbc1_write(gb_t *gb, uint16_t addr, uint8_t val)
{
    if (addr >= 0x0000 && addr <= 0x1fff) {
        gb->mbc.mbc1.ram_enable = ((val & 0x0f) == 0xa) ? true : false;
    } else if (addr >= 0x2000 && addr <= 0x3fff) {
        uint8_t tmp = (!(val & 0x1f)) ? (val & 0x1f) + 1 : val & 0x1f;
        tmp &= mbc1_bit_mask[gb->rom.rom_bank];
        gb->mbc.mbc1.rom_bank_number = tmp;
    } else if (addr >= 0x4000 && addr <= 0x5fff) {
        gb->mbc.mbc1.ram_bank_number = val & 0x03;
    } else if (addr >= 0x6000 && addr <= 0x7fff) {
        gb->mbc.mbc1.banking_mode = val & 0x01;
    }

}
