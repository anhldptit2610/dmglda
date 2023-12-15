// TODO: implement DMA bus conflict after sprite rendering running

#include "ppu.h"

// rgba format
uint32_t default_color[] = { 0x9bbc0fff, 0x8bac0fff, 0x306230ff, 0x0f380fff };

void ppu_do_set_mode(gb_t *gb, ppu_mode_t mode)
{
    gb->ppu.mode = mode;
    gb->ppu.stat = (gb->ppu.stat & 0xfc) | mode;
}

uint32_t ppu_do_get_color_from_palette(gb_t *gb, palette_t palette, uint8_t color)
{
    uint8_t id;

    switch (palette) {
    case PALETTE_BGP:
        id = (gb->ppu.bgp >> (color * 2)) & 0x03;
        break;
    case PALETTE_OBP0:
        id = (gb->ppu.obp0 >> (color * 2)) & 0x03;
        break;
    case PALETTE_OBP1:
        id = (gb->ppu.obp1 >> (color * 2)) & 0x03;
        break;
    default:
        break;
    }
    return default_color[id];
}

void ppu_do_fifo_clear(gb_t *gb, fifo_type_t fifo_type)
{
    pixel_fifo_t *fifo = (fifo_type == FIFO_TYPE_BG_WIN) ? &gb->ppu.bg_window_fifo : &gb->ppu.sprite_fifo;

    if (fifo->size > 0) {
        fifo_entry_t *tmp = NULL;
        while (fifo->head->next != NULL) {
            tmp = fifo->head;
            fifo->head = fifo->head->next;
            free(tmp);
            fifo->size--;
        }
        free(fifo->head);
        fifo->size--;
        tmp = NULL;
        fifo->head = NULL;
        fifo->tail = NULL;
    }
}

void ppu_do_prepare_for_mode3(gb_t *gb)
{
    gb->ppu.bg_window_pf.reset_status = true;
    gb->ppu.bg_window_pf.is_active = true;
    gb->ppu.bg_window_pf.state = 0;
    gb->ppu.bg_window_pf.x = 0;

    gb->ppu.sprite_pf.state = 0;
    gb->ppu.sprite_pf.is_active = false;

    for (int i = 0; i < gb->ppu.oam_buffer_size; i++)
        gb->ppu.oam_buffer[i].rendered = false;

    gb->ppu.current_x = 0;
    ppu_do_fifo_clear(gb, FIFO_TYPE_BG_WIN);
    ppu_do_fifo_clear(gb, FIFO_TYPE_SPRITE);
}

fifo_entry_t *ppu_do_fifo_create_new_entry(gb_t *gb, uint8_t color, uint8_t palette,
                                                    uint8_t sprite_priority, uint8_t bg_priority)
{
    fifo_entry_t *new = malloc(sizeof(fifo_entry_t));

    if (!new)
        GB_Log("[ERROR] ppu_do_fifo_create_new_entry - malloc failed\n");
    new->next = NULL;
    new->color = color;
    new->palette = palette;
    new->sprite_priority = sprite_priority;
    new->bg_priority = bg_priority;
    return new;
}

void ppu_do_fifo_push_a_pixel(gb_t *gb, fifo_type_t fifo_type, uint8_t color, uint8_t palette,
                                        uint8_t sprite_priority, uint8_t bg_priority)
{
    pixel_fifo_t *fifo = (fifo_type == FIFO_TYPE_BG_WIN) ? &gb->ppu.bg_window_fifo : &gb->ppu.sprite_fifo;

    if (fifo->size > 8) {
        GB_Log("[FAILED] FIFO size is full\n");
        return;
    }
    fifo_entry_t *new = ppu_do_fifo_create_new_entry(gb, color, palette, sprite_priority, bg_priority);

    if (!new)
        return;
    if (!fifo->size) {
        fifo->head = fifo->tail = new;
    } else {
        fifo->tail->next = new;
        fifo->tail = new;
    }
    fifo->size++;
}

