#include "cpu.h"

void cpu_tick(gb_t *gb)
{
    timer_tick(gb);
    ppu_tick(gb);
    dma_tick(gb);
}

void cpu_tick4(gb_t *gb)
{
    cpu_tick(gb);
    cpu_tick(gb);
    cpu_tick(gb);
    cpu_tick(gb);
}

static uint8_t cpu_read8(gb_t *gb, uint16_t addr)
{
    cpu_tick4(gb);
    if (gb->dma.mode == DMA_MODE_TRANSFER)
        if ((addr >= VRAM_START_ADDR && addr <= VRAM_END_ADDR) ||
            (addr >= WRAM_START_ADDR && addr <= WRAM_END_ADDR) ||
            (addr <= 0x7fff))
            return gb->dma.current_transfer_byte;
    return mmu_read(gb, addr);
}

static void cpu_write8(gb_t *gb, uint16_t addr, uint8_t val)
{
    cpu_tick4(gb);
    if (gb->dma.mode == DMA_MODE_TRANSFER)
        if ((addr >= VRAM_START_ADDR && addr <= VRAM_END_ADDR) ||
            (addr >= WRAM_START_ADDR && addr <= WRAM_END_ADDR) ||
            (addr <= 0x7fff))
        return;
    mmu_write(gb, addr, val);
}

static uint8_t get_r8(gb_t *gb, cpu_r_t r)
{
    switch (r) {
    case CPU_R_A:
        return gb->cpu.regs.a;
    case CPU_R_B:
        return gb->cpu.regs.b;
    case CPU_R_C:
        return gb->cpu.regs.c;
    case CPU_R_D:
        return gb->cpu.regs.d;
    case CPU_R_E:
        return gb->cpu.regs.e;
    case CPU_R_H:
        return gb->cpu.regs.h;
    case CPU_R_L:
        return gb->cpu.regs.l;
    case CPU_R_F:
        return gb->cpu.regs.f;
    default:
        break;
    }

    return 0xff;
}

static void set_r8(gb_t *gb, cpu_r_t r, uint8_t val)
{
    switch (r) {
    case CPU_R_A:
        gb->cpu.regs.a = val;
        break;
    case CPU_R_B:
        gb->cpu.regs.b = val;
        break;
    case CPU_R_C:
        gb->cpu.regs.c = val;
        break;
    case CPU_R_D:
        gb->cpu.regs.d = val;
        break;
    case CPU_R_E:
        gb->cpu.regs.e = val;
        break;
    case CPU_R_H:
        gb->cpu.regs.h = val;
        break;
    case CPU_R_L:
        gb->cpu.regs.l = val;
        break;
    case CPU_R_F:
        gb->cpu.regs.f = val;
        break;
    default:
        break;
    }
}

static void set_r16(gb_t *gb, cpu_rr_t r, uint16_t val)
{
    switch (r) {
    case CPU_RR_AF:
        gb->cpu.regs.a = (val >> 8) & 0xff;
        gb->cpu.regs.f = val & 0xff;
        break;
    case CPU_RR_BC:
        gb->cpu.regs.b = (val >> 8) & 0xff;
        gb->cpu.regs.c = val & 0xff;
        break;
    case CPU_RR_DE:
        gb->cpu.regs.d = (val >> 8) & 0xff;
        gb->cpu.regs.e = val & 0xff;
        break;
    case CPU_RR_HL:
        gb->cpu.regs.h = (val >> 8) & 0xff;
        gb->cpu.regs.l = val & 0xff;
        break;
    case CPU_RR_SP:
        gb->cpu.regs.sp = val;
        break;
    case CPU_RR_PC:
        gb->cpu.regs.pc = val;
        break;
    default:
        break;
    }
}

static uint16_t get_r16(gb_t *gb, cpu_rr_t r)
{
    switch (r) {
    case CPU_RR_AF:
        return ((uint16_t)(gb->cpu.regs.a) << 8) | (uint16_t)(gb->cpu.regs.f);
    case CPU_RR_BC:
        return ((uint16_t)(gb->cpu.regs.b) << 8) | (uint16_t)(gb->cpu.regs.c);
    case CPU_RR_DE:
        return ((uint16_t)(gb->cpu.regs.d) << 8) | (uint16_t)(gb->cpu.regs.e);
    case CPU_RR_HL:
        return ((uint16_t)(gb->cpu.regs.h) << 8) | (uint16_t)(gb->cpu.regs.l);
    case CPU_RR_SP:
        return gb->cpu.regs.sp; 
    case CPU_RR_PC:
        return gb->cpu.regs.pc; 
    default:
        break;
    }
    
    return 0xff;
}

static void set_flag(gb_t *gb, cpu_flag_t flag)
{
    gb->cpu.regs.f |= flag;
}

static void res_flag(gb_t *gb, cpu_flag_t flag)
{
    gb->cpu.regs.f &= ~flag;
}

static void toggle_z_flag(gb_t *gb, bool val)
{
    if (!val)
        gb->cpu.regs.f |= CPU_FLAG_Z;
    else
        gb->cpu.regs.f &= ~CPU_FLAG_Z;
}

static void toggle_c_flag(gb_t *gb, bool c)
{
    if (c)
        gb->cpu.regs.f |= CPU_FLAG_C;
    else
        gb->cpu.regs.f &= ~CPU_FLAG_C;
}

static void toggle_h_flag(gb_t *gb, bool h)
{
    if (h)
        gb->cpu.regs.f |= CPU_FLAG_H;
    else
        gb->cpu.regs.f &= ~CPU_FLAG_H;
}

static int cpu_check_cond(gb_t *gb, cpu_flag_cond_t cond)
{
    uint8_t f;

    switch (cond) {
    case CPU_FLAG_COND_NZ:
    case CPU_FLAG_COND_Z:
        f = gb->cpu.regs.f & CPU_FLAG_Z;
        if (cond == CPU_FLAG_COND_NZ)
            if (!f) return 1; else return 0;
        else if (cond == CPU_FLAG_COND_Z)
            if (!f) return 0; else return 1;
    case CPU_FLAG_COND_NC:
    case CPU_FLAG_COND_C:
        f = gb->cpu.regs.f & CPU_FLAG_C;
        if (cond == CPU_FLAG_COND_NC)
            if (!f) return 1; else return 0;
        else if (cond == CPU_FLAG_COND_C)
            if (!f) return 0; else return 1;
    default:
        break;
    }
    return 0;
}

void cpu_stack_push(gb_t *gb, uint16_t val)
{
    gb->cpu.regs.sp--;
    cpu_write8(gb, gb->cpu.regs.sp, MSB(val));
    gb->cpu.regs.sp--;
    cpu_write8(gb, gb->cpu.regs.sp, LSB(val));
}

uint16_t cpu_stack_pop(gb_t *gb)
{
    uint8_t lsb = cpu_read8(gb, gb->cpu.regs.sp++);
    uint8_t msb = cpu_read8(gb, gb->cpu.regs.sp++);
    return U16(lsb, msb);
}

static uint8_t fetch8(gb_t *gb)
{
    gb->cpu.operand1 = cpu_read8(gb, gb->cpu.regs.pc++);
    return gb->cpu.operand1;
}

static uint16_t fetch16(gb_t *gb)
{
    uint8_t lsb, msb;
    lsb = gb->cpu.operand1 = cpu_read8(gb, gb->cpu.regs.pc++);
    msb = gb->cpu.operand2 = cpu_read8(gb, gb->cpu.regs.pc++);

    return U16(lsb, msb);
}

static void ld_rr_nn(gb_t *gb, cpu_rr_t rr)
{

    uint16_t nn = fetch16(gb);

    set_r16(gb, rr, nn);
}

static void ld_indirect_rr_a(gb_t *gb, cpu_rr_t rr)
{
    uint16_t rr_val = get_r16(gb, rr);

    cpu_write8(gb, rr_val, gb->cpu.regs.a);
}

static void inc_rr(gb_t *gb, cpu_rr_t rr)
{
    cpu_tick4(gb);
    set_r16(gb, rr, get_r16(gb, rr) + 1);    
}

static void inc_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r);

    toggle_z_flag(gb, (uint8_t)(r_val + 1));
    res_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((r_val & 0xf) + (1 & 0xf)) & 0x10);
    set_r8(gb, r, r_val + 1);
}

static void dec_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r);

    toggle_z_flag(gb, (uint8_t)(r_val - 1));
    set_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((r_val & 0xf) - (1 & 0xf)) & 0x10);
    set_r8(gb, r, r_val - 1);
}

static void ld_r_n(gb_t *gb, cpu_r_t r)
{
    uint8_t n = fetch8(gb);
    set_r8(gb, r, n);
}

static void rlca(gb_t *gb)
{
    uint8_t a = get_r8(gb, CPU_R_A), c = a & 0x80;

    res_flag(gb, CPU_FLAG_Z);
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c);
    set_r8(gb, CPU_R_A, (a << 1) | (c >> 7));
}

static void ld_indirect_nn_sp(gb_t *gb)
{
    uint16_t nn = fetch16(gb);

    cpu_write8(gb, nn, LSB(gb->cpu.regs.sp));
    cpu_write8(gb, nn + 1, MSB(gb->cpu.regs.sp));
}

