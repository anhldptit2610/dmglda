#include "dma.h"

static void oam_dma_transfer(gb_t *gb)
{
    static int i, state;
    
    switch (state) {
    case 0:
        gb->dma.current_transfer_byte = mmu_read(gb, gb->dma.start_addr + i);
        state++;
        break;
    case 1:
        state++;
        break;
    case 2:
        state++;
        break;
    case 3:
        gb->mem[OAM_START_ADDR + i] = gb->dma.current_transfer_byte;
        state = 0;
        i = (i == 160) ? 0 : i + 1;
        break;
    default:
        break;
    }

}

void dma_tick(gb_t *gb)
{
    if (gb->dma.mode != DMA_MODE_OFF) {
        gb->dma.tick++;
        switch (gb->dma.mode) {
        case DMA_MODE_SETUP:
            if (gb->dma.tick >= 4) {
                gb->dma.mode = DMA_MODE_TRANSFER;
                gb->dma.start_addr = ((uint16_t)gb->dma.reg & 0xdf) << 8;
            }
            break;
        case DMA_MODE_TRANSFER:
            oam_dma_transfer(gb);
            if (gb->dma.tick == 644) {
                gb->dma.mode = DMA_MODE_OFF; 
                gb->dma.tick = 0;
            }
        default:
            break;
        }
    }
}