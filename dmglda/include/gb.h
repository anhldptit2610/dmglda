#pragma once

#include "defs.h"

typedef struct gb {
    uint8_t mem[0x10000];
    bool quit;
    struct cpu {
        struct cpu_reg {
            uint8_t a;
            uint8_t b;
            uint8_t c;
            uint8_t d;
            uint8_t e;
            uint8_t h;
            uint8_t l;
            uint8_t f;
            uint16_t sp;
            uint16_t pc;
        } regs; 
        uint8_t mode;
        uint8_t opcode;
        uint8_t operand1;
        uint8_t operand2;
        uint16_t opcode_pc;
    } cpu;

    struct rom {
        struct rom_info {
            char name[17];
            uint8_t manufacturer_code[4];
            uint8_t cgb_flag;
            uint8_t new_licensee_code[2];
            uint8_t sgb_flag;
            uint8_t type;
            uint8_t destination_code : 1;
            uint8_t old_licensee_code;
            uint8_t mask_rom_version_number;
            uint8_t header_checksum;
            uint16_t global_checksum;
            uint32_t size;
            uint8_t bank_number;
            uint32_t ram_size;
            uint8_t ram_size_index;
        } infos;
        uint8_t *data;
        bool boot_rom_unmapped;
        bool rom_loaded;
    } rom;

    struct interrupt {
        bool ime;
        uint8_t intr_flag;
        uint8_t intr_enable;
        bool ime_ready_on;
    } intr;

    struct timer {
        uint16_t div;
        uint8_t tima;
        uint8_t tma;
        uint8_t tac : 3;
        bool tima_overflow;
        uint8_t ticks_after_tima_overflow;
    } timer;

    struct ppu {
        uint8_t lcdc;
        uint8_t stat;
        uint8_t scy;
        uint8_t scx;
        uint8_t ly;
        uint8_t lyc;
        uint8_t dma_reg;
        uint8_t bgp;
        uint8_t obp0;
        uint8_t obp1;
        uint8_t wy;
        uint8_t wx;
        int tick;
        bool need_refresh;
        ppu_mode_t mode;

        /* background/window components */
        pixel_fifo_t bg_win_fifo;
        pixel_fetcher_t bg_win_pf;
        fifo_entry_t bg_win_pixel;
        bool draw_window;
        uint16_t window_line_cnt;
        bool draw_window_this_line;
        bool this_frame_has_window;

        /* sprite components */
        pixel_fifo_t sprite_fifo;
        pixel_fetcher_t sprite_pf;
        fifo_entry_t sprite_pixel;
        int rendered_sprites;

        /* oam components */
        oam_entry_t oam_buffer[10];
        uint16_t current_oam_entry;
        uint16_t oam_buffer_size;

        bool pixel_shifter_active;

        uint8_t current_x;
        unsigned int *frame_buffer;
        uint8_t discard_bits;
        bool stat_interrupt_line;

        bool first_frame_passed;

        int ticks_after_line_153;
        bool reach_line_153;
    } ppu;

    struct dma {
        uint8_t reg;
        int tick;
        uint8_t mode;
        uint16_t start_addr;
        uint8_t current_transfer_byte;
    } dma;

    struct joypad {
        uint8_t p1;
        bool a;
        bool b; 
        bool select;
        bool start;
        bool right;
        bool left;
        bool up;
        bool down;
    } joypad;

    struct apu {
        uint8_t nr52;
    } apu;

    struct mbc {
        struct mbc1 {
            bool ram_enable;
            uint8_t rom_bank_number;
            uint8_t ram_bank_number;
            uint8_t banking_mode;
            uint8_t *ram;
            bool has_battery;
            bool need_save;
        } mbc1;
    } mbc;
} gb_t;

gb_t *gb_init(void);
void gb_destroy(gb_t *gb);
void gb_init_state_after_bootrom(gb_t *gb);