static void add_hl_rr(gb_t *gb, cpu_rr_t rr)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint16_t rr_val = get_r16(gb, rr);

    cpu_tick4(gb);
    set_r16(gb, CPU_RR_HL, hl + get_r16(gb, rr));
    res_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((hl & 0xfff) + (rr_val & 0xfff)) & 0x1000);
    toggle_c_flag(gb, (uint32_t)(hl + rr_val) & 0x10000);
    set_r16(gb, CPU_RR_HL, hl + rr_val);
}

static void dec_rr(gb_t *gb, cpu_rr_t CPU_RR_BC)
{
    cpu_tick4(gb);
    set_r16(gb, CPU_RR_BC, get_r16(gb, CPU_RR_BC) - 1);
}

static void rrca(gb_t *gb)
{
    uint8_t a = get_r8(gb, CPU_R_A), c = a & 0x01;

    res_flag(gb, CPU_FLAG_Z);
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c);
    set_r8(gb, CPU_R_A, (a >> 1) | (c << 7));
}

static void stop(gb_t *gb)
{
    gb->cpu.mode = CPU_MODE_STOP;
}

static void rla(gb_t *gb)
{
    uint8_t a = get_r8(gb, CPU_R_A), c = a & 0x80;
    uint8_t oc = (gb->cpu.regs.f & CPU_FLAG_C) >> 4;

    res_flag(gb, CPU_FLAG_Z);
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c);
    set_r8(gb, CPU_R_A, (a << 1) | oc);
}

static void jr_i8(gb_t *gb)
{
    uint8_t n = fetch8(gb);

    cpu_tick4(gb);
    gb->cpu.regs.pc += (int8_t)n;
}

static void rra(gb_t *gb)
{
    uint8_t a = get_r8(gb, CPU_R_A), c = a & 0x01;
    uint8_t oc = (gb->cpu.regs.f & CPU_FLAG_C) >> 4;

    res_flag(gb, CPU_FLAG_Z);
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c);
    set_r8(gb, CPU_R_A, (a >> 1) | (oc << 7));
}

static void jr_f_i8(gb_t *gb, cpu_flag_cond_t cond)
{
    uint8_t n = fetch8(gb);

    if (cpu_check_cond(gb, cond)) {
        cpu_tick4(gb);
        gb->cpu.regs.pc += (int8_t)n;
    }
}

static void ld_a_indirect_rr(gb_t *gb, cpu_rr_t rr)
{
    uint16_t rr_val = get_r16(gb, rr);

    gb->cpu.regs.a = cpu_read8(gb, rr_val);
}

static void ldi_indirect_hl_a(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);

    cpu_write8(gb, hl, get_r8(gb, CPU_R_A));
    set_r16(gb, CPU_RR_HL, hl + 1);
}

static void ldi_a_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl);

    set_r8(gb, CPU_R_A, hl_val);
    set_r16(gb, CPU_RR_HL, hl + 1);
}

static void ldd_indirect_hl_a(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);

    cpu_write8(gb, hl, get_r8(gb, CPU_R_A));
    set_r16(gb, CPU_RR_HL, hl - 1);
}

static void ldd_a_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl);

    set_r8(gb, CPU_R_A, hl_val);
    set_r16(gb, CPU_RR_HL, hl - 1);
}

static void inc_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl);

    toggle_z_flag(gb, (uint8_t)(hl_val + 1));
    res_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((hl_val & 0xf) + (1 & 0xf)) & 0x10);
    cpu_write8(gb, hl, hl_val + 1);
}

static void cpl(gb_t *gb)
{
    set_flag(gb, CPU_FLAG_N);
    set_flag(gb, CPU_FLAG_H);
    set_r8(gb, CPU_R_A, get_r8(gb, CPU_R_A) ^ 0xff);
}

static void daa(gb_t *gb)
{
    uint8_t a = gb->cpu.regs.a;

    if (!(gb->cpu.regs.f & CPU_FLAG_N)) {  // after an addition, adjust if (half-)carry occurred or if result is out of bounds
        if ((gb->cpu.regs.f & CPU_FLAG_C) || a > 0x99) { a += 0x60; gb->cpu.regs.f |= CPU_FLAG_C; }
        if ((gb->cpu.regs.f & CPU_FLAG_H) || (a & 0x0f) > 0x09) { a += 0x6; }
    } else {  // after a subtraction, only adjust if (half-)carry occurred
        if (gb->cpu.regs.f & CPU_FLAG_C) { a -= 0x60; }
        if (gb->cpu.regs.f & CPU_FLAG_H) { a -= 0x6; }
    }
    // these flags are always updated
    gb->cpu.regs.f = (a == 0) ? gb->cpu.regs.f | CPU_FLAG_Z : gb->cpu.regs.f & ~CPU_FLAG_Z; // the usual z flag
    gb->cpu.regs.f &= ~CPU_FLAG_H; // h flag is always cleared
    set_r8(gb, CPU_R_A, a);
}

static void dec_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl);

    toggle_z_flag(gb, (uint8_t)(hl_val - 1));
    set_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((hl_val & 0xf) - (1 & 0xf)) & 0x10);
    cpu_write8(gb, hl, hl_val - 1);
}

static void ld_indirect_hl_n(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t n = fetch8(gb);

    cpu_write8(gb, hl, n);
}

static void scf(gb_t *gb)
{
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    set_flag(gb, CPU_FLAG_C);
}

static void ccf(gb_t *gb)
{
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    if (gb->cpu.regs.f & CPU_FLAG_C)
        res_flag(gb, CPU_FLAG_C);
    else 
        set_flag(gb, CPU_FLAG_C);
}

static void ld_r_indirect_hl(gb_t *gb, cpu_r_t r)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t val = cpu_read8(gb, hl);

    set_r8(gb, r, val);
}

static void ld_indirect_hl_r(gb_t *gb, cpu_r_t r)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);

    cpu_write8(gb, hl, get_r8(gb, r));
}

static void halt(gb_t *gb)
{
    if (gb->intr.ime == false && is_any_interrupt_pending(gb)) {
        gb->cpu.mode = CPU_MODE_HALT_BUG;
        return;
    }
    gb->cpu.mode = CPU_MODE_HALT;
}

static void add_a_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a + r_val));
    res_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.a & 0xf) + (r_val & 0xf)) & 0x10);
    toggle_c_flag(gb, (uint16_t)(gb->cpu.regs.a + r_val) & 0x100);
    gb->cpu.regs.a += r_val;
}

static void add_a_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL), hl_val = cpu_read8(gb, hl);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a + hl_val));
    res_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.a & 0xf) + (hl_val & 0xf)) & 0x10);
    toggle_c_flag(gb, (uint16_t)(gb->cpu.regs.a + hl_val) & 0x100);
    gb->cpu.regs.a += hl_val;
}

static void adc_a_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r);
    uint8_t c = (gb->cpu.regs.f & CPU_FLAG_C) >> 4;
    
    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a + r_val + c));
    res_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.a & 0xf) + (r_val & 0xf) + (c & 0xf)) & 0x10);
    toggle_c_flag(gb, (uint16_t)(gb->cpu.regs.a + r_val + c) & 0x100);
    gb->cpu.regs.a += r_val + c;
}

static void adc_a_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t c = (gb->cpu.regs.f & CPU_FLAG_C) >> 4, hl_val = cpu_read8(gb, hl);
    
    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a + hl_val + c));
    res_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.a & 0xf) + (hl_val & 0xf) + (c & 0xf)) & 0x10);
    toggle_c_flag(gb, (uint16_t)(gb->cpu.regs.a + hl_val + c) & 0x100);
    gb->cpu.regs.a += hl_val + c;
}

static void sub_a_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a - r_val));
    set_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.a & 0xf) - (r_val & 0xf)) & 0x10);
    toggle_c_flag(gb, (uint16_t)(gb->cpu.regs.a - r_val) & 0x100);
    gb->cpu.regs.a -= r_val;
}

static void sub_a_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a - hl_val));
    set_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.a & 0xf) - (hl_val & 0xf)) & 0x10);
    toggle_c_flag(gb, (uint16_t)(gb->cpu.regs.a - hl_val) & 0x100);
    gb->cpu.regs.a -= hl_val;
}

static void sbc_a_r(gb_t *gb, cpu_r_t r)
{
    uint8_t c = (gb->cpu.regs.f & CPU_FLAG_C) >> 4;
    uint8_t r_val = get_r8(gb, r);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a - r_val - c));
    set_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.a & 0xf) - (r_val & 0xf) - (c & 0xf)) & 0x10);
    toggle_c_flag(gb, (uint16_t)(gb->cpu.regs.a - r_val - c) & 0x100);
    gb->cpu.regs.a -= r_val + c;
}

static void sbc_a_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t c = (gb->cpu.regs.f & CPU_FLAG_C) >> 4, hl_val = cpu_read8(gb, hl);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a - hl_val - c));
    set_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.a & 0xf) - (hl_val & 0xf) - (c & 0xf)) & 0x10);
    toggle_c_flag(gb, (uint16_t)(gb->cpu.regs.a - hl_val - c) & 0x100);
    gb->cpu.regs.a -= hl_val + c;
}

