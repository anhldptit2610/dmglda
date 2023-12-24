#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define BIT(b)          (1U << b)  
#define KiB             1024
#define MiB             1048576
#define MSB(n)      (((uint16_t)(n) >> 8) & 0x00ff)
#define LSB(n)      ((uint16_t)(n) & 0x00ff)
#define U16(lsb, msb)   ((uint16_t)(msb) << 8) | (uint16_t)(lsb)
#define GB_Log(...)    fprintf(stdout, __VA_ARGS__)
#define GB_Error(...)  fprintf(stderr, __VA_ARGS__)

#define SCREEN_WIDTH                160
#define SCREEN_HEIGHT               144

#define VRAM_START_ADDR             0x8000
#define VRAM_END_ADDR               0x9fff

#define WRAM_START_ADDR             0xc000
#define WRAM_END_ADDR               0xdfff

#define OAM_START_ADDR              0xfe00
#define OAM_END_ADDR                0xfe9f

#define HRAM_START_ADDR             0xff80
#define HRAM_END_ADDR               0xfffe

#define JOYPAD_JOYP_REGISTER        0xff00

#define SERIAL_SB_REGISTER          0xff01
#define SERIAL_SC_REGISTER          0xff02

#define TIMER_DIV_REGISTER          0xff04
#define TIMER_TIMA_REGISTER         0xff05
#define TIMER_TMA_REGISTER          0xff06
#define TIMER_TAC_REGISTER          0xff07

#define INTERRUPT_VBLANK_VECTOR     0x0040
#define INTERRUPT_LCD_VECTOR        0x0048
#define INTERRUPT_TIMER_VECTOR      0x0050
#define INTERRUPT_SERIAL_VECTOR     0x0058
#define INTERRUPT_JOYPAD_VECTOR     0x0060
#define INTERRUPT_IF_REGISTER       0xff0f
#define INTERRUPT_IE_REGISTER       0xffff

#define APU_NR10_REGISTER           0xff10
#define APU_NR11_REGISTER           0xff11
#define APU_NR12_REGISTER           0xff12
#define APU_NR13_REGISTER           0xff13
#define APU_NR14_REGISTER           0xff14
#define APU_NR21_REGISTER           0xff16
#define APU_NR22_REGISTER           0xff17
#define APU_NR23_REGISTER           0xff18
#define APU_NR24_REGISTER           0xff19
#define APU_NR30_REGISTER           0xff1a
#define APU_NR31_REGISTER           0xff1b
#define APU_NR32_REGISTER           0xff1c
#define APU_NR33_REGISTER           0xff1d
#define APU_NR34_REGISTER           0xff1e
#define APU_NR41_REGISTER           0xff20
#define APU_NR42_REGISTER           0xff21
#define APU_NR43_REGISTER           0xff22
#define APU_NR44_REGISTER           0xff23
#define APU_NR50_REGISTER           0xff24
#define APU_NR51_REGISTER           0xff25
#define APU_NR52_REGISTER           0xff26

#define WAVE_RAM_START_ADDR         0xff30
#define WAVE_RAM_END_ADDR           0xff3f

#define PPU_LCDC_REGISTER           0xff40
#define PPU_STAT_REGISTER           0xff41
#define PPU_SCY_REGISTER            0xff42
#define PPU_SCX_REGISTER            0xff43
#define PPU_LY_REGISTER             0xff44
#define PPU_LYC_REGISTER            0xff45
#define PPU_BGP_REGISTER            0xff47
#define PPU_OBP0_REGISTER           0xff48
#define PPU_OBP1_REGISTER           0xff49
#define PPU_WY_REGISTER             0xff4a
#define PPU_WX_REGISTER             0xff4b

#define DMA_REGISTER                0xff46

typedef enum CPU_R {
    CPU_R_B,
    CPU_R_C,
    CPU_R_D,
    CPU_R_E,
    CPU_R_H,
    CPU_R_L,
    CPU_R_F,
    CPU_R_A,
} cpu_r_t;

typedef enum CPU_RR {
    CPU_RR_BC,
    CPU_RR_DE,
    CPU_RR_HL,
    CPU_RR_AF,
    CPU_RR_SP,
    CPU_RR_PC,
} cpu_rr_t;

typedef enum CPU_FLAG {
    CPU_FLAG_Z = (1U << 7),
    CPU_FLAG_N = (1U << 6),
    CPU_FLAG_H = (1U << 5),
    CPU_FLAG_C = (1U << 4),
} cpu_flag_t;

typedef enum CPU_FLAG_COND {
    CPU_FLAG_COND_NZ,
    CPU_FLAG_COND_Z,
    CPU_FLAG_COND_NC,
    CPU_FLAG_COND_C,
} cpu_flag_cond_t;

