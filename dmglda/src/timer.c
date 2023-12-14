#include "timer.h"

static uint16_t div_bit_pos[] = {
    [0x00] = (1U << 9),
    [0x01] = (1U << 3),
    [0x10] = (1U << 5),
    [0x11] = (1U << 7),
};

void timer_tick(gb_t *gb)
{
    uint16_t prev_div = gb->timer.div;
    bool output, prev_output, tima_increment = false;

    gb->timer.div++;
    prev_output = (prev_div & div_bit_pos[gb->timer.tac & 0x03]) && (gb->timer.tac & (1U << 2));
    output = (gb->timer.div & div_bit_pos[gb->timer.tac & 0x03]) && (gb->timer.tac & (1U << 2));
    if (prev_output && !output)     // expecting a failing edge
        tima_increment = true;
    if (tima_increment && (gb->timer.tac & (1U << 2))) {
        if (gb->timer.tima == 0xff) {
            gb->timer.ticks_after_tima_overflow = 0;
            gb->timer.tima_overflow = true;
            gb->timer.tima = 0x00;
        } else
            gb->timer.tima++;
    }
    if (gb->timer.tima_overflow && (gb->timer.ticks_after_tima_overflow == 4)) {
        gb->timer.tima = gb->timer.tma;
        interrupt_request(gb, INTERRUPT_SOURCE_TIMER);
        gb->timer.tima_overflow = false;
    } else if (gb->timer.tima_overflow && (gb->timer.ticks_after_tima_overflow < 4)) {
        gb->timer.ticks_after_tima_overflow++;
    }
}

void timer_write(gb_t *gb, uint16_t addr, uint8_t val)
{
    switch (addr) {
    case TIMER_DIV_REGISTER:
        gb->timer.div = 0;
        break;
    case TIMER_TAC_REGISTER:
        gb->timer.tac = val;
        break;
    case TIMER_TIMA_REGISTER:
        gb->timer.tima = val;
        break;
    case TIMER_TMA_REGISTER:
        gb->timer.tma = val;
        break;
    default:
        break;
    }
}

uint8_t timer_read(gb_t *gb, uint16_t addr)
{
    uint8_t ret;

    switch (addr) {
    case TIMER_DIV_REGISTER:
        ret = (uint8_t)(gb->timer.div >> 8);
        break;
    case TIMER_TAC_REGISTER:
        ret = gb->timer.tac & 0x07;
        break;
    case TIMER_TIMA_REGISTER:
        ret = gb->timer.tima;
        break;
    case TIMER_TMA_REGISTER:
        ret = gb->timer.tma;
        break; 
    default:
        break;
    }
    return ret;
}