static void and_a_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a & r_val));
    res_flag(gb, CPU_FLAG_N);
    set_flag(gb, CPU_FLAG_H);
    res_flag(gb, CPU_FLAG_C);
    gb->cpu.regs.a &= r_val;
}

static void and_a_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a & hl_val));
    res_flag(gb, CPU_FLAG_N);
    set_flag(gb, CPU_FLAG_H);
    res_flag(gb, CPU_FLAG_C);
    gb->cpu.regs.a &= hl_val;
}

static void xor_a_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a ^ r_val));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    res_flag(gb, CPU_FLAG_C);
    gb->cpu.regs.a ^= r_val;
}

static void xor_a_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a ^ hl_val));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    res_flag(gb, CPU_FLAG_C);
    gb->cpu.regs.a ^= hl_val;
}


static void or_a_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a | r_val));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    res_flag(gb, CPU_FLAG_C);
    gb->cpu.regs.a |= r_val;
}

static void or_a_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a | hl_val));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    res_flag(gb, CPU_FLAG_C);
    gb->cpu.regs.a |= hl_val;
}

static void cp_a_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a - r_val));
    set_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.a & 0x0f) - (r_val & 0x0f)) & 0x10);
    toggle_c_flag(gb, (uint16_t)(gb->cpu.regs.a - r_val) & 0x100);
}

static void cp_a_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a - hl_val));
    set_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.a & 0x0f) - (hl_val & 0x0f)) & 0x10);
    toggle_c_flag(gb, (uint16_t)(gb->cpu.regs.a - hl_val) & 0x100);
}

static void ret_f(gb_t *gb, cpu_flag_cond_t cond)
{
    cpu_tick4(gb);
    if (cpu_check_cond(gb, cond)) {
        cpu_tick4(gb);
        gb->cpu.regs.pc = cpu_stack_pop(gb);
    }
}

static void pop_rr(gb_t *gb, cpu_rr_t rr)
{
    uint16_t val = cpu_stack_pop(gb);
    
    if (rr == CPU_RR_AF)
        val &= 0xfff0;
    set_r16(gb, rr, val);
}

static void jp_f_nn(gb_t *gb, cpu_flag_cond_t cond)
{
    uint16_t nn = fetch16(gb);

    if (cpu_check_cond(gb, cond)) {
        cpu_tick4(gb);
        gb->cpu.regs.pc = nn;
    }
}

static void jp_nn(gb_t *gb)
{
    uint16_t nn = fetch16(gb);

    cpu_tick4(gb);
    gb->cpu.regs.pc = nn;
}

static void call_f_nn(gb_t *gb, cpu_flag_cond_t cond)
{
    uint16_t nn = fetch16(gb);

    if (cpu_check_cond(gb, cond)) {
        cpu_tick4(gb);
        cpu_stack_push(gb, gb->cpu.regs.pc);
        gb->cpu.regs.pc = nn;
    }
}

static void push_rr(gb_t *gb, cpu_rr_t rr)
{
    cpu_tick4(gb);
    cpu_stack_push(gb, get_r16(gb, rr));
}

static void add_a_n(gb_t *gb)
{
    uint8_t n = fetch8(gb);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a + n));
    res_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.a & 0xf) + (n & 0xf)) & 0x10);
    toggle_c_flag(gb, (uint16_t)(gb->cpu.regs.a + n) & 0x100);
    gb->cpu.regs.a += n;
}

static void rst_n(gb_t *gb, uint16_t n)
{
    cpu_tick4(gb);
    cpu_stack_push(gb, gb->cpu.regs.pc);
    gb->cpu.regs.pc = n;
}

static void ret(gb_t *gb)
{
    uint16_t nn = cpu_stack_pop(gb);
    
    cpu_tick4(gb);
    gb->cpu.regs.pc = nn;
}

static void rlc_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r), c = r_val & 0x80;

    toggle_z_flag(gb, (uint8_t)((r_val << 1) | (c >> 7)));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c);
    set_r8(gb, r, (r_val << 1) | (c >> 7));
}

static void rlc_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl), c = hl_val & 0x80;

    toggle_z_flag(gb, (uint8_t)((hl_val << 1) | (c >> 7)));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c);
    cpu_write8(gb, hl, (hl_val << 1) | (c >> 7));
}

static void rrc_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r), c = r_val & 0x01;

    toggle_z_flag(gb, (uint8_t)((r_val >> 1) | (c << 7)));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c);
    set_r8(gb, r, (r_val >> 1) | (c << 7));
}

static void rrc_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl), c = hl_val & 0x01;

    toggle_z_flag(gb, (uint8_t)((hl_val >> 1) | (c << 7)));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c);
    cpu_write8(gb, hl, (hl_val >> 1) | (c << 7));
}

static void rl_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r), oc = (gb->cpu.regs.f & CPU_FLAG_C) >> 4;
    uint8_t c = r_val & 0x80;

    toggle_z_flag(gb, (uint8_t)((r_val << 1) | (oc)));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c); 
    set_r8(gb, r, (r_val << 1) | oc);
}

static void rl_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t oc = (gb->cpu.regs.f & CPU_FLAG_C) >> 4;
    uint8_t hl_val = cpu_read8(gb, hl);
    uint8_t c = hl_val & 0x80;

    toggle_z_flag(gb, (uint8_t)((hl_val << 1) | (oc)));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c); 
    cpu_write8(gb, hl, (hl_val << 1) | oc);
}

static void rr_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r), oc = (gb->cpu.regs.f & CPU_FLAG_C) >> 4;
    uint8_t c = r_val & 0x01;

    toggle_z_flag(gb, (uint8_t)((r_val >> 1) | (oc << 7)));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c); 
    set_r8(gb, r, (r_val >> 1) | (oc << 7));
}

static void rr_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t oc = (gb->cpu.regs.f & CPU_FLAG_C) >> 4, hl_val = cpu_read8(gb, hl);
    uint8_t c = hl_val & 0x01;

    toggle_z_flag(gb, (uint8_t)((hl_val >> 1) | (oc << 7)));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c); 
    cpu_write8(gb, hl, (hl_val >> 1) | (oc << 7));
}

static void sla_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r), c = r_val & 0x80;

    toggle_z_flag(gb, (uint8_t)(r_val << 1));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c);
    set_r8(gb, r, r_val << 1);
}

static void sla_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl), c = hl_val & 0x80;

    toggle_z_flag(gb, (uint8_t)(hl_val << 1));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c);
    cpu_write8(gb, hl, hl_val << 1);
}

static void sra_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r), c = r_val & 0x01;

    toggle_z_flag(gb, (uint8_t)((r_val >> 1) | (r_val & 0x80)));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c);
    set_r8(gb, r, (r_val >> 1) | (r_val & 0x80));
}

static void sra_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl), c= hl_val & 0x01;

    toggle_z_flag(gb, (uint8_t)((hl_val >> 1) | (hl_val & 0x80)));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c);
    cpu_write8(gb, hl, (hl_val >> 1) | (hl_val & 0x80));
}

static void swap_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r);

    toggle_z_flag(gb, (uint8_t)((r_val << 4) | (r_val >> 4)));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    res_flag(gb, CPU_FLAG_C);
    set_r8(gb, r, (r_val << 4) | (r_val >> 4));
}

static void swap_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl);

    toggle_z_flag(gb, (uint8_t)((hl_val << 4) | (hl_val >> 4)));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    res_flag(gb, CPU_FLAG_C);
    cpu_write8(gb, hl, (hl_val << 4) | (hl_val >> 4));
}

static void srl_r(gb_t *gb, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r), c = r_val & 0x01;

    toggle_z_flag(gb, (uint8_t)(r_val >> 1));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c);
    set_r8(gb, r, r_val >> 1);
}

static void srl_indirect_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl), c= hl_val & 0x01;

    toggle_z_flag(gb, (uint8_t)(hl_val >> 1));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    toggle_c_flag(gb, c);
    cpu_write8(gb, hl, hl_val >> 1);
}

static void bit_n_r(gb_t *gb, uint8_t n, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r);

    toggle_z_flag(gb, (uint8_t)(r_val & (1U << n)));
    res_flag(gb, CPU_FLAG_N);
    set_flag(gb, CPU_FLAG_H);
}

static void bit_n_indirect_hl(gb_t *gb, uint8_t n)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl);

    toggle_z_flag(gb, (uint8_t)(hl_val & (1U << n)));
    res_flag(gb, CPU_FLAG_N);
    set_flag(gb, CPU_FLAG_H);
}

static void res_n_r(gb_t *gb, uint8_t n, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r);

    set_r8(gb, r, r_val & ~(1U << n));
}

static void res_n_indirect_hl(gb_t *gb, uint8_t n)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl);

    cpu_write8(gb, hl, hl_val & ~(1U << n));
}

static void set_n_r(gb_t *gb, uint8_t n, cpu_r_t r)
{
    uint8_t r_val = get_r8(gb, r);

    set_r8(gb, r, r_val | (1U << n));
}

