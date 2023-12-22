#include "joypad.h"

void joypad_press_button(gb_t *gb, uint8_t key)
{
    switch (key) {
    case JOYPAD_A:
        gb->joypad.a = 0;
        break;
    case JOYPAD_B:
        gb->joypad.b = 0;
        break;
    case JOYPAD_SELECT:
        gb->joypad.select = 0;
        break;
    case JOYPAD_START:
        gb->joypad.start = 0;
        break;
    case JOYPAD_RIGHT:    
        gb->joypad.right = 0;
        break;
    case JOYPAD_LEFT:
        gb->joypad.left = 0;
        break;
    case JOYPAD_UP:
        gb->joypad.up = 0;
        break;
    case JOYPAD_DOWN:
        gb->joypad.down = 0;
        break;
    default:
        break;
    }
    interrupt_request(gb, INTERRUPT_SOURCE_JOYPAD);
}

void joypad_release_button(gb_t *gb, uint8_t key)
{
    switch (key) {
    case JOYPAD_A:
        gb->joypad.a = 1;
        break;
    case JOYPAD_B:
        gb->joypad.b = 1;
        break;
    case JOYPAD_SELECT:
        gb->joypad.select = 1;
        break;
    case JOYPAD_START:
        gb->joypad.start = 1;
        break;
    case JOYPAD_RIGHT:    
        gb->joypad.right = 1;
        break;
    case JOYPAD_LEFT:
        gb->joypad.left = 1;
        break;
    case JOYPAD_UP:
        gb->joypad.up = 1;
        break;
    case JOYPAD_DOWN:
        gb->joypad.down = 1;
        break;
    default:
        break;
    }

}

void joypad_write(gb_t *gb, uint8_t val)
{
    gb->joypad.p1 = 0xcf | val;
}

uint8_t joypad_read(gb_t *gb)
{
    uint8_t ret = 0xff;
    uint8_t select_button = (gb->joypad.p1 >> 5) & 0x1;
    uint8_t select_dpad = (gb->joypad.p1 >> 4) & 0x1;

    if (select_button && select_dpad)
        ret = 0xff;
    else if (!select_button)    // select button
        ret = (gb->joypad.p1 & 0xf0) | (gb->joypad.a << 0) | (gb->joypad.b << 1) | (gb->joypad.select << 2) | (gb->joypad.start << 3);
    else if (!select_dpad)    // select d-pad
        ret = (gb->joypad.p1 & 0xf0) | (gb->joypad.right << 0) | (gb->joypad.left << 1) | (gb->joypad.up << 2) | (gb->joypad.down << 3);
    return ret;
}