int ppu_do_fifo_pop_a_pixel(gb_t *gb, fifo_type_t fifo_type)
{
    fifo_entry_t *tmp = NULL;
    fifo_entry_t *pixel = (fifo_type == FIFO_TYPE_BG_WIN) ? &gb->ppu.bg_window_pixel : &gb->ppu.sprite_pixel;
    pixel_fifo_t *fifo = (fifo_type == FIFO_TYPE_BG_WIN) ? &gb->ppu.bg_window_fifo : &gb->ppu.sprite_fifo;

    if (fifo->size <= 0) {
        return -1;
    } else {
        pixel->color = fifo->head->color;
        pixel->palette = fifo->head->palette;
        pixel->sprite_priority = fifo->head->sprite_priority;
        pixel->bg_priority = fifo->head->bg_priority;
        pixel->next = NULL;
        tmp = fifo->head;
        fifo->head = fifo->head->next;
        free(tmp);
        tmp = NULL;
        fifo->size--;
    }
    return 0;
}

void ppu_do_oam_scan(gb_t *gb)
{
    static int i;
    uint16_t oam_entry_addr, sprite_height;

    if (gb->ppu.tick % 2 == 0) {
        oam_entry_addr = OAM_START_ADDR + i * 4;
        sprite_height = (gb->ppu.lcdc & LCDC_SPRITE_SIZE) ? 16 : 8;
        if (gb->mem[oam_entry_addr + 1] > 0 &&
            gb->ppu.ly + 16 >= gb->mem[oam_entry_addr] &&
            gb->ppu.ly + 16 < gb->mem[oam_entry_addr] + sprite_height &&
            gb->ppu.oam_buffer_size < 10) {
                gb->ppu.oam_buffer[gb->ppu.oam_buffer_size].y_pos = gb->mem[oam_entry_addr];
                gb->ppu.oam_buffer[gb->ppu.oam_buffer_size].x_pos = gb->mem[oam_entry_addr + 1];
                gb->ppu.oam_buffer[gb->ppu.oam_buffer_size].tile_index = gb->mem[oam_entry_addr + 2];
                gb->ppu.oam_buffer[gb->ppu.oam_buffer_size].attribute = gb->mem[oam_entry_addr + 3];
                gb->ppu.oam_buffer_size++;
        }
        i++;
    }
    if (gb->ppu.tick == 80) {
        ppu_do_set_mode(gb, PPU_MODE_DRAWING);
        ppu_do_prepare_for_mode3(gb);
        gb->ppu.discard_bits = gb->ppu.scx % 8;
        i = 0;
    }
}

int ppu_do_bg_fifo_push_pixels(gb_t *gb)
{
    uint8_t low_bit, high_bit, palette, bg_priority;

    if (gb->ppu.bg_window_fifo.size > 0)
        return 1;
    for (int i = 0; i < 8; i++) {

        low_bit = (gb->ppu.bg_window_pf.tile_data_low >> (7 - i)) & 0x01;
        high_bit = (gb->ppu.bg_window_pf.tile_data_high >> (7 - i)) & 0x01;
        ppu_do_fifo_push_a_pixel(gb, FIFO_TYPE_BG_WIN, (high_bit << 1) | low_bit, palette,
                                    SPRITE_PRIORITY_NONE, BACKGROUND_PRIORITY_NONE);
    }
    return 0;
}

int ppu_do_sprite_fifo_push_pixels(gb_t *gb)
{
    uint8_t low_bit, high_bit, palette, bg_priority, offset;
    int current_oam_entry = gb->ppu.current_oam_entry;

    palette = (gb->ppu.oam_buffer[current_oam_entry].attribute & OAM_ATTRIBUTE_DMG_PALETTE) ? PALETTE_OBP1 : PALETTE_OBP0;
    bg_priority = (gb->ppu.oam_buffer[current_oam_entry].attribute & OAM_ATTRIBUTE_PRIORITY) ? 1 : 0;
    for (int i = 0; i < 8; i++) {
        offset = 7 - i;
        if (gb->ppu.oam_buffer[gb->ppu.current_oam_entry].attribute & OAM_ATTRIBUTE_X_FLIP)
            offset = i;
        else
            offset = 7 - i;
        low_bit = (gb->ppu.sprite_pf.tile_data_low >> (offset)) & 0x01;
        high_bit = (gb->ppu.sprite_pf.tile_data_high >> (offset)) & 0x01;
        ppu_do_fifo_push_a_pixel(gb, FIFO_TYPE_SPRITE, (high_bit << 1) | low_bit, palette,
                                    SPRITE_PRIORITY_NONE, bg_priority);
    }
    return 0;
}