static void set_n_indirect_hl(gb_t *gb, uint8_t n)
{

    uint16_t hl = get_r16(gb, CPU_RR_HL);
    uint8_t hl_val = cpu_read8(gb, hl);

    cpu_write8(gb, hl, hl_val | (1U << n));
}

static void call_nn(gb_t *gb)
{
    uint16_t nn = fetch16(gb);

    cpu_tick4(gb);
    cpu_stack_push(gb, gb->cpu.regs.pc);
    gb->cpu.regs.pc = nn;
}

static void adc_a_n(gb_t *gb)
{
    uint8_t n = fetch8(gb);
    uint8_t c = (gb->cpu.regs.f & CPU_FLAG_C) >> 4;
    
    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a + n + c));
    res_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.a & 0xf) + (n & 0xf) + (c & 0xf)) & 0x10);
    toggle_c_flag(gb, (uint16_t)(gb->cpu.regs.a + n + c) & 0x100);
    gb->cpu.regs.a += n + c;
}

static void sub_a_n(gb_t *gb)
{
    uint8_t n = fetch8(gb);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a - n));
    set_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.a & 0xf) - (n & 0xf)) & 0x10);
    toggle_c_flag(gb, (uint16_t)(gb->cpu.regs.a - n) & 0x100);
    gb->cpu.regs.a -= n;
}

static void and_a_n(gb_t *gb)
{
    uint8_t n = fetch8(gb);;

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a & n));
    res_flag(gb, CPU_FLAG_N);
    set_flag(gb, CPU_FLAG_H);
    res_flag(gb, CPU_FLAG_C);
    gb->cpu.regs.a &= n;
}

static void or_a_n(gb_t *gb)
{
    uint8_t n = fetch8(gb);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a | n));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    res_flag(gb, CPU_FLAG_C);
    gb->cpu.regs.a |= n;
}

static void sbc_a_n(gb_t *gb)
{
    uint8_t c = (gb->cpu.regs.f & CPU_FLAG_C) >> 4, n = fetch8(gb);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a - n - c));
    set_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.a & 0xf) - (n & 0xf) - (c & 0xf)) & 0x10);
    toggle_c_flag(gb, (uint16_t)(gb->cpu.regs.a - n - c) & 0x100);
    gb->cpu.regs.a -= n + c;
}

static void xor_a_n(gb_t *gb)
{
    uint8_t n = fetch8(gb);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a ^ n));
    res_flag(gb, CPU_FLAG_N);
    res_flag(gb, CPU_FLAG_H);
    res_flag(gb, CPU_FLAG_C);
    gb->cpu.regs.a ^= n;
}

static void cp_a_n(gb_t *gb)
{
    uint8_t n = fetch8(gb);

    toggle_z_flag(gb, (uint8_t)(gb->cpu.regs.a - n));
    set_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.a & 0x0f) - (n & 0x0f)) & 0x10);
    toggle_c_flag(gb, (uint16_t)(gb->cpu.regs.a - n) & 0x100);
}

static void reti(gb_t *gb)
{
    uint16_t pc = cpu_stack_pop(gb);

    cpu_tick4(gb);
    gb->cpu.regs.pc = pc;
    gb->intr.ime = true;
}

static void ldh_indirect_n_a(gb_t *gb)
{
    uint8_t n = fetch8(gb);

    cpu_write8(gb, 0xff00 + n, gb->cpu.regs.a);
}

static void ldh_indirect_c_a(gb_t *gb)
{
    cpu_write8(gb, 0xff00 + gb->cpu.regs.c, gb->cpu.regs.a);
}

static void ldh_a_indirect_n(gb_t *gb)
{
    uint8_t n = fetch8(gb);

    gb->cpu.regs.a = cpu_read8(gb, 0xff00 + n);
}

static void ldh_a_indirect_c(gb_t *gb)
{
    gb->cpu.regs.a = cpu_read8(gb, 0xff00 + gb->cpu.regs.c);
}

static void add_sp_i8(gb_t *gb)
{
    uint8_t i = fetch8(gb);

    cpu_tick4(gb);
    cpu_tick4(gb);
    res_flag(gb, CPU_FLAG_Z);
    res_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.sp & 0x0f) + (i & 0x0f)) & 0x10);
    toggle_c_flag(gb, ((gb->cpu.regs.sp & 0xff) + (i & 0xff)) & 0x100);
    gb->cpu.regs.sp += (int8_t)i;
}

static void ld_hl_sp_i8(gb_t *gb)
{
    uint8_t i = fetch8(gb);

    cpu_tick4(gb);
    res_flag(gb, CPU_FLAG_Z);
    res_flag(gb, CPU_FLAG_N);
    toggle_h_flag(gb, ((gb->cpu.regs.sp & 0x0f) + (i & 0x0f)) & 0x10);
    toggle_c_flag(gb, ((gb->cpu.regs.sp & 0xff) + (i & 0xff)) & 0x100);
    set_r16(gb, CPU_RR_HL, gb->cpu.regs.sp + (int8_t)i);
}

static void jp_hl(gb_t *gb)
{
    gb->cpu.regs.pc = get_r16(gb, CPU_RR_HL);
}

static void ld_indirect_nn_a(gb_t *gb)
{
    uint16_t nn = fetch16(gb);

    cpu_write8(gb, nn, gb->cpu.regs.a);
}

static void ld_a_indirect_nn(gb_t *gb)
{
    uint16_t nn = fetch16(gb);

    gb->cpu.regs.a = cpu_read8(gb, nn);
}

static void di(gb_t *gb)
{
    gb->intr.ime = false;
}

static void ei(gb_t *gb)
{
    gb->cpu.mode = CPU_MODE_SET_IME;
}

static void ld_sp_hl(gb_t *gb)
{
    uint16_t hl = get_r16(gb, CPU_RR_HL);

    cpu_tick4(gb);
    gb->cpu.regs.sp = hl;
}

