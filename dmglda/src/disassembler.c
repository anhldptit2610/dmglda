#include "disassembler.h"

void disassembler_cb_write(FILE *file, gb_t *gb)
{
    switch (gb->cpu.operand1) {
    case 0x00:
        Disassembler_Log("RLC B\n");
        break;
    case 0x01:
        Disassembler_Log("RLC C\n");
        break;
    case 0x02:
        Disassembler_Log("RLC D\n");
        break;
    case 0x03:
        Disassembler_Log("RLC E\n");
        break;
    case 0x04:
        Disassembler_Log("RLC H\n");
        break;
    case 0x05:
        Disassembler_Log("RLC L\n");
        break;
    case 0x06:
        Disassembler_Log("RLC (HL)\n");
        break;
    case 0x07:
        Disassembler_Log("RLC A\n");
        break;
    case 0x08:
        Disassembler_Log("RRC B\n");
        break;
    case 0x09:
        Disassembler_Log("RRC C\n");
        break;
    case 0x0a:
        Disassembler_Log("RRC D\n");
        break;
    case 0x0b:
        Disassembler_Log("RRC E\n");
        break;
    case 0x0c:
        Disassembler_Log("RRC H\n");
        break;
    case 0x0d:
        Disassembler_Log("RRC L\n");
        break;
    case 0x0e:
        Disassembler_Log("RRC (HL)\n");
        break;
    case 0x0f:
        Disassembler_Log("RRC A\n");
        break;
    case 0x10:
        Disassembler_Log("RL B\n");
        break;
    case 0x11:
        Disassembler_Log("RL C\n");
        break;
    case 0x12:
        Disassembler_Log("RL D\n");
        break;
    case 0x13:
        Disassembler_Log("RL E\n");
        break;
    case 0x14:
        Disassembler_Log("RL H\n");
        break;
    case 0x15:
        Disassembler_Log("RL L\n");
        break;
    case 0x16:
        Disassembler_Log("RL (HL)\n");
        break;
    case 0x17:
        Disassembler_Log("RL A\n");
        break;
    case 0x18:
        Disassembler_Log("RR B\n");
        break;
    case 0x19:
        Disassembler_Log("RR C\n");
        break;
    case 0x1a:
        Disassembler_Log("RR D\n");
        break;
    case 0x1b:
        Disassembler_Log("RR E\n");
        break;
    case 0x1c:
        Disassembler_Log("RR H\n");
        break;
    case 0x1d:
        Disassembler_Log("RR L\n");
        break;
    case 0x1e:
        Disassembler_Log("RR (HL)\n");
        break;
    case 0x1f:
        Disassembler_Log("RR A\n");
        break;
    case 0x20:
        Disassembler_Log("SLA B\n");
        break;
    case 0x21:
        Disassembler_Log("SLA C\n");
        break;
    case 0x22:
        Disassembler_Log("SLA D\n");
        break;
    case 0x23:
        Disassembler_Log("SLA E\n");
        break;
    case 0x24:
        Disassembler_Log("SLA H\n");
        break;
    case 0x25:
        Disassembler_Log("SLA L\n");
        break;
    case 0x26:
        Disassembler_Log("SLA (HL)\n");
        break;
    case 0x27:
        Disassembler_Log("SLA A\n");
        break;
    case 0x28:
        Disassembler_Log("SRA B\n");
        break;
    case 0x29:
        Disassembler_Log("SRA C\n");
        break;
    case 0x2a:
        Disassembler_Log("SRA D\n");
        break;
    case 0x2b:
        Disassembler_Log("SRA E\n");
        break;
    case 0x2c:
        Disassembler_Log("SRA H\n");
        break;
    case 0x2d:
        Disassembler_Log("SRA L\n");
        break;
    case 0x2e:
        Disassembler_Log("SRA (HL)\n");
        break;
    case 0x2f:
        Disassembler_Log("SRA A\n");
        break;
    case 0x30:
        Disassembler_Log("SWAP B\n");
        break;
    case 0x31:
        Disassembler_Log("SWAP C\n");
        break;
    case 0x32:
        Disassembler_Log("SWAP D\n");
        break;
    case 0x33:
        Disassembler_Log("SWAP E\n");
        break;
    case 0x34:
        Disassembler_Log("SWAP H\n");
        break;
    case 0x35:
        Disassembler_Log("SWAP L\n");
        break;
    case 0x36:
        Disassembler_Log("SWAP (HL)\n");
        break;
    case 0x37:
        Disassembler_Log("SWAP A\n");
        break;
    case 0x38:
        Disassembler_Log("SRL B\n");
        break;
    case 0x39:
        Disassembler_Log("SRL C\n");
        break;
    case 0x3a:
        Disassembler_Log("SRL D\n");
        break;
    case 0x3b:
        Disassembler_Log("SRL E\n");
        break;
    case 0x3c:
        Disassembler_Log("SRL H\n");
        break;
    case 0x3d:
        Disassembler_Log("SRL L\n");
        break;
    case 0x3e:
        Disassembler_Log("SRL (HL)\n");
        break;
    case 0x3f:
        Disassembler_Log("SRL A\n");
        break;
    case 0x40:
        Disassembler_Log("BIT 0,B\n");
        break;
    case 0x41:
        Disassembler_Log("BIT 0,C\n");
        break;
    case 0x42:
        Disassembler_Log("BIT 0,D\n");
        break;
    case 0x43:
        Disassembler_Log("BIT 0,E\n");
        break;
    case 0x44:
        Disassembler_Log("BIT 0,H\n");
        break;
    case 0x45:
        Disassembler_Log("BIT 0,L\n");
        break;
    case 0x46:
        Disassembler_Log("BIT 0,(HL)\n");
        break;
    case 0x47:
        Disassembler_Log("BIT 0,A\n");
        break;
    case 0x48:
        Disassembler_Log("BIT 1,B\n");
        break;
    case 0x49:
        Disassembler_Log("BIT 1,C\n");
        break;
    case 0x4a:
        Disassembler_Log("BIT 1,D\n");
        break;
    case 0x4b:
        Disassembler_Log("BIT 1,E\n");
        break;
    case 0x4c:
        Disassembler_Log("BIT 1,H\n");
        break;
    case 0x4d:
        Disassembler_Log("BIT 1,L\n");
        break;
    case 0x4e:
        Disassembler_Log("BIT 1,(HL)\n");
        break;
    case 0x4f:
        Disassembler_Log("BIT 1,A\n");
        break;
    case 0x50:
        Disassembler_Log("BIT 2,B\n");
        break;
    case 0x51:
        Disassembler_Log("BIT 2,C\n");
        break;
    case 0x52:
        Disassembler_Log("BIT 2,D\n");
        break;
    case 0x53:
        Disassembler_Log("BIT 2,E\n");
        break;
    case 0x54:
        Disassembler_Log("BIT 2,H\n");
        break;
    case 0x55:
        Disassembler_Log("BIT 2,L\n");
        break;
    case 0x56:
        Disassembler_Log("BIT 2,(HL)\n");
        break;
    case 0x57:
        Disassembler_Log("BIT 2,A\n");
        break;
    case 0x58:
        Disassembler_Log("BIT 3,B\n");
        break;
    case 0x59:
        Disassembler_Log("BIT 3,C\n");
        break;
    case 0x5a:
        Disassembler_Log("BIT 3,D\n");
        break;
    case 0x5b:
        Disassembler_Log("BIT 3,E\n");
        break;
    case 0x5c:
        Disassembler_Log("BIT 3,H\n");
        break;
    case 0x5d:
        Disassembler_Log("BIT 3,L\n");
        break;
    case 0x5e:
        Disassembler_Log("BIT 3,(HL)\n");
        break;
    case 0x5f:
        Disassembler_Log("BIT 3,A\n");
        break;
    case 0x60:
        Disassembler_Log("BIT 4,B\n");
        break;
    case 0x61:
        Disassembler_Log("BIT 4,C\n");
        break;
    case 0x62:
        Disassembler_Log("BIT 4,D\n");
        break;
    case 0x63:
        Disassembler_Log("BIT 4,E\n");
        break;
    case 0x64:
        Disassembler_Log("BIT 4,H\n");
        break;
    case 0x65:
        Disassembler_Log("BIT 4,L\n");
        break;
    case 0x66:
        Disassembler_Log("BIT 4,(HL)\n");
        break;
    case 0x67:
        Disassembler_Log("BIT 4,A\n");
        break;
    case 0x68:
        Disassembler_Log("BIT 5,B\n");
        break;
    case 0x69:
        Disassembler_Log("BIT 5,C\n");
        break;
    case 0x6a:
        Disassembler_Log("BIT 5,D\n");
        break;
    case 0x6b:
        Disassembler_Log("BIT 5,E\n");
        break;
    case 0x6c:
        Disassembler_Log("BIT 5,H\n");
        break;
    case 0x6d:
        Disassembler_Log("BIT 5,L\n");
        break;
    case 0x6e:
        Disassembler_Log("BIT 5,(HL)\n");
        break;
    case 0x6f:
        Disassembler_Log("BIT 5,A\n");
        break;
    case 0x70:
        Disassembler_Log("BIT 6,B\n");
        break;
    case 0x71:
        Disassembler_Log("BIT 6,C\n");
        break;
    case 0x72:
        Disassembler_Log("BIT 6,D\n");
        break;
    case 0x73:
        Disassembler_Log("BIT 6,E\n");
        break;
    case 0x74:
        Disassembler_Log("BIT 6,H\n");
        break;
    case 0x75:
        Disassembler_Log("BIT 6,L\n");
        break;
    case 0x76:
        Disassembler_Log("BIT 6,(HL)\n");
        break;
    case 0x77:
        Disassembler_Log("BIT 6,A\n");
        break;
    case 0x78:
        Disassembler_Log("BIT 7,B\n");
        break;
    case 0x79:
        Disassembler_Log("BIT 7,C\n");
        break;
    case 0x7a:
        Disassembler_Log("BIT 7,D\n");
        break;
    case 0x7b:
        Disassembler_Log("BIT 7,E\n");
        break;
    case 0x7c:
        Disassembler_Log("BIT 7,H\n");
        break;
    case 0x7d:
        Disassembler_Log("BIT 7,L\n");
        break;
    case 0x7e:
        Disassembler_Log("BIT 7,(HL)\n");
        break;
    case 0x7f:
        Disassembler_Log("BIT 7,A\n");
        break;
    case 0x80:
        Disassembler_Log("RES 0,B\n");
        break;
    case 0x81:
        Disassembler_Log("RES 0,C\n");
        break;
    case 0x82:
        Disassembler_Log("RES 0,D\n");
        break;
    case 0x83:
        Disassembler_Log("RES 0,E\n");
        break;
    case 0x84:
        Disassembler_Log("RES 0,H\n");
        break;
    case 0x85:
        Disassembler_Log("RES 0,L\n");
        break;
    case 0x86:
        Disassembler_Log("RES 0,(HL)\n");
        break;
    case 0x87:
        Disassembler_Log("RES 0,A\n");
        break;
    case 0x88:
        Disassembler_Log("RES 1,B\n");
        break;
    case 0x89:
        Disassembler_Log("RES 1,C\n");
        break;
    case 0x8a:
        Disassembler_Log("RES 1,D\n");
        break;
    case 0x8b:
        Disassembler_Log("RES 1,E\n");
        break;
    case 0x8c:
        Disassembler_Log("RES 1,H\n");
        break;
    case 0x8d:
        Disassembler_Log("RES 1,L\n");
        break;
    case 0x8e:
        Disassembler_Log("RES 1,(HL)\n");
        break;
    case 0x8f:
        Disassembler_Log("RES 1,A\n");
        break;
    case 0x90:
        Disassembler_Log("RES 2,B\n");
        break;
    case 0x91:
        Disassembler_Log("RES 2,C\n");
        break;
    case 0x92:
        Disassembler_Log("RES 2,D\n");
        break;
    case 0x93:
        Disassembler_Log("RES 2,E\n");
        break;
    case 0x94:
        Disassembler_Log("RES 2,H\n");
        break;
    case 0x95:
        Disassembler_Log("RES 2,L\n");
        break;
    case 0x96:
        Disassembler_Log("RES 2,(HL)\n");
        break;
    case 0x97:
        Disassembler_Log("RES 2,A\n");
        break;
    case 0x98:
        Disassembler_Log("RES 3,B\n");
        break;
    case 0x99:
        Disassembler_Log("RES 3,C\n");
        break;
    case 0x9a:
        Disassembler_Log("RES 3,D\n");
        break;
    case 0x9b:
        Disassembler_Log("RES 3,E\n");
        break;
    case 0x9c:
        Disassembler_Log("RES 3,H\n");
        break;
    case 0x9d:
        Disassembler_Log("RES 3,L\n");
        break;
    case 0x9e:
        Disassembler_Log("RES 3,(HL)\n");
        break;
    case 0x9f:
        Disassembler_Log("RES 3,A\n");
        break;
    case 0xa0:
        Disassembler_Log("RES 4,B\n");
        break;
    case 0xa1:
        Disassembler_Log("RES 4,C\n");
        break;
    case 0xa2:
        Disassembler_Log("RES 4,D\n");
        break;
    case 0xa3:
        Disassembler_Log("RES 4,E\n");
        break;
    case 0xa4:
        Disassembler_Log("RES 4,H\n");
        break;
    case 0xa5:
        Disassembler_Log("RES 4,L\n");
        break;
    case 0xa6:
        Disassembler_Log("RES 4,(HL)\n");
        break;
    case 0xa7:
        Disassembler_Log("RES 4,A\n");
        break;
    case 0xa8:
        Disassembler_Log("RES 5,B\n");
        break;
    case 0xa9:
        Disassembler_Log("RES 5,C\n");
        break;
    case 0xaa:
        Disassembler_Log("RES 5,D\n");
        break;
    case 0xab:
        Disassembler_Log("RES 5,E\n");
        break;
    case 0xac:
        Disassembler_Log("RES 5,H\n");
        break;
    case 0xad:
        Disassembler_Log("RES 5,L\n");
        break;
    case 0xae:
        Disassembler_Log("RES 5,(HL)\n");
        break;
    case 0xaf:
        Disassembler_Log("RES 5,A\n");
        break;
    case 0xb0:
        Disassembler_Log("RES 6,B\n");
        break;
    case 0xb1:
        Disassembler_Log("RES 6,C\n");
        break;
    case 0xb2:
        Disassembler_Log("RES 6,D\n");
        break;
    case 0xb3:
        Disassembler_Log("RES 6,E\n");
        break;
    case 0xb4:
        Disassembler_Log("RES 6,H\n");
        break;
    case 0xb5:
        Disassembler_Log("RES 6,L\n");
        break;
    case 0xb6:
        Disassembler_Log("RES 6,(HL)\n");
        break;
    case 0xb7:
        Disassembler_Log("RES 6,A\n");
        break;
    case 0xb8:
        Disassembler_Log("RES 7,B\n");
        break;
    case 0xb9:
        Disassembler_Log("RES 7,C\n");
        break;
    case 0xba:
        Disassembler_Log("RES 7,D\n");
        break;
    case 0xbb:
        Disassembler_Log("RES 7,E\n");
        break;
    case 0xbc:
        Disassembler_Log("RES 7,H\n");
        break;
    case 0xbd:
        Disassembler_Log("RES 7,L\n");
        break;
    case 0xbe:
        Disassembler_Log("RES 7,(HL)\n");
        break;
    case 0xbf:
        Disassembler_Log("RES 7,A\n");
        break;
    case 0xc0:
        Disassembler_Log("SET 0,B\n");
        break;
    case 0xc1:
        Disassembler_Log("SET 0,C\n");
        break;
    case 0xc2:
        Disassembler_Log("SET 0,D\n");
        break;
    case 0xc3:
        Disassembler_Log("SET 0,E\n");
        break;
    case 0xc4:
        Disassembler_Log("SET 0,H\n");
        break;
    case 0xc5:
        Disassembler_Log("SET 0,L\n");
        break;
    case 0xc6:
        Disassembler_Log("SET 0,(HL)\n");
        break;
    case 0xc7:
        Disassembler_Log("SET 0,A\n");
        break;
    case 0xc8:
        Disassembler_Log("SET 1,B\n");
        break;
    case 0xc9:
        Disassembler_Log("SET 1,C\n");
        break;
    case 0xca:
        Disassembler_Log("SET 1,D\n");
        break;
    case 0xcb:
        Disassembler_Log("SET 1,E\n");
        break;
    case 0xcc:
        Disassembler_Log("SET 1,H\n");
        break;
    case 0xcd:
        Disassembler_Log("SET 1,L\n");
        break;
    case 0xce:
        Disassembler_Log("SET 1,(HL)\n");
        break;
    case 0xcf:
        Disassembler_Log("SET 1,A\n");
        break;
    case 0xd0:
        Disassembler_Log("SET 2,B\n");
        break;
    case 0xd1:
        Disassembler_Log("SET 2,C\n");
        break;
    case 0xd2:
        Disassembler_Log("SET 2,D\n");
        break;
    case 0xd3:
        Disassembler_Log("SET 2,E\n");
        break;
    case 0xd4:
        Disassembler_Log("SET 2,H\n");
        break;
    case 0xd5:
        Disassembler_Log("SET 2,L\n");
        break;
    case 0xd6:
        Disassembler_Log("SET 2,(HL)\n");
        break;
    case 0xd7:
        Disassembler_Log("SET 2,A\n");
        break;
    case 0xd8:
        Disassembler_Log("SET 3,B\n");
        break;
    case 0xd9:
        Disassembler_Log("SET 3,C\n");
        break;
    case 0xda:
        Disassembler_Log("SET 3,D\n");
        break;
    case 0xdb:
        Disassembler_Log("SET 3,E\n");
        break;
    case 0xdc:
        Disassembler_Log("SET 3,H\n");
        break;
    case 0xdd:
        Disassembler_Log("SET 3,L\n");
        break;
    case 0xde:
        Disassembler_Log("SET 3,(HL)\n");
        break;
    case 0xdf:
        Disassembler_Log("SET 3,A\n");
        break;
    case 0xe0:
        Disassembler_Log("SET 4,B\n");
        break;
    case 0xe1:
        Disassembler_Log("SET 4,C\n");
        break;
    case 0xe2:
        Disassembler_Log("SET 4,D\n");
        break;
    case 0xe3:
        Disassembler_Log("SET 4,E\n");
        break;
    case 0xe4:
        Disassembler_Log("SET 4,H\n");
        break;
    case 0xe5:
        Disassembler_Log("SET 4,L\n");
        break;
    case 0xe6:
        Disassembler_Log("SET 4,(HL)\n");
        break;
    case 0xe7:
        Disassembler_Log("SET 4,A\n");
        break;
    case 0xe8:
        Disassembler_Log("SET 5,B\n");
        break;
    case 0xe9:
        Disassembler_Log("SET 5,C\n");
        break;
    case 0xea:
        Disassembler_Log("SET 5,D\n");
        break;
    case 0xeb:
        Disassembler_Log("SET 5,E\n");
        break;
    case 0xec:
        Disassembler_Log("SET 5,H\n");
        break;
    case 0xed:
        Disassembler_Log("SET 5,L\n");
        break;
    case 0xee:
        Disassembler_Log("SET 5,(HL)\n");
        break;
    case 0xef:
        Disassembler_Log("SET 5,A\n");
        break;
    case 0xf0:
        Disassembler_Log("SET 6,B\n");
        break;
    case 0xf1:
        Disassembler_Log("SET 6,C\n");
        break;
    case 0xf2:
        Disassembler_Log("SET 6,D\n");
        break;
    case 0xf3:
        Disassembler_Log("SET 6,E\n");
        break;
    case 0xf4:
        Disassembler_Log("SET 6,H\n");
        break;
    case 0xf5:
        Disassembler_Log("SET 6,L\n");
        break;
    case 0xf6:
        Disassembler_Log("SET 6,(HL)\n");
        break;
    case 0xf7:
        Disassembler_Log("SET 6,A\n");
        break;
    case 0xf8:
        Disassembler_Log("SET 7,B\n");
        break;
    case 0xf9:
        Disassembler_Log("SET 7,C\n");
        break;
    case 0xfa:
        Disassembler_Log("SET 7,D\n");
        break;
    case 0xfb:
        Disassembler_Log("SET 7,E\n");
        break;
    case 0xfc:
        Disassembler_Log("SET 7,H\n");
        break;
    case 0xfd:
        Disassembler_Log("SET 7,L\n");
        break;
    case 0xfe:
        Disassembler_Log("SET 7,(HL)\n");
        break;
    case 0xff:
        Disassembler_Log("SET 7,A\n");
        break;
    default:
        break;
    }
    fclose(file);
}