void ppu_do_fetch_bg_window_pixels(gb_t *gb)
{
    uint16_t addr, x_offset, y_offset, offset;

    if (!gb->ppu.bg_window_pf.is_active)
        return;
    switch (gb->ppu.bg_window_pf.state) {
    case 0:   
        addr = (gb->ppu.lcdc & LCDC_BG_TILE_MAP) ? 0x9c00 : 0x9800;
        x_offset = (gb->ppu.bg_window_pf.x + gb->ppu.scx / 8) & 0x1f;
        y_offset = 32 * (((gb->ppu.ly + gb->ppu.scy) & 0xff) / 8);
        addr += (x_offset + y_offset) & 0x3ff;
        gb->ppu.bg_window_pf.tile_number = gb->mem[addr];
        gb->ppu.bg_window_pf.state++;
        break;
    case 1:
        gb->ppu.bg_window_pf.state++;
        break;
    case 2:
        addr = (gb->ppu.lcdc & LCDC_BG_WINDOW_TILESET) ? 0x8000 : 0x9000;
        offset = 2 * ((gb->ppu.ly + gb->ppu.scy) % 8);
        addr = (addr == 0x8000) ? addr + 16 * (uint8_t)gb->ppu.bg_window_pf.tile_number 
                                : addr + 16 * (int8_t)gb->ppu.bg_window_pf.tile_number;
        gb->ppu.bg_window_pf.tile_data_low = gb->mem[addr + offset];
        gb->ppu.bg_window_pf.state++;
        break;
    case 3:
        gb->ppu.bg_window_pf.state++;
        break;
    case 4:
        addr = (gb->ppu.lcdc & LCDC_BG_WINDOW_TILESET) ? 0x8000 : 0x9000;
        offset = 2 * ((gb->ppu.ly + gb->ppu.scy) % 8);
        addr = (addr == 0x8000) ? addr + 16 * (uint8_t)gb->ppu.bg_window_pf.tile_number 
                                : addr + 16 * (int8_t)gb->ppu.bg_window_pf.tile_number;
        gb->ppu.bg_window_pf.tile_data_high = gb->mem[addr + offset + 1];
        if (gb->ppu.bg_window_pf.reset_status) {     /* if it's the first time of a scanline, reset to step 1. */
            gb->ppu.bg_window_pf.state = 0;
            gb->ppu.bg_window_pf.reset_status = false;
        } else
            gb->ppu.bg_window_pf.state++;
        break;
    case 5:
        gb->ppu.bg_window_pf.state++;
        break;
    case 6:
        if (!ppu_do_bg_fifo_push_pixels(gb)) {
            gb->ppu.bg_window_pf.state++;
            gb->ppu.bg_window_pf.x++;
        }
        break;
    case 7:
        gb->ppu.bg_window_pf.state = 0;
        break;
    default:
        break;
    }
}