static void cpu_execute_cb_instruction(gb_t *gb)
{
    uint8_t opcode = gb->cpu.operand1 = fetch8(gb);

    switch (opcode) {
    case 0x00: rlc_r(gb, CPU_R_B);          break;
    case 0x01: rlc_r(gb, CPU_R_C);          break;
    case 0x02: rlc_r(gb, CPU_R_D);          break;
    case 0x03: rlc_r(gb, CPU_R_E);          break;
    case 0x04: rlc_r(gb, CPU_R_H);          break;
    case 0x05: rlc_r(gb, CPU_R_L);          break;
    case 0x06: rlc_indirect_hl(gb);         break;
    case 0x07: rlc_r(gb, CPU_R_A);          break;
    case 0x08: rrc_r(gb, CPU_R_B);          break;
    case 0x09: rrc_r(gb, CPU_R_C);          break;
    case 0x0a: rrc_r(gb, CPU_R_D);          break;
    case 0x0b: rrc_r(gb, CPU_R_E);          break;
    case 0x0c: rrc_r(gb, CPU_R_H);          break;
    case 0x0d: rrc_r(gb, CPU_R_L);          break;
    case 0x0e: rrc_indirect_hl(gb);         break;
    case 0x0f: rrc_r(gb, CPU_R_A);          break;
    case 0x10: rl_r(gb, CPU_R_B);           break;
    case 0x11: rl_r(gb, CPU_R_C);           break;
    case 0x12: rl_r(gb, CPU_R_D);           break;
    case 0x13: rl_r(gb, CPU_R_E);           break;
    case 0x14: rl_r(gb, CPU_R_H);           break;
    case 0x15: rl_r(gb, CPU_R_L);           break;
    case 0x16: rl_indirect_hl(gb);          break;
    case 0x17: rl_r(gb, CPU_R_A);           break;
    case 0x18: rr_r(gb, CPU_R_B);           break;
    case 0x19: rr_r(gb, CPU_R_C);           break;
    case 0x1a: rr_r(gb, CPU_R_D);           break;
    case 0x1b: rr_r(gb, CPU_R_E);           break;
    case 0x1c: rr_r(gb, CPU_R_H);           break;
    case 0x1d: rr_r(gb, CPU_R_L);           break;
    case 0x1e: rr_indirect_hl(gb);          break;
    case 0x1f: rr_r(gb, CPU_R_A);           break;
    case 0x20: sla_r(gb, CPU_R_B);          break;
    case 0x21: sla_r(gb, CPU_R_C);          break;
    case 0x22: sla_r(gb, CPU_R_D);          break;
    case 0x23: sla_r(gb, CPU_R_E);          break;
    case 0x24: sla_r(gb, CPU_R_H);          break;
    case 0x25: sla_r(gb, CPU_R_L);          break;
    case 0x26: sla_indirect_hl(gb);         break;
    case 0x27: sla_r(gb, CPU_R_A);          break;
    case 0x28: sra_r(gb, CPU_R_B);          break;
    case 0x29: sra_r(gb, CPU_R_C);          break;
    case 0x2a: sra_r(gb, CPU_R_D);          break;
    case 0x2b: sra_r(gb, CPU_R_E);          break;
    case 0x2c: sra_r(gb, CPU_R_H);          break;
    case 0x2d: sra_r(gb, CPU_R_L);          break;
    case 0x2e: sra_indirect_hl(gb);         break;
    case 0x2f: sra_r(gb, CPU_R_A);          break;
    case 0x30: swap_r(gb, CPU_R_B);         break;
    case 0x31: swap_r(gb, CPU_R_C);         break;
    case 0x32: swap_r(gb, CPU_R_D);         break;
    case 0x33: swap_r(gb, CPU_R_E);         break;
    case 0x34: swap_r(gb, CPU_R_H);         break;
    case 0x35: swap_r(gb, CPU_R_L);         break;
    case 0x36: swap_indirect_hl(gb);        break;
    case 0x37: swap_r(gb, CPU_R_A);         break;
    case 0x38: srl_r(gb, CPU_R_B);          break;
    case 0x39: srl_r(gb, CPU_R_C);          break;
    case 0x3a: srl_r(gb, CPU_R_D);          break;
    case 0x3b: srl_r(gb, CPU_R_E);          break;
    case 0x3c: srl_r(gb, CPU_R_H);          break;
    case 0x3d: srl_r(gb, CPU_R_L);          break;
    case 0x3e: srl_indirect_hl(gb);         break;
    case 0x3f: srl_r(gb, CPU_R_A);          break;
    case 0x40: bit_n_r(gb, 0, CPU_R_B);     break;
    case 0x41: bit_n_r(gb, 0, CPU_R_C);     break;
    case 0x42: bit_n_r(gb, 0, CPU_R_D);     break;
    case 0x43: bit_n_r(gb, 0, CPU_R_E);     break;
    case 0x44: bit_n_r(gb, 0, CPU_R_H);     break;
    case 0x45: bit_n_r(gb, 0, CPU_R_L);     break;
    case 0x46: bit_n_indirect_hl(gb, 0);    break;
    case 0x47: bit_n_r(gb, 0, CPU_R_A);     break;
    case 0x48: bit_n_r(gb, 1, CPU_R_B);     break;
    case 0x49: bit_n_r(gb, 1, CPU_R_C);     break;
    case 0x4a: bit_n_r(gb, 1, CPU_R_D);     break;
    case 0x4b: bit_n_r(gb, 1, CPU_R_E);     break;
    case 0x4c: bit_n_r(gb, 1, CPU_R_H);     break;
    case 0x4d: bit_n_r(gb, 1, CPU_R_L);     break;
    case 0x4e: bit_n_indirect_hl(gb, 1);    break;
    case 0x4f: bit_n_r(gb, 1, CPU_R_A);     break;
    case 0x50: bit_n_r(gb, 2, CPU_R_B);     break;
    case 0x51: bit_n_r(gb, 2, CPU_R_C);     break;
    case 0x52: bit_n_r(gb, 2, CPU_R_D);     break;
    case 0x53: bit_n_r(gb, 2, CPU_R_E);     break;
    case 0x54: bit_n_r(gb, 2, CPU_R_H);     break;
    case 0x55: bit_n_r(gb, 2, CPU_R_L);     break;
    case 0x56: bit_n_indirect_hl(gb, 2);    break;
    case 0x57: bit_n_r(gb, 2, CPU_R_A);     break;
    case 0x58: bit_n_r(gb, 3, CPU_R_B);     break;
    case 0x59: bit_n_r(gb, 3, CPU_R_C);     break;
    case 0x5a: bit_n_r(gb, 3, CPU_R_D);     break;
    case 0x5b: bit_n_r(gb, 3, CPU_R_E);     break;
    case 0x5c: bit_n_r(gb, 3, CPU_R_H);     break;
    case 0x5d: bit_n_r(gb, 3, CPU_R_L);     break;
    case 0x5e: bit_n_indirect_hl(gb, 3);    break;
    case 0x5f: bit_n_r(gb, 3, CPU_R_A);     break;
    case 0x60: bit_n_r(gb, 4, CPU_R_B);     break;
    case 0x61: bit_n_r(gb, 4, CPU_R_C);     break;
    case 0x62: bit_n_r(gb, 4, CPU_R_D);     break;
    case 0x63: bit_n_r(gb, 4, CPU_R_E);     break;
    case 0x64: bit_n_r(gb, 4, CPU_R_H);     break;
    case 0x65: bit_n_r(gb, 4, CPU_R_L);     break;
    case 0x66: bit_n_indirect_hl(gb, 4);    break;
    case 0x67: bit_n_r(gb, 4, CPU_R_A);     break;
    case 0x68: bit_n_r(gb, 5, CPU_R_B);     break;
    case 0x69: bit_n_r(gb, 5, CPU_R_C);     break;
    case 0x6a: bit_n_r(gb, 5, CPU_R_D);     break;
    case 0x6b: bit_n_r(gb, 5, CPU_R_E);     break;
    case 0x6c: bit_n_r(gb, 5, CPU_R_H);     break;
    case 0x6d: bit_n_r(gb, 5, CPU_R_L);     break;
    case 0x6e: bit_n_indirect_hl(gb, 5);    break;
    case 0x6f: bit_n_r(gb, 5, CPU_R_A);     break;
    case 0x70: bit_n_r(gb, 6, CPU_R_B);     break;
    case 0x71: bit_n_r(gb, 6, CPU_R_C);     break;
    case 0x72: bit_n_r(gb, 6, CPU_R_D);     break;
    case 0x73: bit_n_r(gb, 6, CPU_R_E);     break;
    case 0x74: bit_n_r(gb, 6, CPU_R_H);     break;
    case 0x75: bit_n_r(gb, 6, CPU_R_L);     break;
    case 0x76: bit_n_indirect_hl(gb, 6);    break;
    case 0x77: bit_n_r(gb, 6, CPU_R_A);     break;
    case 0x78: bit_n_r(gb, 7, CPU_R_B);     break;
    case 0x79: bit_n_r(gb, 7, CPU_R_C);     break;
    case 0x7a: bit_n_r(gb, 7, CPU_R_D);     break;
    case 0x7b: bit_n_r(gb, 7, CPU_R_E);     break;
    case 0x7c: bit_n_r(gb, 7, CPU_R_H);     break;
    case 0x7d: bit_n_r(gb, 7, CPU_R_L);     break;
    case 0x7e: bit_n_indirect_hl(gb, 7);    break;
    case 0x7f: bit_n_r(gb, 7, CPU_R_A);     break;
    case 0x80: res_n_r(gb, 0, CPU_R_B);     break;
    case 0x81: res_n_r(gb, 0, CPU_R_C);     break;
    case 0x82: res_n_r(gb, 0, CPU_R_D);     break;
    case 0x83: res_n_r(gb, 0, CPU_R_E);     break;
    case 0x84: res_n_r(gb, 0, CPU_R_H);     break;
    case 0x85: res_n_r(gb, 0, CPU_R_L);     break;
    case 0x86: res_n_indirect_hl(gb, 0);    break;
    case 0x87: res_n_r(gb, 0, CPU_R_A);     break;
    case 0x88: res_n_r(gb, 1, CPU_R_B);     break;
    case 0x89: res_n_r(gb, 1, CPU_R_C);     break;
    case 0x8a: res_n_r(gb, 1, CPU_R_D);     break;
    case 0x8b: res_n_r(gb, 1, CPU_R_E);     break;
    case 0x8c: res_n_r(gb, 1, CPU_R_H);     break;
    case 0x8d: res_n_r(gb, 1, CPU_R_L);     break;
    case 0x8e: res_n_indirect_hl(gb, 1);    break;
    case 0x8f: res_n_r(gb, 1, CPU_R_A);     break;
    case 0x90: res_n_r(gb, 2, CPU_R_B);     break;
    case 0x91: res_n_r(gb, 2, CPU_R_C);     break;
    case 0x92: res_n_r(gb, 2, CPU_R_D);     break;
    case 0x93: res_n_r(gb, 2, CPU_R_E);     break;
    case 0x94: res_n_r(gb, 2, CPU_R_H);     break;
    case 0x95: res_n_r(gb, 2, CPU_R_L);     break;
    case 0x96: res_n_indirect_hl(gb, 2);    break;
    case 0x97: res_n_r(gb, 2, CPU_R_A);     break;
    case 0x98: res_n_r(gb, 3, CPU_R_B);     break;
    case 0x99: res_n_r(gb, 3, CPU_R_C);     break;
    case 0x9a: res_n_r(gb, 3, CPU_R_D);     break;
    case 0x9b: res_n_r(gb, 3, CPU_R_E);     break;
    case 0x9c: res_n_r(gb, 3, CPU_R_H);     break;
    case 0x9d: res_n_r(gb, 3, CPU_R_L);     break;
    case 0x9e: res_n_indirect_hl(gb, 3);    break;
    case 0x9f: res_n_r(gb, 3, CPU_R_A);     break;
    case 0xa0: res_n_r(gb, 4, CPU_R_B);     break;
    case 0xa1: res_n_r(gb, 4, CPU_R_C);     break;
    case 0xa2: res_n_r(gb, 4, CPU_R_D);     break;
    case 0xa3: res_n_r(gb, 4, CPU_R_E);     break;
    case 0xa4: res_n_r(gb, 4, CPU_R_H);     break;
    case 0xa5: res_n_r(gb, 4, CPU_R_L);     break;
    case 0xa6: res_n_indirect_hl(gb, 4);    break;
    case 0xa7: res_n_r(gb, 4, CPU_R_A);     break;
    case 0xa8: res_n_r(gb, 5, CPU_R_B);     break;
    case 0xa9: res_n_r(gb, 5, CPU_R_C);     break;
    case 0xaa: res_n_r(gb, 5, CPU_R_D);     break;
    case 0xab: res_n_r(gb, 5, CPU_R_E);     break;
    case 0xac: res_n_r(gb, 5, CPU_R_H);     break;
    case 0xad: res_n_r(gb, 5, CPU_R_L);     break;
    case 0xae: res_n_indirect_hl(gb, 5);    break;
    case 0xaf: res_n_r(gb, 5, CPU_R_A);     break;
    case 0xb0: res_n_r(gb, 6, CPU_R_B);     break;
    case 0xb1: res_n_r(gb, 6, CPU_R_C);     break;
    case 0xb2: res_n_r(gb, 6, CPU_R_D);     break;
    case 0xb3: res_n_r(gb, 6, CPU_R_E);     break;
    case 0xb4: res_n_r(gb, 6, CPU_R_H);     break;
    case 0xb5: res_n_r(gb, 6, CPU_R_L);     break;
    case 0xb6: res_n_indirect_hl(gb, 6);    break;
    case 0xb7: res_n_r(gb, 6, CPU_R_A);     break;
    case 0xb8: res_n_r(gb, 7, CPU_R_B);     break;
    case 0xb9: res_n_r(gb, 7, CPU_R_C);     break;
    case 0xba: res_n_r(gb, 7, CPU_R_D);     break;
    case 0xbb: res_n_r(gb, 7, CPU_R_E);     break;
    case 0xbc: res_n_r(gb, 7, CPU_R_H);     break;
    case 0xbd: res_n_r(gb, 7, CPU_R_L);     break;
    case 0xbe: res_n_indirect_hl(gb, 7);    break;
    case 0xbf: res_n_r(gb, 7, CPU_R_A);     break;
    case 0xc0: set_n_r(gb, 0, CPU_R_B);     break;
    case 0xc1: set_n_r(gb, 0, CPU_R_C);     break;
    case 0xc2: set_n_r(gb, 0, CPU_R_D);     break;
    case 0xc3: set_n_r(gb, 0, CPU_R_E);     break;
    case 0xc4: set_n_r(gb, 0, CPU_R_H);     break;
    case 0xc5: set_n_r(gb, 0, CPU_R_L);     break;
    case 0xc6: set_n_indirect_hl(gb, 0);    break;
    case 0xc7: set_n_r(gb, 0, CPU_R_A);     break;
    case 0xc8: set_n_r(gb, 1, CPU_R_B);     break;
    case 0xc9: set_n_r(gb, 1, CPU_R_C);     break;
    case 0xca: set_n_r(gb, 1, CPU_R_D);     break;
    case 0xcb: set_n_r(gb, 1, CPU_R_E);     break;
    case 0xcc: set_n_r(gb, 1, CPU_R_H);     break;
    case 0xcd: set_n_r(gb, 1, CPU_R_L);     break;
    case 0xce: set_n_indirect_hl(gb, 1);    break;
    case 0xcf: set_n_r(gb, 1, CPU_R_A);     break;
    case 0xd0: set_n_r(gb, 2, CPU_R_B);     break;
    case 0xd1: set_n_r(gb, 2, CPU_R_C);     break;
    case 0xd2: set_n_r(gb, 2, CPU_R_D);     break;
    case 0xd3: set_n_r(gb, 2, CPU_R_E);     break;
    case 0xd4: set_n_r(gb, 2, CPU_R_H);     break;
    case 0xd5: set_n_r(gb, 2, CPU_R_L);     break;
    case 0xd6: set_n_indirect_hl(gb, 2);    break;
    case 0xd7: set_n_r(gb, 2, CPU_R_A);     break;
    case 0xd8: set_n_r(gb, 3, CPU_R_B);     break;
    case 0xd9: set_n_r(gb, 3, CPU_R_C);     break;
    case 0xda: set_n_r(gb, 3, CPU_R_D);     break;
    case 0xdb: set_n_r(gb, 3, CPU_R_E);     break;
    case 0xdc: set_n_r(gb, 3, CPU_R_H);     break;
    case 0xdd: set_n_r(gb, 3, CPU_R_L);     break;
    case 0xde: set_n_indirect_hl(gb, 3);    break;
    case 0xdf: set_n_r(gb, 3, CPU_R_A);     break;
    case 0xe0: set_n_r(gb, 4, CPU_R_B);     break;
    case 0xe1: set_n_r(gb, 4, CPU_R_C);     break;
    case 0xe2: set_n_r(gb, 4, CPU_R_D);     break;
    case 0xe3: set_n_r(gb, 4, CPU_R_E);     break;
    case 0xe4: set_n_r(gb, 4, CPU_R_H);     break;
    case 0xe5: set_n_r(gb, 4, CPU_R_L);     break;
    case 0xe6: set_n_indirect_hl(gb, 4);    break;
    case 0xe7: set_n_r(gb, 4, CPU_R_A);     break;
    case 0xe8: set_n_r(gb, 5, CPU_R_B);     break;
    case 0xe9: set_n_r(gb, 5, CPU_R_C);     break;
    case 0xea: set_n_r(gb, 5, CPU_R_D);     break;
    case 0xeb: set_n_r(gb, 5, CPU_R_E);     break;
    case 0xec: set_n_r(gb, 5, CPU_R_H);     break;
    case 0xed: set_n_r(gb, 5, CPU_R_L);     break;
    case 0xee: set_n_indirect_hl(gb, 5);    break;
    case 0xef: set_n_r(gb, 5, CPU_R_A);     break;
    case 0xf0: set_n_r(gb, 6, CPU_R_B);     break;
    case 0xf1: set_n_r(gb, 6, CPU_R_C);     break;
    case 0xf2: set_n_r(gb, 6, CPU_R_D);     break;
    case 0xf3: set_n_r(gb, 6, CPU_R_E);     break;
    case 0xf4: set_n_r(gb, 6, CPU_R_H);     break;
    case 0xf5: set_n_r(gb, 6, CPU_R_L);     break;
    case 0xf6: set_n_indirect_hl(gb, 6);    break;
    case 0xf7: set_n_r(gb, 6, CPU_R_A);     break;
    case 0xf8: set_n_r(gb, 7, CPU_R_B);     break;
    case 0xf9: set_n_r(gb, 7, CPU_R_C);     break;
    case 0xfa: set_n_r(gb, 7, CPU_R_D);     break;
    case 0xfb: set_n_r(gb, 7, CPU_R_E);     break;
    case 0xfc: set_n_r(gb, 7, CPU_R_H);     break;
    case 0xfd: set_n_r(gb, 7, CPU_R_L);     break;
    case 0xfe: set_n_indirect_hl(gb, 7);    break;
    case 0xff: set_n_r(gb, 7, CPU_R_A);     break;
    default:
        break;
    }
}

