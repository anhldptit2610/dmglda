#define _GNU_SOURCE
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
    uint32_t read_addr;

    if (addr >= 0x0000 && addr <= 0x3fff) {
        read_addr = (!(gb->rom.infos.bank_number >= 64 && gb->mbc.mbc1.banking_mode == 1)) ? addr :
                    addr + 0x4000 * (gb->mbc.mbc1.ram_bank_number << 5);
        ret = gb->rom.data[read_addr];
    } else if (addr >= 0x4000 && addr <= 0x7fff) {
        read_addr = (!(gb->rom.infos.bank_number >= 64)) ? (addr - 0x4000) + 0x4000 * gb->mbc.mbc1.rom_bank_number
                    : (addr - 0x4000) + 0x4000 * (gb->mbc.mbc1.rom_bank_number + (gb->mbc.mbc1.ram_bank_number << 5));
        ret = gb->rom.data[read_addr];
    } else if (addr >= 0xa000 && addr <= 0xbfff) {
        if (!gb->mbc.mbc1.ram_enable) {
            ret = 0xff;
        } else {
            if (gb->rom.infos.ram_size == 32 * KiB && gb->mbc.mbc1.banking_mode == 1)
                ret = gb->mbc.mbc1.ram[(addr - 0xa000) + 0x2000 * gb->mbc.mbc1.ram_bank_number];
            else
                ret = gb->mbc.mbc1.ram[addr - 0xa000];
        }
    }
    return ret;
}

void mbc1_write(gb_t *gb, uint16_t addr, uint8_t val)
{
    if (addr >= 0x0000 && addr <= 0x1fff) {
        gb->mbc.mbc1.ram_enable = ((val & 0x0f) == 0x0a) ? true : false;
    } else if (addr >= 0x2000 && addr <= 0x3fff) {
        uint8_t tmp = (!(val & 0x1f)) ? 1 : val & 0x1f;
        gb->mbc.mbc1.rom_bank_number = (tmp & 0b00011111) & mbc1_bit_mask[gb->rom.infos.bank_number];
    } else if (addr >= 0x4000 && addr <= 0x5fff) {
        gb->mbc.mbc1.ram_bank_number = val & 0x03;
    } else if (addr >= 0x6000 && addr <= 0x7fff) {
        gb->mbc.mbc1.banking_mode = val & 0x01;
    } else if (addr >= 0xa000 && addr <= 0xbfff) {
        if (!gb->mbc.mbc1.ram_enable)
            return;
        if (gb->rom.infos.ram_size == 32 * KiB && gb->mbc.mbc1.banking_mode == 1)
            gb->mbc.mbc1.ram[(addr - 0xa000) + 0x2000 * gb->mbc.mbc1.ram_bank_number] = val;
        else
            gb->mbc.mbc1.ram[addr - 0xa000] = val;
    }
}

uint8_t mbc_read(gb_t *gb, mbc_type_t mbc_type, uint16_t addr)
{
    uint8_t ret;

    switch (mbc_type) {
    case MBC0:
        ret = mbc0_read(gb, addr);        
        break;
    case MBC1_NONE:
    case MBC1_RAM:
    case MBC1_BATTERY_BUFFERED_RAM:
        if (gb->rom.infos.bank_number == 2)
            ret = mbc0_read(gb, addr);
        else
            ret = mbc1_read(gb, addr);
        break;
    default:
        break;
    }
}

void mbc_write(gb_t *gb, mbc_type_t mbc_type, uint16_t addr, uint8_t val)
{
    switch (mbc_type) {
    case MBC0:
        break;
    case MBC1_NONE:
    case MBC1_RAM:
    case MBC1_BATTERY_BUFFERED_RAM:
        mbc1_write(gb, addr, val);
        break;
    default:
        break;
    }
}

void mbc1_allocate_ram(gb_t *gb, uint32_t ram_size)
{
    gb->mbc.mbc1.ram = malloc(sizeof(uint8_t) * ram_size);
    if (!gb->mbc.mbc1.ram) {
        GB_Error("[ERROR] Can't allocate external RAM\n");
        exit(EXIT_FAILURE);
    } else {
        GB_Log("[LOG] External RAM allocated.\n");
    }
}

void mbc1_save_ram(gb_t *gb)
{
    char *save_file;

    asprintf(&save_file, "%s.sav", gb->rom.infos.name);
    FILE *fp = fopen(save_file, "w");
    if (!fp) {
        GB_Error("[ERROR Can't create save file\n");
        return;
    }
    fwrite(gb->mbc.mbc1.ram, sizeof(uint8_t), gb->rom.infos.ram_size, fp);
    fclose(fp);
}

void mbc1_load_ram(gb_t *gb)
{
    char *save_file;

    asprintf(&save_file, "%s.sav", gb->rom.infos.name);
    FILE *fp = fopen(save_file, "r");
    if (!fp) {
        GB_Log("[LOG] Save file for %s is unavailable.\n", gb->rom.infos.name);
        return;
    }
    fread(gb->mbc.mbc1.ram, sizeof(uint8_t), gb->rom.infos.ram_size, fp);
    fclose(fp);
}

void mbc_init(gb_t *gb)
{
    switch (gb->rom.infos.type) {
    case MBC0:
        break;
    case MBC1_NONE:
        gb->mbc.mbc1.rom_bank_number = 0;
        break;
    case MBC1_RAM:
        mbc1_allocate_ram(gb, gb->rom.infos.ram_size); 
        gb->mbc.mbc1.rom_bank_number = 0;
        break; 
    case MBC1_BATTERY_BUFFERED_RAM:
        mbc1_allocate_ram(gb, gb->rom.infos.ram_size); 
        gb->mbc.mbc1.rom_bank_number = 0;
        gb->mbc.mbc1.has_battery = true;
        gb->mbc.mbc1.need_save = false;
        mbc1_load_ram(gb);
        break;
    default:
        break;
    }
}

void mbc_save_data(gb_t *gb)
{
    if (gb->mbc.mbc1.has_battery)
        mbc1_save_ram(gb);
}