typedef enum INTERRUPT_SOURCE {
    INTERRUPT_SOURCE_VBLANK = (1U << 0),
    INTERRUPT_SOURCE_LCD = (1U << 1),
    INTERRUPT_SOURCE_TIMER = (1U << 2),
    INTERRUPT_SOURCE_SERIAL = (1U << 3),
    INTERRUPT_SOURCE_JOYPAD = (1U << 4),
} interrupt_source_t;

typedef enum PPU_MODE {
    PPU_MODE_HBLANK,                // mode 0
    PPU_MODE_VBLANK,                // mode 1
    PPU_MODE_OAM_SCAN,              // mode 2
    PPU_MODE_DRAWING,               // mode 3
} ppu_mode_t;

typedef enum CPU_MODE {
    CPU_MODE_NORMAL,
    CPU_MODE_HALT,
    CPU_MODE_HALT_BUG,
    CPU_MODE_STOP,
    CPU_MODE_SET_IME,
} cpu_mode_t;

typedef enum DMA_MODE {
    DMA_MODE_OFF,
    DMA_MODE_SETUP,
    DMA_MODE_TRANSFER,
} dma_mode_t;

typedef enum PIXEL_FETCHER_STATE {
    PF_STATE_GET_TILE,
    PF_STATE_GET_TILEDATA0,
    PF_STATE_GET_TILEDATA1,
    PF_STATE_PUSH,
} pf_state_t;

enum LCDC_BITS {
    LCDC_LCD_POWER = (1U << 7),
    LCDC_WINDOW_TILE_MAP = (1U << 6),
    LCDC_WINDOW_ENABLE = (1U << 5),
    LCDC_BG_WINDOW_TILESET = (1U << 4),
    LCDC_BG_TILE_MAP = (1U << 3),
    LCDC_SPRITE_SIZE = (1U << 2),
    LCDC_SPRITES_ENABLED = (1U << 1),
    LCDC_BG_ENABLED = (1U << 0),
};

enum STAT_BITS {
    STAT_LYC = (1U << 6),
    STAT_MODE2 = (1U << 5),
    STAT_MODE1 = (1U << 4),
    STAT_MODE0 = (1U << 3),
    STAT_LYC_EQUAL_LY = (1U << 2),
};

enum OAM_ATTRIBUTE {
    OAM_ATTRIBUTE_PRIORITY = (1U << 7),
    OAM_ATTRIBUTE_Y_FLIP = (1U << 6),
    OAM_ATTRIBUTE_X_FLIP = (1U << 5),
    OAM_ATTRIBUTE_DMG_PALETTE = (1U << 4),
};

enum SPRITE_PRIORITY {
    SPRITE_PRIORITY_NONE = 0xff,
};

enum BACKGROUND_PRIORITY {
    BG_PRIORITY_NONE = 0xff,
};

typedef enum PALETTE {
    PALETTE_BGP,
    PALETTE_OBP0,
    PALETTE_OBP1,
} palette_t;

typedef enum FIFO_TYPE {
    FIFO_TYPE_SPRITE,
    FIFO_TYPE_BG_WIN,
} fifo_type_t;

enum COLOR {
    COLOR_WHITE,
    COLOR_LIGHT_GRAY,
    COLOR_DARK_GRAY,
    COLOR_BLACK,
};

typedef enum MBC_TYPE {
    MBC0,
    MBC1_NONE,
    MBC1_RAM,
    MBC1_BATTERY_BUFFERED_RAM,
} mbc_type_t;

typedef enum JOYPAD_MODE {
    JOYPAD_MODE_BUTTONS = (1U << 5),
    JOYPAD_MODE_D_PAD = (1U << 4),
    JOYPAD_MODE_RELEASE,
} joypad_mode_t;

enum JOYPAD_KEYS {
    JOYPAD_A = (1U << 0),
    JOYPAD_B = (1U << 1),
    JOYPAD_SELECT = (1U << 2),
    JOYPAD_START = (1U << 3),
    JOYPAD_RIGHT = (1U << 4),
    JOYPAD_LEFT = (1U << 5),
    JOYPAD_UP = (1U << 6),
    JOYPAD_DOWN = (1U << 7),
};

typedef struct fifo_entry {
    uint8_t color : 2;
    uint8_t palette;
    uint8_t sprite_priority;
    uint8_t bg_priority;
    struct fifo_entry *next;
} fifo_entry_t;

typedef struct pixel_fifo {
    fifo_entry_t *head;
    fifo_entry_t *tail;
    int size;
} pixel_fifo_t;

typedef struct pixel_fetcher {
    uint8_t state;
    uint8_t tile_number;
    uint8_t tile_data_low;
    uint8_t tile_data_high;
    bool reset_status;
    bool is_active;
    int x;                  // fetcher's X counter
} pixel_fetcher_t;

typedef struct oam_entry {
    uint8_t y_pos;
    uint8_t x_pos;
    uint8_t tile_index;
    uint8_t attribute;
    bool rendered;
} oam_entry_t;