static void cpu_execute_instruction(gb_t *gb, cpu_mode_t mode)
{
    gb->cpu.opcode_pc = gb->cpu.regs.pc;
    uint8_t opcode = gb->cpu.opcode = fetch8(gb);

    if (mode == CPU_MODE_HALT_BUG)
        gb->cpu.regs.pc--;
    switch (opcode) {
    case 0x00:                                  break;
    case 0x01: ld_rr_nn(gb, CPU_RR_BC);         break;
    case 0x02: ld_indirect_rr_a(gb, CPU_RR_BC); break;
    case 0x03: inc_rr(gb, CPU_RR_BC);           break;
    case 0x04: inc_r(gb, CPU_R_B);              break;
    case 0x05: dec_r(gb, CPU_R_B);              break;
    case 0x06: ld_r_n(gb, CPU_R_B);             break;
    case 0x07: rlca(gb);                        break;
    case 0x08: ld_indirect_nn_sp(gb);           break;
    case 0x09: add_hl_rr(gb, CPU_RR_BC);        break;
    case 0x0a: ld_a_indirect_rr(gb, CPU_RR_BC); break;
    case 0x0b: dec_rr(gb, CPU_RR_BC);           break;
    case 0x0c: inc_r(gb, CPU_R_C);              break;
    case 0x0d: dec_r(gb, CPU_R_C);              break;
    case 0x0e: ld_r_n(gb, CPU_R_C);             break;
    case 0x0f: rrca(gb);                        break;
    case 0x10: stop(gb);                        break;
    case 0x11: ld_rr_nn(gb, CPU_RR_DE);         break;
    case 0x12: ld_indirect_rr_a(gb, CPU_RR_DE); break;
    case 0x13: inc_rr(gb, CPU_RR_DE);           break;
    case 0x14: inc_r(gb, CPU_R_D);              break;
    case 0x15: dec_r(gb, CPU_R_D);              break;
    case 0x16: ld_r_n(gb, CPU_R_D);             break;
    case 0x17: rla(gb);                         break;
    case 0x18: jr_i8(gb);                       break;
    case 0x19: add_hl_rr(gb, CPU_RR_DE);        break;
    case 0x1a: ld_a_indirect_rr(gb, CPU_RR_DE); break;
    case 0x1b: dec_rr(gb, CPU_RR_DE);           break;
    case 0x1c: inc_r(gb, CPU_R_E);              break;
    case 0x1d: dec_r(gb, CPU_R_E);              break;
    case 0x1e: ld_r_n(gb, CPU_R_E);             break;
    case 0x1f: rra(gb);                         break;
    case 0x20: jr_f_i8(gb, CPU_FLAG_COND_NZ);   break;
    case 0x21: ld_rr_nn(gb, CPU_RR_HL);         break;
    case 0x22: ldi_indirect_hl_a(gb);           break;
    case 0x23: inc_rr(gb, CPU_RR_HL);           break;
    case 0x24: inc_r(gb, CPU_R_H);              break;
    case 0x25: dec_r(gb, CPU_R_H);              break;
    case 0x26: ld_r_n(gb, CPU_R_H);             break;
    case 0x27: daa(gb);                         break;
    case 0x28: jr_f_i8(gb, CPU_FLAG_COND_Z);    break;
    case 0x29: add_hl_rr(gb, CPU_RR_HL);        break;
    case 0x2a: ldi_a_indirect_hl(gb);           break;
    case 0x2b: dec_rr(gb, CPU_RR_HL);           break;
    case 0x2c: inc_r(gb, CPU_R_L);              break;
    case 0x2d: dec_r(gb, CPU_R_L);              break;
    case 0x2e: ld_r_n(gb, CPU_R_L);             break;
    case 0x2f: cpl(gb);                         break;
    case 0x30: jr_f_i8(gb, CPU_FLAG_COND_NC);   break;
    case 0x31: ld_rr_nn(gb, CPU_RR_SP);         break;
    case 0x32: ldd_indirect_hl_a(gb);           break;
    case 0x33: inc_rr(gb, CPU_RR_SP);           break;
    case 0x34: inc_indirect_hl(gb);             break;
    case 0x35: dec_indirect_hl(gb);             break;
    case 0x36: ld_indirect_hl_n(gb);            break;
    case 0x37: scf(gb);                         break;
    case 0x38: jr_f_i8(gb, CPU_FLAG_COND_C);    break;
    case 0x39: add_hl_rr(gb, CPU_RR_SP);        break;
    case 0x3a: ldd_a_indirect_hl(gb);           break;
    case 0x3b: dec_rr(gb, CPU_RR_SP);           break;
    case 0x3c: inc_r(gb, CPU_R_A);              break;
    case 0x3d: dec_r(gb, CPU_R_A);              break;
    case 0x3e: ld_r_n(gb, CPU_R_A);             break;
    case 0x3f: ccf(gb);                         break;
    case 0x40: gb->cpu.regs.b = gb->cpu.regs.b; break;
    case 0x41: gb->cpu.regs.b = gb->cpu.regs.c; break;
    case 0x42: gb->cpu.regs.b = gb->cpu.regs.d; break;
    case 0x43: gb->cpu.regs.b = gb->cpu.regs.e; break;
    case 0x44: gb->cpu.regs.b = gb->cpu.regs.h; break;
    case 0x45: gb->cpu.regs.b = gb->cpu.regs.l; break;
    case 0x46: ld_r_indirect_hl(gb, CPU_R_B);   break;
    case 0x47: gb->cpu.regs.b = gb->cpu.regs.a; break;
    case 0x48: gb->cpu.regs.c = gb->cpu.regs.b; break;
    case 0x49: gb->cpu.regs.c = gb->cpu.regs.c; break;
    case 0x4a: gb->cpu.regs.c = gb->cpu.regs.d; break;
    case 0x4b: gb->cpu.regs.c = gb->cpu.regs.e; break;
    case 0x4c: gb->cpu.regs.c = gb->cpu.regs.h; break;
    case 0x4d: gb->cpu.regs.c = gb->cpu.regs.l; break;
    case 0x4e: ld_r_indirect_hl(gb, CPU_R_C);   break;
    case 0x4f: gb->cpu.regs.c = gb->cpu.regs.a; break;
    case 0x50: gb->cpu.regs.d = gb->cpu.regs.b; break;
    case 0x51: gb->cpu.regs.d = gb->cpu.regs.c; break;
    case 0x52: gb->cpu.regs.d = gb->cpu.regs.d; break;
    case 0x53: gb->cpu.regs.d = gb->cpu.regs.e; break;
    case 0x54: gb->cpu.regs.d = gb->cpu.regs.h; break;
    case 0x55: gb->cpu.regs.d = gb->cpu.regs.l; break;
    case 0x56: ld_r_indirect_hl(gb, CPU_R_D);   break;
    case 0x57: gb->cpu.regs.d = gb->cpu.regs.a; break;
    case 0x58: gb->cpu.regs.e = gb->cpu.regs.b; break;
    case 0x59: gb->cpu.regs.e = gb->cpu.regs.c; break;
    case 0x5a: gb->cpu.regs.e = gb->cpu.regs.d; break;
    case 0x5b: gb->cpu.regs.e = gb->cpu.regs.e; break;
    case 0x5c: gb->cpu.regs.e = gb->cpu.regs.h; break;
    case 0x5d: gb->cpu.regs.e = gb->cpu.regs.l; break;
    case 0x5e: ld_r_indirect_hl(gb, CPU_R_E);   break;
    case 0x5f: gb->cpu.regs.e = gb->cpu.regs.a; break;
    case 0x60: gb->cpu.regs.h = gb->cpu.regs.b; break;
    case 0x61: gb->cpu.regs.h = gb->cpu.regs.c; break;
    case 0x62: gb->cpu.regs.h = gb->cpu.regs.d; break;
    case 0x63: gb->cpu.regs.h = gb->cpu.regs.e; break;
    case 0x64: gb->cpu.regs.h = gb->cpu.regs.h; break;
    case 0x65: gb->cpu.regs.h = gb->cpu.regs.l; break;
    case 0x66: ld_r_indirect_hl(gb, CPU_R_H);   break;
    case 0x67: gb->cpu.regs.h = gb->cpu.regs.a; break;
    case 0x68: gb->cpu.regs.l = gb->cpu.regs.b; break;
    case 0x69: gb->cpu.regs.l = gb->cpu.regs.c; break;
    case 0x6a: gb->cpu.regs.l = gb->cpu.regs.d; break;
    case 0x6b: gb->cpu.regs.l = gb->cpu.regs.e; break;
    case 0x6c: gb->cpu.regs.l = gb->cpu.regs.h; break;
    case 0x6d: gb->cpu.regs.l = gb->cpu.regs.l; break;
    case 0x6e: ld_r_indirect_hl(gb, CPU_R_L);   break;
    case 0x6f: gb->cpu.regs.l = gb->cpu.regs.a; break;
    case 0x70: ld_indirect_hl_r(gb, CPU_R_B);   break;
    case 0x71: ld_indirect_hl_r(gb, CPU_R_C);   break;
    case 0x72: ld_indirect_hl_r(gb, CPU_R_D);   break;
    case 0x73: ld_indirect_hl_r(gb, CPU_R_E);   break;
    case 0x74: ld_indirect_hl_r(gb, CPU_R_H);   break;
    case 0x75: ld_indirect_hl_r(gb, CPU_R_L);   break;
    case 0x76: halt(gb);                        break;
    case 0x77: ld_indirect_hl_r(gb, CPU_R_A);   break;
    case 0x78: gb->cpu.regs.a = gb->cpu.regs.b; break;
    case 0x79: gb->cpu.regs.a = gb->cpu.regs.c; break;
    case 0x7a: gb->cpu.regs.a = gb->cpu.regs.d; break;
    case 0x7b: gb->cpu.regs.a = gb->cpu.regs.e; break;
    case 0x7c: gb->cpu.regs.a = gb->cpu.regs.h; break;
    case 0x7d: gb->cpu.regs.a = gb->cpu.regs.l; break;
    case 0x7e: ld_r_indirect_hl(gb, CPU_R_A);   break;
    case 0x7f: gb->cpu.regs.a = gb->cpu.regs.a; break;
    case 0x80: add_a_r(gb, CPU_R_B);            break;
    case 0x81: add_a_r(gb, CPU_R_C);            break;
    case 0x82: add_a_r(gb, CPU_R_D);            break;
    case 0x83: add_a_r(gb, CPU_R_E);            break;
    case 0x84: add_a_r(gb, CPU_R_H);            break;
    case 0x85: add_a_r(gb, CPU_R_L);            break;
    case 0x86: add_a_indirect_hl(gb);           break;
    case 0x87: add_a_r(gb, CPU_R_A);            break;
    case 0x88: adc_a_r(gb, CPU_R_B);            break;
    case 0x89: adc_a_r(gb, CPU_R_C);            break;
    case 0x8a: adc_a_r(gb, CPU_R_D);            break;
    case 0x8b: adc_a_r(gb, CPU_R_E);            break;
    case 0x8c: adc_a_r(gb, CPU_R_H);            break;
    case 0x8d: adc_a_r(gb, CPU_R_L);            break;
    case 0x8e: adc_a_indirect_hl(gb);           break;
    case 0x8f: adc_a_r(gb, CPU_R_A);            break;
    case 0x90: sub_a_r(gb, CPU_R_B);            break;
    case 0x91: sub_a_r(gb, CPU_R_C);            break;
    case 0x92: sub_a_r(gb, CPU_R_D);            break;
    case 0x93: sub_a_r(gb, CPU_R_E);            break;
    case 0x94: sub_a_r(gb, CPU_R_H);            break;
    case 0x95: sub_a_r(gb, CPU_R_L);            break;
    case 0x96: sub_a_indirect_hl(gb);           break;
    case 0x97: sub_a_r(gb, CPU_R_A);            break;
    case 0x98: sbc_a_r(gb, CPU_R_B);            break;
    case 0x99: sbc_a_r(gb, CPU_R_C);            break;
    case 0x9a: sbc_a_r(gb, CPU_R_D);            break;
    case 0x9b: sbc_a_r(gb, CPU_R_E);            break;
    case 0x9c: sbc_a_r(gb, CPU_R_H);            break;
    case 0x9d: sbc_a_r(gb, CPU_R_L);            break;
    case 0x9e: sbc_a_indirect_hl(gb);           break;
    case 0x9f: sbc_a_r(gb, CPU_R_A);            break;
    case 0xa0: and_a_r(gb, CPU_R_B);            break;
    case 0xa1: and_a_r(gb, CPU_R_C);            break;
    case 0xa2: and_a_r(gb, CPU_R_D);            break;
    case 0xa3: and_a_r(gb, CPU_R_E);            break;
    case 0xa4: and_a_r(gb, CPU_R_H);            break;
    case 0xa5: and_a_r(gb, CPU_R_L);            break;
    case 0xa6: and_a_indirect_hl(gb);           break;
    case 0xa7: and_a_r(gb, CPU_R_A);            break;
    case 0xa8: xor_a_r(gb, CPU_R_B);            break;
    case 0xa9: xor_a_r(gb, CPU_R_C);            break;
    case 0xaa: xor_a_r(gb, CPU_R_D);            break;
    case 0xab: xor_a_r(gb, CPU_R_E);            break;
    case 0xac: xor_a_r(gb, CPU_R_H);            break;
    case 0xad: xor_a_r(gb, CPU_R_L);            break;
    case 0xae: xor_a_indirect_hl(gb);           break;
    case 0xaf: xor_a_r(gb, CPU_R_A);            break;
    case 0xb0: or_a_r(gb, CPU_R_B);             break;
    case 0xb1: or_a_r(gb, CPU_R_C);             break;
    case 0xb2: or_a_r(gb, CPU_R_D);             break;
    case 0xb3: or_a_r(gb, CPU_R_E);             break;
    case 0xb4: or_a_r(gb, CPU_R_H);             break;
    case 0xb5: or_a_r(gb, CPU_R_L);             break;
    case 0xb6: or_a_indirect_hl(gb);            break;
    case 0xb7: or_a_r(gb, CPU_R_A);             break;
    case 0xb8: cp_a_r(gb, CPU_R_B);             break;
    case 0xb9: cp_a_r(gb, CPU_R_C);             break;
    case 0xba: cp_a_r(gb, CPU_R_D);             break;
    case 0xbb: cp_a_r(gb, CPU_R_E);             break;
    case 0xbc: cp_a_r(gb, CPU_R_H);             break;
    case 0xbd: cp_a_r(gb, CPU_R_L);             break;
    case 0xbe: cp_a_indirect_hl(gb);            break;
    case 0xbf: cp_a_r(gb, CPU_R_A);             break;
    case 0xc0: ret_f(gb, CPU_FLAG_COND_NZ);     break;
    case 0xc1: pop_rr(gb, CPU_RR_BC);           break;
    case 0xc2: jp_f_nn(gb, CPU_FLAG_COND_NZ);   break;
    case 0xc3: jp_nn(gb);                       break;
    case 0xc4: call_f_nn(gb, CPU_FLAG_COND_NZ); break;
    case 0xc5: push_rr(gb, CPU_RR_BC);          break;
    case 0xc6: add_a_n(gb);                     break;
    case 0xc7: rst_n(gb, 0x00);                 break;
    case 0xc8: ret_f(gb, CPU_FLAG_COND_Z);      break;
    case 0xc9: ret(gb);                         break;
    case 0xca: jp_f_nn(gb, CPU_FLAG_COND_Z);    break;
    case 0xcb: cpu_execute_cb_instruction(gb);  break;
    case 0xcc: call_f_nn(gb, CPU_FLAG_COND_Z);  break;
    case 0xcd: call_nn(gb);                     break;
    case 0xce: adc_a_n(gb);                     break;
    case 0xcf: rst_n(gb, 0x08);                 break;
    case 0xd0: ret_f(gb, CPU_FLAG_COND_NC);     break;
    case 0xd1: pop_rr(gb, CPU_RR_DE);           break;
    case 0xd2: jp_f_nn(gb, CPU_FLAG_COND_NC);   break;
    case 0xd3:                                  break;
    case 0xd4: call_f_nn(gb, CPU_FLAG_COND_NC); break;
    case 0xd5: push_rr(gb, CPU_RR_DE);          break;
    case 0xd6: sub_a_n(gb);                     break;
    case 0xd7: rst_n(gb, 0x10);                 break;
    case 0xd8: ret_f(gb, CPU_FLAG_COND_C);      break;
    case 0xd9: reti(gb);                        break;
    case 0xda: jp_f_nn(gb, CPU_FLAG_COND_C);    break;
    case 0xdb:                                  break;
    case 0xdc: call_f_nn(gb, CPU_FLAG_COND_C);  break;
    case 0xdd:                                  break;
    case 0xde: sbc_a_n(gb);                     break;
    case 0xdf: rst_n(gb, 0x18);                 break;
    case 0xe0: ldh_indirect_n_a(gb);            break;
    case 0xe1: pop_rr(gb, CPU_RR_HL);           break;
    case 0xe2: ldh_indirect_c_a(gb);            break;
    case 0xe3:                                  break;
    case 0xe4:                                  break;
    case 0xe5: push_rr(gb, CPU_RR_HL);          break;
    case 0xe6: and_a_n(gb);                     break;
    case 0xe7: rst_n(gb, 0x20);                 break;
    case 0xe8: add_sp_i8(gb);                   break;
    case 0xe9: jp_hl(gb);                       break;
    case 0xea: ld_indirect_nn_a(gb);            break;
    case 0xeb:                                  break;
    case 0xec:                                  break;
    case 0xed:                                  break;
    case 0xee: xor_a_n(gb);                     break;
    case 0xef: rst_n(gb, 0x28);                 break;
    case 0xf0: ldh_a_indirect_n(gb);            break;
    case 0xf1: pop_rr(gb, CPU_RR_AF);           break;
    case 0xf2: ldh_a_indirect_c(gb);            break;
    case 0xf3: di(gb);                          break;
    case 0xf4:                                  break;
    case 0xf5: push_rr(gb, CPU_RR_AF);          break;
    case 0xf6: or_a_n(gb);                      break;
    case 0xf7: rst_n(gb, 0x30);                 break;
    case 0xf8: ld_hl_sp_i8(gb);                 break;
    case 0xf9: ld_sp_hl(gb);                    break;
    case 0xfa: ld_a_indirect_nn(gb);            break;
    case 0xfb: ei(gb);                          break;
    case 0xfc:                                  break;
    case 0xfd:                                  break;
    case 0xfe: cp_a_n(gb);                      break;
    case 0xff: rst_n(gb, 0x38);                 break;
    default:
        break;
    }
    // blarggs test - serial output
    if (gb->mem[0xff02] == 0x81) {
        fprintf(stdout, "%c", gb->mem[0xff01]);
        gb->mem[0xff02] = 0x00;
    }
}

void cpu_step(gb_t *gb)
{
    bool interrupt = false;

    //sdl_handle_input(gb);
    switch (gb->cpu.mode) {
    case CPU_MODE_NORMAL:
        cpu_execute_instruction(gb, gb->cpu.mode);
        interrupt = interrupt_check(gb);
        break;
    case CPU_MODE_HALT:
        cpu_tick4(gb);
        if (is_any_interrupt_pending(gb))
            gb->cpu.mode = CPU_MODE_NORMAL;
        break;
    case CPU_MODE_HALT_BUG:
        gb->cpu.mode = CPU_MODE_NORMAL;
        cpu_execute_instruction(gb, CPU_MODE_HALT_BUG);
        interrupt = interrupt_check(gb);
        break;
    case CPU_MODE_STOP:
        // TODO: implement it when working with joypad
        break;
    case CPU_MODE_SET_IME:
        gb->intr.ime = true;
        gb->cpu.mode = CPU_MODE_NORMAL;
        break;
    default:
        break;
    }
    if (interrupt)
        interrupt_handler(gb);
}