void ppu_do_fetch_sprite_pixels(gb_t *gb)
{
    uint16_t addr, current_oam_entry, offset, sprite_height;

    sprite_height = (gb->ppu.lcdc & LCDC_SPRITE_SIZE) ? 16 : 8;
    if (!gb->ppu.sprite_pf.is_active)
        return;
    switch (gb->ppu.sprite_pf.state) {
    case 0:   
        current_oam_entry = gb->ppu.current_oam_entry;
        gb->ppu.sprite_pf.tile_number = gb->ppu.oam_buffer[current_oam_entry].tile_index;
        gb->ppu.sprite_pf.state++;
        break;
    case 1:
        gb->ppu.sprite_pf.state++;
        break;
    case 2:
        offset = 2 * ((gb->ppu.ly + gb->ppu.scy) % 8);
        if (gb->ppu.oam_buffer[gb->ppu.current_oam_entry].attribute & OAM_ATTRIBUTE_Y_FLIP)
            offset = 2 * (sprite_height - 1 - (gb->ppu.ly + gb->ppu.scy) % 8);
        addr = 0x8000 + 16 * (uint8_t)gb->ppu.sprite_pf.tile_number;
        gb->ppu.sprite_pf.tile_data_low = gb->mem[addr + offset];
        gb->ppu.sprite_pf.state++;
        break;
    case 3:
        gb->ppu.sprite_pf.state++;
        break;
    case 4:
        offset = 2 * ((gb->ppu.ly + gb->ppu.scy) % 8);
        if (gb->ppu.oam_buffer[gb->ppu.current_oam_entry].attribute & OAM_ATTRIBUTE_Y_FLIP)
            offset = 2 * (sprite_height - 1 - (gb->ppu.ly + gb->ppu.scy) % 8);
        addr = 0x8000 + 16 * (uint8_t)gb->ppu.sprite_pf.tile_number;
        gb->ppu.sprite_pf.tile_data_high = gb->mem[addr + offset + 1];
        gb->ppu.sprite_pf.state++;
        break;
    case 5:
        gb->ppu.sprite_pf.state++;
        break;
    case 6:
        if (!ppu_do_sprite_fifo_push_pixels(gb)) {
            gb->ppu.sprite_pf.state++;
            gb->ppu.bg_window_pf.is_active = true;
        }
        break;
    case 7:
        gb->ppu.sprite_pf.state = 0;
        gb->ppu.sprite_pf.is_active = false;
        break;
    default:
        break;
    }
}

int ppu_do_poll_oam_buffer(gb_t *gb)
{
    int ret = -1;

    for (int i = 0; i < gb->ppu.oam_buffer_size; i++) {
        if ((gb->ppu.oam_buffer[i].x_pos <= gb->ppu.current_x + 8) &&
            (gb->ppu.oam_buffer[i].rendered == false)) {
            gb->ppu.current_oam_entry = i;
            gb->ppu.oam_buffer[i].rendered = true;
            ret = 0;
            break;
        }
    }
    return ret;
}

void ppu_do_fetch_pixels(gb_t *gb)
{
    if (!ppu_do_poll_oam_buffer(gb)) {
        gb->ppu.bg_window_pf.is_active = false;
        gb->ppu.sprite_pf.is_active = true;
        gb->ppu.sprite_pf.state = gb->ppu.bg_window_pf.state = 0;
    }
    ppu_do_fetch_sprite_pixels(gb);
    ppu_do_fetch_bg_window_pixels(gb);
}

void ppu_do_output_pixel(gb_t *gb)
{
    int ret1, ret2;
    static int i = 0;
    unsigned int color;

    ret1 = ppu_do_fifo_pop_a_pixel(gb, FIFO_TYPE_BG_WIN);
    if (!ret1) {
        if (gb->ppu.discard_bits > 0) {
            gb->ppu.discard_bits--;
            return;
        }
        ret2 = ppu_do_fifo_pop_a_pixel(gb, FIFO_TYPE_SPRITE);
        if (!ret2) {
            if ((!gb->ppu.sprite_pixel.color) ||
                (gb->ppu.sprite_pixel.bg_priority && gb->ppu.bg_window_pixel.color > 0)) {
                color = ppu_do_get_color_from_palette(gb, PALETTE_BGP, gb->ppu.bg_window_pixel.color);
                gb->ppu.frame_buffer[gb->ppu.ly * 160 + gb->ppu.current_x] = 
                        (gb->ppu.lcdc & LCDC_BG_ENABLED) ? color : default_color[0];
            } else {
                color = ppu_do_get_color_from_palette(gb, gb->ppu.sprite_pixel.palette, gb->ppu.sprite_pixel.color);
                gb->ppu.frame_buffer[gb->ppu.ly * 160 + gb->ppu.current_x] = 
                        (gb->ppu.lcdc & LCDC_SPRITES_ENABLED) ? color : 
                            ppu_do_get_color_from_palette(gb, PALETTE_BGP, gb->ppu.bg_window_pixel.color);
            }
        } else {
            unsigned int color = ppu_do_get_color_from_palette(gb, PALETTE_BGP, gb->ppu.bg_window_pixel.color);
            gb->ppu.frame_buffer[gb->ppu.ly * 160 + gb->ppu.current_x] = 
                    (gb->ppu.lcdc & LCDC_BG_ENABLED) ? color : default_color[0];
        }
        if (gb->ppu.current_x == 160) {
            gb->ppu.current_x = 0;
            ppu_do_set_mode(gb, PPU_MODE_HBLANK);
            for (int i = 0; i < gb->ppu.oam_buffer_size; i++)
                gb->ppu.oam_buffer[i].rendered = false;
                gb->ppu.oam_buffer_size = 0;
            } else {
            gb->ppu.current_x++;
        }
    }
}

