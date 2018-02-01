#include "dma.h"

void DMA_Init(void) {
   PCONP |= (1<<29); //power




   //NVIC interrupt: ID 26, exception number 42, vector offset 0xa8, GPDMA
   IPR6 = (IPR6&(~(0x1f<<19))) | (5<<19);
   ISER0 |= (1<<26); //GPDMA interrupt enable
}
