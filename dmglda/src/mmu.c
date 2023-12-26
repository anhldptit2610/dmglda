#include "mmu.h"

static unsigned const char bootrom[256] =
{
    0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,
    0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0,
    0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
    0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
    0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
    0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
    0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
    0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
    0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xE2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
    0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
    0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
    0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
    0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
    0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3C, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x3C,
    0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20, 0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x20,
    0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x20, 0xFE, 0x3E, 0x01, 0xE0, 0x50
};

void hram_write(gb_t *gb, uint16_t addr, uint8_t val)
{
    gb->mem[addr] = val;
}

uint8_t hram_read(gb_t *gb, uint16_t addr)
{
    return gb->mem[addr];
}

void io_write(gb_t *gb, uint16_t addr, uint8_t val)
{
    if (addr == INTERRUPT_IE_REGISTER || addr == INTERRUPT_IF_REGISTER) {
        interrupt_write(gb, addr, val);
    } else if (addr == TIMER_DIV_REGISTER || addr == TIMER_TAC_REGISTER || addr == TIMER_TIMA_REGISTER ||
            addr == TIMER_TMA_REGISTER) {
        timer_write(gb, addr, val);
    } else if (addr == PPU_LCDC_REGISTER || addr == PPU_STAT_REGISTER || addr == PPU_SCY_REGISTER ||
            addr == PPU_SCX_REGISTER || addr == PPU_LYC_REGISTER ||
            addr == PPU_BGP_REGISTER || addr == PPU_OBP0_REGISTER || addr == PPU_OBP1_REGISTER ||
            addr == PPU_WY_REGISTER || addr == PPU_WX_REGISTER) {
        ppu_write(gb, addr, val);
    } else if (addr == DMA_REGISTER) {
        gb->dma.reg = val;
        gb->dma.mode = DMA_MODE_SETUP;
    } else if (addr == JOYPAD_JOYP_REGISTER)  {
        joypad_write(gb, val);
    } else if (addr == 0xff50 && val == 1) {
        gb->rom.boot_rom_unmapped = true;
    } 
}

uint8_t io_read(gb_t *gb, uint16_t addr)
{
    uint8_t ret;

    if (addr == INTERRUPT_IE_REGISTER || addr == INTERRUPT_IF_REGISTER) {
        ret = interrupt_read(gb, addr);
    } else if (addr == TIMER_DIV_REGISTER || addr == TIMER_TAC_REGISTER || addr == TIMER_TIMA_REGISTER ||
            addr == TIMER_TMA_REGISTER) {
        ret = timer_read(gb, addr);
    } else if (addr == PPU_LCDC_REGISTER || addr == PPU_STAT_REGISTER || addr == PPU_SCY_REGISTER ||
            addr == PPU_SCX_REGISTER || addr == PPU_LY_REGISTER ||
            addr == PPU_BGP_REGISTER || addr == PPU_OBP0_REGISTER || addr == PPU_OBP1_REGISTER ||
            addr == PPU_WY_REGISTER || addr == PPU_WX_REGISTER) {
        ret = ppu_read(gb, addr);
    } else if (addr == DMA_REGISTER) {
        ret = gb->dma.reg;
    } else if (addr == JOYPAD_JOYP_REGISTER) {
        ret = joypad_read(gb);
    } else {
        ret = 0xff;
    }
    return ret;
}

void oam_write(gb_t *gb, uint16_t addr, uint8_t val)
{
    if (gb->ppu.mode == PPU_MODE_HBLANK || gb->ppu.mode == PPU_MODE_VBLANK)
        gb->mem[addr] = val; 
}

uint8_t oam_read(gb_t *gb, uint16_t addr)
{
    uint8_t ret = 0xff;

    if (gb->ppu.mode == PPU_MODE_HBLANK || gb->ppu.mode == PPU_MODE_VBLANK)
        ret = gb->mem[addr];
    return ret;
}

void vram_write(gb_t *gb, uint16_t addr, uint8_t val)
{
    if (gb->ppu.mode != PPU_MODE_DRAWING)
        gb->mem[addr] = val;
}

uint8_t vram_read(gb_t *gb, uint16_t addr)
{
    uint8_t ret = 0xff;

    if (gb->ppu.mode != PPU_MODE_DRAWING)
        ret = gb->mem[addr];
    return ret;
}

void wram_write(gb_t *gb, uint16_t addr, uint8_t val)
{
    gb->mem[addr] = val;
}

uint8_t wram_read(gb_t *gb, uint16_t addr)
{
    uint8_t ret;

    ret = gb->mem[addr];
    return ret;
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

uint8_t mmu_read(gb_t *gb, uint16_t addr)
{
    uint8_t ret;

    if (addr >= 0xe000 && addr <= 0xfdff)
        addr -= 0x2000;

    if (addr == 0xffff)
        ret = interrupt_read(gb, addr);
    else if (addr >= HRAM_START_ADDR && addr <= HRAM_END_ADDR)
        ret = hram_read(gb, addr);
    else if (addr >= 0xff00 && addr <= 0xff7f)
        ret = io_read(gb, addr);
    else if (addr >= 0xfea0 && addr <= 0xfeff)
        ret = 0xff;
    else if (addr >= OAM_START_ADDR && addr <= OAM_END_ADDR)
        ret = oam_read(gb, addr);
    else if (addr >= WRAM_START_ADDR && addr <= WRAM_END_ADDR)
        ret = wram_read(gb, addr);
    else if (addr >= VRAM_START_ADDR && addr <= VRAM_END_ADDR)
        ret = vram_read(gb, addr);
    else if (addr < 0x8000 && gb->rom.boot_rom_unmapped)
        ret = mbc_read(gb, gb->rom.infos.type, addr);
    else if (addr >= 0x100 && addr < 0x8000 && !gb->rom.boot_rom_unmapped)
        ret = rom_read(gb, addr);
    else if (addr >= 0x0000 && addr <= 0x00ff && !gb->rom.boot_rom_unmapped)
        ret = bootrom[addr];
    else
        ret = 0xff;
    return ret;
}

void mmu_write(gb_t *gb, uint16_t addr, uint8_t val)
{
    if (addr >= 0xe000 && addr <= 0xfdff)
        addr -= 0x2000;

    if (addr == 0xffff)
        interrupt_write(gb, addr, val);
    else if (addr >= HRAM_START_ADDR && addr <= HRAM_END_ADDR)
        hram_write(gb, addr, val);
    else if (addr >= 0xff00 && addr <= 0xff7f)
        io_write(gb, addr, val);
    else if (addr >= 0xfea0 && addr <= 0xfeff)
        return;
    else if (addr >= OAM_START_ADDR && addr <= OAM_END_ADDR)
        oam_write(gb, addr, val);
    else if (addr >= WRAM_START_ADDR && addr <= WRAM_END_ADDR)
        wram_write(gb, addr, val);
    else if (addr >= VRAM_START_ADDR && addr <= VRAM_END_ADDR)
        vram_write(gb, addr, val);
    else if (addr < 0x8000)
        mbc_write(gb, gb->rom.infos.type, addr, val);
}