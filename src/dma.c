#include "dma.h"
#include "LPC1769.h"

//TC interrupt is generated, if enabled, when the DMA transfer is completed and before DMA controller moves on to the next LLI

struct __attribute__ ((aligned (4))) __attribute__ ((packed)) DMA_LLI {
   unsigned int SrcAddr,DestAddr,LLI,Control;
};

extern volatile unsigned int adc0,adc2;

struct DMA_LLI dma_lli_adc_next1;

void DMA_Init(void) {
   PCONP |= (1<<29); //power

   DMACIntTCClear |= (0xff<<0); //clear all channels terminal count interrupt
   DMACIntErrClr |= (0xff<<0); //clear all channels error interrupt

   //NVIC interrupt: ID 26, exception number 42, vector offset 0xa8, GPDMA
   IPR6 = (IPR6&(~(0x1f<<19))) | (5<<19);
   ISER0 |= (1<<26); //GPDMA interrupt enable

   DMAReqSel |= (0xff<<0); //select Timer DMA requests for DMA inputs 8 through 15
   DMACSync &= (~(0xffff<<0)); //symchronization logic for all request signals is enabled
   DMACConfig = (DMACConfig & (~(0x3<<0))) | (1<<0) | (0<<1); //enable DMA controller, little-endian mode

   //configure channel 0 for ADC (DMA channel registers: SrcAddr, DestAddr, LLI, Control, Config)
   DMACC0SrcAddr = (unsigned int)&AD0DR0; //channel 0 source address [peripheral]
   DMACC0DestAddr = (unsigned int)&adc0; //channel 0 destination address
   DMACC0LLI = (unsigned int)&dma_lli_adc_next1; //address of the next linked list item [bits 1:0 must be 0 (4 kartotinis; word-aligned)]
   DMACC0Control = (DMACC0Control&(~(0xfff<<0))) | (1<<0); //transfer size 1
   DMACC0Control = (DMACC0Control&(~(0x7<<12))) | (0<<12); //source burst size 1 (000)
   DMACC0Control = (DMACC0Control&(~(0x7<<15))) | (0<<15); //dest burst size 1 (000) [dest is memory thus here is memory boundary size]
   DMACC0Control = (DMACC0Control&(~(0x7<<18))) | (2<<18); //source trasfer size 32b
   DMACC0Control = (DMACC0Control&(~(0x7<<21))) | (2<<21); //destination transfer width 32b
   DMACC0Control = (DMACC0Control&(~(0x1<<26))) | (0<<26); //source address is not incremented
   DMACC0Control = (DMACC0Control&(~(0x1<<27))) | (0<<27); //destination address is not incremented
   DMACC0Control = (DMACC0Control&(~(0x1<<31))) | (1<<31); //terminal count interrupt is enabled
   DMACC0Config = (DMACC0Config&(~(0x1f<<1))) | (4<<1); //source peripheral is ADC (request line 4)
   DMACC0Config = (DMACC0Config&(~(0x1f<<6))) | (0<<6); //destination peripheral -- ignored because destination is memory
   DMACC0Config = (DMACC0Config&(~(0x7<<11))) | (2<<11); //tranfer type is peripheral-to-memory
   DMACC0Config = (DMACC0Config&(~(0x1<<14))) | (1<<14); //no masking of the error interrupt
   DMACC0Config = (DMACC0Config&(~(0x1<<15))) | (1<<15); //no masking of the terminal count interrupt
   dma_lli_adc_next1.SrcAddr = (unsigned int)&AD0DR2;
   dma_lli_adc_next1.DestAddr = (unsigned int)&adc2;
   dma_lli_adc_next1.LLI = 0;
   dma_lli_adc_next1.Control = DMACC0Control;
   DMACC0Config |= (1<<0); //enable channel 0 [must be fully initialized before enabling]
}

void DMA_IRQHandler(void) {
   if(DMACIntStat&(1<<0)) { //channel 0 has an active interrupt request
      if(DMACIntTCStat&(1<<0)) { //channel 0 has an active terminal count interrupt request [after masking]
         DMACIntTCClear |= (1<<0); //clear terminal count interrupt
      }
      if(DMACIntErrStat&(1<<0)) { //channel 0 has an active error interrupt request [after masking]
         DMACIntErrClr |= (1<<0); //clear error interrupt
      }
   }
}

//find out which dma channels are enabled: dma_enabled_channels_mask = DMACEnbldChns&0xff;
//init DMA burst request by software (not by hardware -- peripheral or timer): DMACSoftBReq |= (1<<request_line);
//init DMA single transfer request by software: DMACSoftSReq |= (1<<request_line);
//DMA source address: DMACC0SrcAddr
//DMAReqSel -- select between UART and Timer DMA requests for DMA inputs 8 through 15