void ppu_do_drawing(gb_t *gb)
{
    ppu_do_fetch_pixels(gb);
    ppu_do_output_pixel(gb);
}

bool ppu_do_check_stat_interrupt(gb_t *gb)
{
    bool stat_interrupt_line = false, old_stat_interrupt_line = gb->ppu.stat_interrupt_line;

    if ((gb->ppu.stat & STAT_LYC) && (gb->ppu.ly == gb->ppu.lyc) && 
        !(gb->ppu.stat_interrupt_happen & STAT_LYC)) {
        stat_interrupt_line = true;
        gb->ppu.stat_interrupt_happen |= STAT_LYC;
    }
    if ((gb->ppu.stat & STAT_MODE2) && (gb->ppu.mode == PPU_MODE_OAM_SCAN) && 
        !(gb->ppu.stat_interrupt_happen & STAT_MODE2)) {
        stat_interrupt_line = true;
        gb->ppu.stat_interrupt_happen |= STAT_MODE2;
    }
    if ((gb->ppu.stat & STAT_MODE1) && (gb->ppu.mode == PPU_MODE_VBLANK) && 
        !(gb->ppu.stat_interrupt_happen & STAT_MODE1)) {
        stat_interrupt_line = true;
        gb->ppu.stat_interrupt_happen |= STAT_MODE1;
    }
    if ((gb->ppu.stat & STAT_MODE0) && (gb->ppu.mode == PPU_MODE_HBLANK) && 
        !(gb->ppu.stat_interrupt_happen & STAT_MODE0)) {
        stat_interrupt_line = true;
        gb->ppu.stat_interrupt_happen |= STAT_MODE0;
    }
    gb->ppu.stat_interrupt_line = stat_interrupt_line;
    return (!old_stat_interrupt_line && stat_interrupt_line) ? 1 : 0;
}

void ppu_tick(gb_t *gb)
{
    if (gb->ppu.lcdc & LCDC_LCD_POWER) {
        gb->ppu.tick++;
        if (ppu_do_check_stat_interrupt(gb))
            interrupt_request(gb, INTERRUPT_SOURCE_LCD);
        switch (gb->ppu.mode) {
        case PPU_MODE_OAM_SCAN:
            ppu_do_oam_scan(gb);
            break;
        case PPU_MODE_DRAWING:
            ppu_do_drawing(gb);
            break;
        case PPU_MODE_HBLANK:
            if (gb->ppu.tick == 456) {
                if (gb->ppu.ly == 143) {
                    ppu_do_set_mode(gb, PPU_MODE_VBLANK);
                    interrupt_request(gb, INTERRUPT_SOURCE_VBLANK);
                } else {
                    ppu_do_set_mode(gb, PPU_MODE_OAM_SCAN);
                }
                gb->ppu.ly++;
                gb->ppu.stat = (gb->ppu.ly == gb->ppu.lyc) ? gb->ppu.stat | STAT_LYC_EQUAL_LY : 
                                                            gb->ppu.stat & ~STAT_LYC_EQUAL_LY;
                gb->ppu.stat_interrupt_happen = 0;
                gb->ppu.tick = 0;
            }
            break;
        case PPU_MODE_VBLANK:
            if (gb->ppu.tick == 456) {
                if (gb->ppu.ly == 153) {
                    ppu_do_set_mode(gb, PPU_MODE_OAM_SCAN);
                    gb->ppu.need_refresh = true;
                    gb->ppu.bg_window_pf.reset_status = true;
                    gb->ppu.ly = 0;
                } else {
                    gb->ppu.ly++;
                }
                gb->ppu.stat = (gb->ppu.ly == gb->ppu.lyc) ? gb->ppu.stat | STAT_LYC_EQUAL_LY : 
                                                            gb->ppu.stat & ~STAT_LYC_EQUAL_LY;
                gb->ppu.stat_interrupt_happen = 0;
                gb->ppu.tick = 0;
            }
            break;
        default:
            break;
        }
    }
}

