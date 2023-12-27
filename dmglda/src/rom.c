#include "rom.h"

typedef struct ram_size {
    int size;
    char *name;
} ram_size_t;

ram_size_t external_ram_size[] = {
    {0,         "0"},
    {0,         "0"},
    {8 * KiB,   "8 KiB"},
    {32 * KiB,  "32 KiB"},
    {128 * KiB, "128 KiB"},
    {64 * KiB,  "64 KiB"},
};

static char *cart_types[] = {
	[0x00] = "ROM ONLY",
	[0x01] = "MBC1",
	[0x02] = "MBC1+RAM",
	[0x03] = "MBC1+RAM+BATTERY",
	[0x05] = "MBC2",
	[0x06] = "MBC2+BATTERY",
	[0x08] = "ROM+RAM",
	[0x09] = "ROM+RAM+BATTERY",
	[0x0B] = "MMM01",
	[0x0C] = "MMM01+RAM",
	[0x0D] = "MMM01+RAM+BATTERY",
	[0x0F] = "MBC3+TIMER+BATTERY",
	[0x10] = "MBC3+TIMER+RAM+BATTERY",
	[0x11] = "MBC3",
	[0x12] = "MBC3+RAM",
	[0x13] = "MBC3+RAM+BATTERY",
	[0x15] = "MBC4",
	[0x16] = "MBC4+RAM",
	[0x17] = "MBC4+RAM+BATTERY",
	[0x19] = "MBC5",
	[0x1A] = "MBC5+RAM",
	[0x1B] = "MBC5+RAM+BATTERY",
	[0x1C] = "MBC5+RUMBLE",
	[0x1D] = "MBC5+RUMBLE+RAM",
	[0x1E] = "MBC5+RUMBLE+RAM+BATTERY",
	[0xFC] = "POCKET CAMERA",
	[0xFD] = "BANDAI TAMA5",
	[0xFE] = "HuC3",
	[0xFF] = "HuC1+RAM+BATTERY",
};

static char *rom_size[] = {
    "32 KiB",
    "64 KiB",
    "128 KiB",
    "256 KiB",
    "512 KiB",
    "1 MiB",
    "2 MiB",
    "4 MiB",
    "8 MiB",
    "1.1 MiB",
    "1.2 MiB",
    "1.5 MiB",
};

static void handle_error(char *log)
{
    GB_Error(log);
    exit(EXIT_FAILURE);
}

void rom_load(gb_t *gb, char *rom_path)
{
    FILE *fp = fopen(rom_path, "r");
    struct stat statbuf;

    if (!fp)
        handle_error("[ERROR] Can't open the rom file\n");
    fseek(fp, 0, SEEK_END);
    gb->rom.rom_size = ftell(fp);
    rewind(fp);
    gb->rom.content = malloc(gb->rom.rom_size);
    fread(gb->rom.content, 1, gb->rom.rom_size, fp);
    fclose(fp);
    gb->rom.rom_loaded = true;
    GB_Log("[INFO] ROM has been loaded\n");    
}

void rom_unload(gb_t *gb)
{
    free(gb->rom.content);
    gb->rom.content = NULL;
}

void rom_get_info(gb_t *gb)
{
    for (uint16_t addr = 0x0134; addr <= 0x0143; addr++) {
        if (gb->rom.content[addr] == 0x00) {        // reach the end of the name
            gb->rom.title[addr - 0x0134] = '\0';
            break;
        } else {
            gb->rom.title[addr - 0x0134] = gb->rom.content[addr];
        }
    }
    for (uint16_t addr = 0x013f; addr <= 0x0142; addr++)
        gb->rom.manufacturer_code[addr - 0x013f] = gb->rom.content[addr];
    gb->rom.cgb_flag = gb->rom.content[0x0143];
    gb->rom.new_licensee_code[0] = gb->rom.content[0x0144];
    gb->rom.new_licensee_code[1] = gb->rom.content[0x0145];
    gb->rom.type = gb->rom.content[0x0147];
    gb->rom.rom_size = 32 * KiB * (1 << gb->rom.content[0x0148]);
    gb->rom.ram_size = external_ram_size[gb->rom.content[0x0149]].size;
    gb->rom.ram_size_index = gb->rom.content[0x0149];
    gb->rom.destination_code = gb->rom.content[0x014a];
    gb->rom.old_licensee_code = gb->rom.content[0x014b];
    gb->rom.mask_rom_version_number = gb->rom.content[0x014c];
    gb->rom.header_checksum = gb->rom.content[0x014d];
    gb->rom.global_checksum = U16(gb->rom.content[0x014f], gb->rom.content[0x014e]);
    gb->rom.rom_bank = gb->rom.rom_size / (16 * KiB);

    GB_Log("Cartridge informations\n");
    GB_Log("-------------------------------\n");
    GB_Log("Name: %s\n", gb->rom.title);
    GB_Log("New licensee code: %c%c\n", gb->rom.new_licensee_code[0], gb->rom.new_licensee_code[1]);
    GB_Log("SGB flag: 0x%02x\n", gb->rom.sgb_flag);
    GB_Log("Type: %s\n", cart_types[gb->rom.type]);
    GB_Log("ROM size: %s\n", rom_size[gb->rom.content[0x0148]]);
    GB_Log("ROM bank: %d\n", gb->rom.rom_bank);
    GB_Log("RAM size: %s\n", external_ram_size[gb->rom.ram_size_index].name);
    GB_Log("Destination code: 0x%02x\n", gb->rom.destination_code);
    GB_Log("Old licensee code: 0x%02x\n", gb->rom.old_licensee_code);
    GB_Log("Mask ROM version number: 0x%02x\n", gb->rom.mask_rom_version_number);
    GB_Log("Header checksum: 0x%02x\n", gb->rom.header_checksum);
    GB_Log("Global checksum: 0x%04x\n", gb->rom.global_checksum);
    GB_Log("-------------------------------\n");
}

uint8_t rom_read(gb_t *gb, uint16_t addr)
{
    return (!gb->rom.rom_loaded) ? 0xff : gb->rom.content[addr];
}