#include "interrupt.h"

bool is_any_interrupt_pending(gb_t *gb)
{
    return (gb->intr.intr_enable & gb->intr.intr_flag) ? 1 : 0;
}

void interrupt_write(gb_t *gb, uint16_t addr, uint8_t val)
{
    switch (addr) {
    case INTERRUPT_IE_REGISTER:
        gb->intr.intr_enable = val;
        break;
    case INTERRUPT_IF_REGISTER:
        gb->intr.intr_flag = val;
        break;
    default:
        break;
    }
}

uint8_t interrupt_read(gb_t *gb, uint16_t addr)
{
    uint8_t ret;

    switch (addr) {
    case INTERRUPT_IE_REGISTER:
        ret = gb->intr.intr_enable;
        break;
    case INTERRUPT_IF_REGISTER:
        ret = gb->intr.intr_flag;
        break;
    default:
        ret = 0xff;
        break;
    }
    return ret;
}

static void isr(gb_t *gb, interrupt_source_t source, uint16_t handler_addr) 
{
    gb->intr.intr_flag &= ~source;
    gb->intr.ime = false;
    cpu_tick4(gb);
    cpu_tick4(gb);
    cpu_stack_push(gb, gb->cpu.regs.pc);
    cpu_tick4(gb);
    gb->cpu.regs.pc = handler_addr;
}

void interrupt_handler(gb_t *gb)
{
    if ((gb->intr.intr_enable & INTERRUPT_SOURCE_VBLANK) && (gb->intr.intr_flag & INTERRUPT_SOURCE_VBLANK))
        isr(gb, INTERRUPT_SOURCE_VBLANK, INTERRUPT_VBLANK_VECTOR);
    else if ((gb->intr.intr_enable & INTERRUPT_SOURCE_LCD) && (gb->intr.intr_flag & INTERRUPT_SOURCE_LCD))
        isr(gb, INTERRUPT_SOURCE_LCD, INTERRUPT_LCD_VECTOR);
    else if ((gb->intr.intr_enable & INTERRUPT_SOURCE_TIMER) && (gb->intr.intr_flag & INTERRUPT_SOURCE_TIMER))
        isr(gb, INTERRUPT_SOURCE_TIMER, INTERRUPT_TIMER_VECTOR);
    else if ((gb->intr.intr_enable & INTERRUPT_SOURCE_SERIAL) && (gb->intr.intr_flag & INTERRUPT_SOURCE_SERIAL))
        isr(gb, INTERRUPT_SOURCE_SERIAL, INTERRUPT_SERIAL_VECTOR);
    else if ((gb->intr.intr_enable & INTERRUPT_SOURCE_JOYPAD) && (gb->intr.intr_flag & INTERRUPT_SOURCE_JOYPAD)) {
        isr(gb, INTERRUPT_SOURCE_JOYPAD, INTERRUPT_JOYPAD_VECTOR);
    }
}

bool interrupt_check(gb_t *gb)
{
    return (is_any_interrupt_pending(gb) && gb->intr.ime == true) ? 1 : 0;
}

void interrupt_request(gb_t *gb, interrupt_source_t source)
{
    gb->intr.intr_flag |= source;
}