void ppu_write(gb_t *gb, uint16_t addr, uint8_t val)
{
    switch (addr) {
    case PPU_LCDC_REGISTER:
        gb->ppu.lcdc = val; 
        if (!(val & LCDC_LCD_POWER)) {
            ppu_do_set_mode(gb, PPU_MODE_HBLANK);
            gb->ppu.ly = 0;
        }
        break;
    case PPU_STAT_REGISTER:
        gb->ppu.stat = (val & 0xf8) | (gb->ppu.stat & 0x87); 
        break;
    case PPU_SCY_REGISTER:
        gb->ppu.scy = val; 
        break;
    case PPU_SCX_REGISTER:
        gb->ppu.scx = val; 
        break;
    case PPU_LY_REGISTER:
        printf("ERROR: %04x isn't writeable\n", addr);
        break;
    case PPU_LYC_REGISTER:
        gb->ppu.lyc = val; 
        break;
    case PPU_WX_REGISTER:
        gb->ppu.wx = val;
        break;
    case PPU_WY_REGISTER:
        gb->ppu.wy = val;
        break;
    case PPU_BGP_REGISTER:
        gb->ppu.bgp = val; 
        break;
    case PPU_OBP0_REGISTER:
        gb->ppu.obp0 = val; 
        break;
    case PPU_OBP1_REGISTER:
        gb->ppu.obp1 = val; 
        break;
    default:
        return;
    }
}

uint8_t ppu_read(gb_t *gb, uint16_t addr)
{
    uint8_t ret;

    switch (addr) {
    case PPU_LCDC_REGISTER:
        ret = gb->ppu.lcdc;
        break;
    case PPU_STAT_REGISTER:
        ret = gb->ppu.stat;
        break;
    case PPU_SCY_REGISTER:
        ret = gb->ppu.scy;
        break;
    case PPU_SCX_REGISTER:
        ret = gb->ppu.scx;
        break;
    case PPU_WX_REGISTER:
        ret = gb->ppu.wx;
        break;
    case PPU_WY_REGISTER:
        ret = gb->ppu.wy;
        break;
    case PPU_LY_REGISTER:
        ret = gb->ppu.ly;
        break;
    case PPU_LYC_REGISTER:
        ret = gb->ppu.lyc;
        break;
    case PPU_BGP_REGISTER:
        ret = gb->ppu.bgp;
        break;
    case PPU_OBP0_REGISTER:
        ret = gb->ppu.obp0;
        break;
    case PPU_OBP1_REGISTER:
        ret = gb->ppu.obp1;
        break;
    default:
        break;
    }
    return ret;
}

void ppu_init(gb_t *gb)
{
    gb->ppu.bg_window_fifo.size = 0;
    gb->ppu.bg_window_fifo.head = NULL;
    gb->ppu.bg_window_fifo.tail = NULL;

    gb->ppu.current_x = 0;
    gb->ppu.mode = PPU_MODE_OAM_SCAN;
    gb->ppu.need_refresh = false;
    gb->ppu.tick = 0;
    gb->ppu.ly = 0;

    gb->dma.tick = 0;
    gb->dma.mode = DMA_MODE_OFF;

    gb->ppu.oam_buffer_size = 0;

    gb->ppu.bg_window_pf.x = 0;
    gb->ppu.bg_window_pf.state = 0;
    gb->ppu.bg_window_pf.reset_status = true;
}

/* vram_write and vram_read only used by cpu, not ppu */

void vram_write(gb_t *gb, uint16_t addr, uint8_t val)
{
    if (gb->ppu.mode != PPU_MODE_DRAWING)
        gb->mem[addr] = val;
}

uint8_t vram_read(gb_t *gb, uint16_t addr)
{
    uint8_t ret;

    if (gb->ppu.mode == PPU_MODE_DRAWING)
        ret = 0xff;
    else 
        ret = gb->mem[addr];
    return ret;
}