void log_cpu_state(gb_t *gb)
{
    uint16_t pc = gb->cpu.regs.pc;

    FILE *file = fopen("lda.log", "a");
    if (!file) {
        GB_Error("can't open/create log file\n");
        return;
    }
    Disassembler_Log("A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
                gb->cpu.regs.a, gb->cpu.regs.f, gb->cpu.regs.b, gb->cpu.regs.c, gb->cpu.regs.d,
                gb->cpu.regs.e, gb->cpu.regs.h, gb->cpu.regs.l, gb->cpu.regs.sp, gb->cpu.regs.pc,
                mmu_read(gb, pc), mmu_read(gb, pc + 1), mmu_read(gb, pc + 2), mmu_read(gb, pc + 3));
    fclose(file);
}   

// write instruction disassemble to a log file
void disassembler_write(gb_t *cgb)
{
    FILE *file = fopen("log.txt", "a");
    if (!file) {
        GB_Error("can't open/create log file\n");
        return;
    }

    Disassembler_Log("PC: 0x%04x ", cgb->cpu.opcode_pc);
    switch (cgb->cpu.opcode) {
    case 0x00:
        Disassembler_Log("NOP\n");
        break;
    case 0x01:
        Disassembler_Log("LD BC,$%02x%02x\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0x02:
        Disassembler_Log("LD (BC),A\n");
        break;
    case 0x03:
        Disassembler_Log("INC BC\n");
        break;
    case 0x04:
        Disassembler_Log("INC B\n");
        break;
    case 0x05:
        Disassembler_Log("DEC B\n");
        break;
    case 0x06:
        Disassembler_Log("LD B,$%02x\n", cgb->cpu.operand1);
        break;
    case 0x07:
        Disassembler_Log("RLCA\n");
        break;
    case 0x08:
        Disassembler_Log("LD ($%02x%02x),SP\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0x09:
        Disassembler_Log("ADD HL,BC\n");
        break;
    case 0x0a:
        Disassembler_Log("LD A,(BC)\n");
        break;
    case 0x0b:
        Disassembler_Log("DEC BC\n");
        break;
    case 0x0c:
        Disassembler_Log("INC C\n");
        break;
    case 0x0d:
        Disassembler_Log("DEC C\n");
        break;
    case 0x0e:
        Disassembler_Log("LD C,$%02x\n", cgb->cpu.operand1);
        break;
    case 0x0f:
        Disassembler_Log("RRCA\n");
        break;
    case 0x10:
        Disassembler_Log("STOP\n");
        break;
    case 0x11:
        Disassembler_Log("LD DE,$%02x%02x\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0x12:
        Disassembler_Log("LD (DE),A\n");
        break;
    case 0x13:
        Disassembler_Log("INC DE\n");
        break;
    case 0x14:
        Disassembler_Log("INC D\n");
        break;
    case 0x15:
        Disassembler_Log("DEC D\n");
        break;
    case 0x16:
        Disassembler_Log("LD D,$%02x\n", cgb->cpu.operand1);
        break;
    case 0x17:
        Disassembler_Log("RLA\n");
        break;
    case 0x18:
        Disassembler_Log("JR $%02x\n", cgb->cpu.operand1);
        break;
    case 0x19:
        Disassembler_Log("ADD HL,DE\n");
        break;
    case 0x1a:
        Disassembler_Log("LD A,(DE)\n");
        break;
    case 0x1b:
        Disassembler_Log("DEC DE\n");
        break;
    case 0x1c:
        Disassembler_Log("INC E\n");
        break;
    case 0x1d:
        Disassembler_Log("DEC E\n");
        break;
    case 0x1e:
        Disassembler_Log("LD E,$%02x\n", cgb->cpu.operand1);
        break;
    case 0x1f:
        Disassembler_Log("RRA\n");
        break;
    case 0x20:
        Disassembler_Log("JR NZ, $%02x\n", cgb->cpu.operand1);
        break;
    case 0x21:
        Disassembler_Log("LD HL,$%02x%02x\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0x22:
        Disassembler_Log("LD (HL+),A\n");
        break;
    case 0x23:
        Disassembler_Log("INC HL\n");
        break;
    case 0x24:
        Disassembler_Log("INC H\n");
        break;
    case 0x25:
        Disassembler_Log("DEC H\n");
        break;
    case 0x26:
        Disassembler_Log("LD H,$%02x\n", cgb->cpu.operand1);
        break;
    case 0x27:
        Disassembler_Log("DAA\n");
        break;
    case 0x28:
        Disassembler_Log("JR Z,$%02x\n", cgb->cpu.operand1);
        break;
    case 0x29:
        Disassembler_Log("ADD HL,HL\n");
        break;
    case 0x2a:
        Disassembler_Log("LD A,(HL+)\n");
        break;
    case 0x2b:
        Disassembler_Log("DEC HL\n");
        break;
    case 0x2c:
        Disassembler_Log("INC L\n");
        break;
    case 0x2d:
        Disassembler_Log("DEC L\n");
        break;
    case 0x2e:
        Disassembler_Log("LD L,$%02x\n", cgb->cpu.operand1);
        break;
    case 0x2f:
        Disassembler_Log("CPL\n");
        break;
    case 0x30:
        Disassembler_Log("JR NC,$%02x\n", cgb->cpu.operand1);
        break;
    case 0x31:
        Disassembler_Log("LD SP,$%02x%02x\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0x32:
        Disassembler_Log("LD (HL-),A\n");
        break;
    case 0x33:
        Disassembler_Log("INC SP\n");
        break;
    case 0x34:
        Disassembler_Log("INC (HL)\n");
        break;
    case 0x35:
        Disassembler_Log("DEC (HL)\n");
        break;
    case 0x36:
        Disassembler_Log("LD (HL),$%02x\n", cgb->cpu.operand1);
        break;
    case 0x37:
        Disassembler_Log("SCF\n");
        break;
    case 0x38:
        Disassembler_Log("JR C,$%02x\n", cgb->cpu.operand1);
        break;
    case 0x39:
        Disassembler_Log("ADD HL,SP\n");
        break;
    case 0x3a:
        Disassembler_Log("LD A,(HL-)\n");
        break;
    case 0x3b:
        Disassembler_Log("DEC SP\n");
        break;
    case 0x3c:
        Disassembler_Log("INC A\n");
        break;
    case 0x3d:
        Disassembler_Log("DEC A\n");
        break;
    case 0x3e:
        Disassembler_Log("LD A,$%02x\n", cgb->cpu.operand1);
        break;
    case 0x3f:
        Disassembler_Log("CCF\n");
        break;
    case 0x40:
        Disassembler_Log("LD B,B\n");
        break;
    case 0x41:
        Disassembler_Log("LD B,C\n");
        break;
    case 0x42:
        Disassembler_Log("LD B,D\n");
        break;
    case 0x43:
        Disassembler_Log("LD B,E\n");
        break;
    case 0x44:
        Disassembler_Log("LD B,H\n");
        break;
    case 0x45:
        Disassembler_Log("LD B,L\n");
        break;
    case 0x46:
        Disassembler_Log("LD B,(HL)\n");
        break;
    case 0x47:
        Disassembler_Log("LD B,A\n");
        break;
    case 0x48:
        Disassembler_Log("LD C,B\n");
        break;
    case 0x49:
        Disassembler_Log("LD C,C\n");
        break;
    case 0x4a:
        Disassembler_Log("LD C,D\n");
        break;
    case 0x4b:
        Disassembler_Log("LD C,E\n");
        break;
    case 0x4c:
        Disassembler_Log("LD C,H\n");
        break;
    case 0x4d:
        Disassembler_Log("LD C,L\n");
        break;
    case 0x4e:
        Disassembler_Log("LD C,(HL)\n");
        break;
    case 0x4f:
        Disassembler_Log("LD C,A\n");
        break;
    case 0x50:
        Disassembler_Log("LD D,B\n");
        break;
    case 0x51:
        Disassembler_Log("LD D,C\n");
        break;
    case 0x52:
        Disassembler_Log("LD D,D\n");
        break;
    case 0x53:
        Disassembler_Log("LD D,E\n");
        break;
    case 0x54:
        Disassembler_Log("LD D,H\n");
        break;
    case 0x55:
        Disassembler_Log("LD D,L\n");
        break;
    case 0x56:
        Disassembler_Log("LD D,(HL)\n");
        break;
    case 0x57:
        Disassembler_Log("LD D,A\n");
        break;
    case 0x58:
        Disassembler_Log("LD E,B\n");
        break;
    case 0x59:
        Disassembler_Log("LD E,C\n");
        break;
    case 0x5a:
        Disassembler_Log("LD E,D\n");
        break;
    case 0x5b:
        Disassembler_Log("LD E,E\n");
        break;
    case 0x5c:
        Disassembler_Log("LD E,H\n");
        break;
    case 0x5d:
        Disassembler_Log("LD E,L\n");
        break;
    case 0x5e:
        Disassembler_Log("LD E,(HL)\n");
        break;
    case 0x5f:
        Disassembler_Log("LD E,A\n");
        break;
    case 0x60:
        Disassembler_Log("LD H,B\n");
        break;
    case 0x61:
        Disassembler_Log("LD H,C\n");
        break;
    case 0x62:
        Disassembler_Log("LD H,D\n");
        break;
    case 0x63:
        Disassembler_Log("LD H,E\n");
        break;
    case 0x64:
        Disassembler_Log("LD H,H\n");
        break;
    case 0x65:
        Disassembler_Log("LD H,L\n");
        break;
    case 0x66:
        Disassembler_Log("LD H,(HL)\n");
        break;
    case 0x67:
        Disassembler_Log("LD H,A\n");
        break;
    case 0x68:
        Disassembler_Log("LD L,B\n");
        break;
    case 0x69:
        Disassembler_Log("LD L,C\n");
        break;
    case 0x6a:
        Disassembler_Log("LD L,D\n");
        break;
    case 0x6b:
        Disassembler_Log("LD L,E\n");
        break;
    case 0x6c:
        Disassembler_Log("LD L,H\n");
        break;
    case 0x6d:
        Disassembler_Log("LD L,L\n");
        break;
    case 0x6e:
        Disassembler_Log("LD L,(HL)\n");
        break;
    case 0x6f:
        Disassembler_Log("LD L,A\n");
        break;
    case 0x70:
        Disassembler_Log("LD (HL),B\n");
        break;
    case 0x71:
        Disassembler_Log("LD (HL),C\n");
        break;
    case 0x72:
        Disassembler_Log("LD (HL),D\n");
        break;
    case 0x73:
        Disassembler_Log("LD (HL),E\n");
        break;
    case 0x74:
        Disassembler_Log("LD (HL),H\n");
        break;
    case 0x75:
        Disassembler_Log("LD (HL),L\n");
        break;
    case 0x76:
        Disassembler_Log("HALT\n");
        break;
    case 0x77:
        Disassembler_Log("LD (HL),A\n");
        break;
    case 0x78:
        Disassembler_Log("LD A,B\n");
        break;
    case 0x79:
        Disassembler_Log("LD A,C\n");
        break;
    case 0x7a:
        Disassembler_Log("LD A,D\n");
        break;
    case 0x7b:
        Disassembler_Log("LD A,E\n");
        break;
    case 0x7c:
        Disassembler_Log("LD A,H\n");
        break;
    case 0x7d:
        Disassembler_Log("LD A,L\n");
        break;
    case 0x7e:
        Disassembler_Log("LD A,(HL)\n");
        break;
    case 0x7f:
        Disassembler_Log("LD A,A\n");
        break;
    case 0x80:
        Disassembler_Log("ADD A,B\n");
        break;
    case 0x81:
        Disassembler_Log("ADD A,C\n");
        break;
    case 0x82:
        Disassembler_Log("ADD A,D\n");
        break;
    case 0x83:
        Disassembler_Log("ADD A,E\n");
        break;
    case 0x84:
        Disassembler_Log("ADD A,H\n");
        break;
    case 0x85:
        Disassembler_Log("ADD A,L\n");
        break;
    case 0x86:
        Disassembler_Log("ADD A,(HL)\n");
        break;
    case 0x87:
        Disassembler_Log("ADD A,A\n");
        break;
    case 0x88:
        Disassembler_Log("ADC A,B\n");
        break;
    case 0x89:
        Disassembler_Log("ADC A,C\n");
        break;
    case 0x8a:
        Disassembler_Log("ADC A,D\n");
        break;
    case 0x8b:
        Disassembler_Log("ADC A,E\n");
        break;
    case 0x8c:
        Disassembler_Log("ADC A,H\n");
        break;
    case 0x8d:
        Disassembler_Log("ADC A,L\n");
        break;
    case 0x8e:
        Disassembler_Log("ADC A,(HL)\n");
        break;
    case 0x8f:
        Disassembler_Log("ADC A,A\n");
        break;
    case 0x90:
        Disassembler_Log("SUB A,B\n");
        break;
    case 0x91:
        Disassembler_Log("SUB A,C\n");
        break;
    case 0x92:
        Disassembler_Log("SUB A,D\n");
        break;
    case 0x93:
        Disassembler_Log("SUB A,E\n");
        break;
    case 0x94:
        Disassembler_Log("SUB A,H\n");
        break;
    case 0x95:
        Disassembler_Log("SUB A,L\n");
        break;
    case 0x96:
        Disassembler_Log("SUB A,(HL)\n");
        break;
    case 0x97:
        Disassembler_Log("SUB A,A\n");
        break;
    case 0x98:
        Disassembler_Log("SBC A,B\n");
        break;
    case 0x99:
        Disassembler_Log("SBC A,C\n");
        break;
    case 0x9a:
        Disassembler_Log("SBC A,D\n");
        break;
    case 0x9b:
        Disassembler_Log("SBC A,E\n");
        break;
    case 0x9c:
        Disassembler_Log("SBC A,H\n");
        break;
    case 0x9d:
        Disassembler_Log("SBC A,L\n");
        break;
    case 0x9e:
        Disassembler_Log("SBC A,(HL)\n");
        break;
    case 0x9f:
        Disassembler_Log("SBC A,A\n");
        break;
    case 0xa0:
        Disassembler_Log("AND A,B\n");
        break;
    case 0xa1:
        Disassembler_Log("AND A,C\n");
        break;
    case 0xa2:
        Disassembler_Log("AND A,D\n");
        break;
    case 0xa3:
        Disassembler_Log("AND A,E\n");
        break;
    case 0xa4:
        Disassembler_Log("AND A,H\n");
        break;
    case 0xa5:
        Disassembler_Log("AND A,L\n");
        break;
    case 0xa6:
        Disassembler_Log("AND A,(HL)\n");
        break;
    case 0xa7:
        Disassembler_Log("AND A,A\n");
        break;
    case 0xa8:
        Disassembler_Log("XOR A,B\n");
        break;
    case 0xa9:
        Disassembler_Log("XOR A,C\n");
        break;
    case 0xaa:
        Disassembler_Log("XOR A,D\n");
        break;
    case 0xab:
        Disassembler_Log("XOR A,E\n");
        break;
    case 0xac:
        Disassembler_Log("XOR A,H\n");
        break;
    case 0xad:
        Disassembler_Log("XOR A,L\n");
        break;
    case 0xae:
        Disassembler_Log("XOR A,(HL)\n");
        break;
    case 0xaf:
        Disassembler_Log("XOR A,A\n");
        break;
    case 0xb0:
        Disassembler_Log("OR A,B\n");
        break;
    case 0xb1:
        Disassembler_Log("OR A,C\n");
        break;
    case 0xb2:
        Disassembler_Log("OR A,D\n");
        break;
    case 0xb3:
        Disassembler_Log("OR A,E\n");
        break;
    case 0xb4:
        Disassembler_Log("OR A,H\n");
        break;
    case 0xb5:
        Disassembler_Log("OR A,L\n");
        break;
    case 0xb6:
        Disassembler_Log("OR A,(HL)\n");
        break;
    case 0xb7:
        Disassembler_Log("OR A,A\n");
        break;
    case 0xb8:
        Disassembler_Log("CP A,B\n");
        break;
    case 0xb9:
        Disassembler_Log("CP A,C\n");
        break;
    case 0xba:
        Disassembler_Log("CP A,D\n");
        break;
    case 0xbb:
        Disassembler_Log("CP A,E\n");
        break;
    case 0xbc:
        Disassembler_Log("CP A,H\n");
        break;
    case 0xbd:
        Disassembler_Log("CP A,L\n");
        break;
    case 0xbe:
        Disassembler_Log("CP A,(HL)\n");
        break;
    case 0xbf:
        Disassembler_Log("CP A,A\n");
        break;
    case 0xc0:
        Disassembler_Log("RET NZ\n");
        break;
    case 0xc1:
        Disassembler_Log("POP BC\n");
        break;
    case 0xc2:
        Disassembler_Log("JP NZ,$%02x%02x\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0xc3:
        Disassembler_Log("JP $%02x%02x\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0xc4:
        Disassembler_Log("CALL NZ,$%02x%02x\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0xc5:
        Disassembler_Log("PUSH BC\n");
        break;
    case 0xc6:
        Disassembler_Log("ADD A,$%02x\n", cgb->cpu.operand1);
        break;
    case 0xc7:
        Disassembler_Log("RST $00\n");
        break;
    case 0xc8:
        Disassembler_Log("RET Z\n");
        break;
    case 0xc9:
        Disassembler_Log("RET\n");
        break;
    case 0xca: 
        Disassembler_Log("JP Z,$%02x%02x\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0xcb:
        disassembler_cb_write(file, cgb);
        return;
    case 0xcc:
        Disassembler_Log("CALL Z,$%02x%02x\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0xcd:
        Disassembler_Log("CALL $%02x%02x\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0xce:
        Disassembler_Log("ADC A,$%02x\n", cgb->cpu.operand1);
        break;
    case 0xcf:
        Disassembler_Log("RST $08\n");
        break;
    case 0xd0:
        Disassembler_Log("RET NC\n");
        break;
    case 0xd1:
        Disassembler_Log("POP DE\n");
        break;
    case 0xd2:
        Disassembler_Log("JP NC,$%02x%02x\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0xd3:
        break;
    case 0xd4:
        Disassembler_Log("CALL NC,$%02x%02x\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0xd5:
        Disassembler_Log("PUSH DE\n");
        break;
    case 0xd6:
        Disassembler_Log("SUB A,$%02x\n", cgb->cpu.operand1);
        break;
    case 0xd7:
        Disassembler_Log("RST $10\n");
        break;
    case 0xd8:
        Disassembler_Log("RET C\n");
        break;
    case 0xd9:
        Disassembler_Log("RETI\n");
        break;
    case 0xda: 
        Disassembler_Log("JP C,$%02x%02x\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0xdb:
        break;
    case 0xdc:
        Disassembler_Log("CALL C,$%02x%02x\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0xdd:
        break;
    case 0xde:
        Disassembler_Log("SBC A,$%02x\n", cgb->cpu.operand1);
        break;
    case 0xdf:
        Disassembler_Log("RST $18\n");
        break;
    case 0xe0:
        Disassembler_Log("LD ($ff00 + $%02x),A\n", cgb->cpu.operand1);
        break;
    case 0xe1:
        Disassembler_Log("POP HL\n");
        break;
    case 0xe2:
        Disassembler_Log("LD ($ff00 + C),A\n");
        break;
    case 0xe3:
        break;
    case 0xe4:
        break;
    case 0xe5:
        Disassembler_Log("PUSH HL\n");
        break;
    case 0xe6:
        Disassembler_Log("AND A,$%02x\n", cgb->cpu.operand1);
        break;
    case 0xe7:
        Disassembler_Log("RST $20\n");
        break;
    case 0xe8:
        Disassembler_Log("ADD SP,$%02x\n", cgb->cpu.operand1);
        break;
    case 0xe9:
        Disassembler_Log("JP HL\n");
        break;
    case 0xea: 
        Disassembler_Log("LD ($%02x%02x),A\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0xeb:
        break;
    case 0xec:
        break;
    case 0xed:
        break;
    case 0xee:
        Disassembler_Log("XOR A,$%02x\n", cgb->cpu.operand1);
        break;
    case 0xef:
        Disassembler_Log("RST $28\n");
        break;
    case 0xf0:
        Disassembler_Log("LD A,($ff00 + $%02x)\n", cgb->cpu.operand1);
        break;
    case 0xf1:
        Disassembler_Log("POP AF\n");
        break;
    case 0xf2:
        Disassembler_Log("LD A,($ff00 + C)\n");
        break;
    case 0xf3:
        Disassembler_Log("DI\n");
        break;
    case 0xf4:
        break;
    case 0xf5:
        Disassembler_Log("PUSH AF\n");
        break;
    case 0xf6:
        Disassembler_Log("OR A,$%02x\n", cgb->cpu.operand1);
        break;
    case 0xf7:
        Disassembler_Log("RST $30\n");
        break;
    case 0xf8:
        Disassembler_Log("LD HL,SP+$%02x\n", cgb->cpu.operand1);
        break;
    case 0xf9:
        Disassembler_Log("LD SP,HL\n");
        break;
    case 0xfa: 
        Disassembler_Log("LD A,($%02x%02x)\n", cgb->cpu.operand2, cgb->cpu.operand1);
        break;
    case 0xfb:
        Disassembler_Log("EI\n");
        break;
    case 0xfc:
        break;
    case 0xfd:
        break;
    case 0xfe:
        Disassembler_Log("CP A,$%02x\n", cgb->cpu.operand1);
        break;
    case 0xff:
        Disassembler_Log("RST $38\n");
        break;
    }
    fclose(file);
}