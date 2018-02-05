#include "dma.h"
#include "LPC1769.h"

void DMA_Init(void) {
   PCONP |= (1<<29); //power


   DMACIntTCClear |= (0xff<<0); //clear all channels terminal count interrupt
   DMACIntErrClr |= (0xff<<0); //clear all channels error interrupt

   DMACC0SrcAddr = 0; //channel 0 source address [peripheral]
   DMACC0DestAddr = 0; //channel 0 destination address

   DMACC0LLI = 0; //channel 0 linked list item -- not used

   DMACC0Control = ((1&0xfff)<<0) | ((0&0x7)<<12) | ((0&0x7)<<15) | ((2&0x7)<<18) | ((2&0x7)<<21) | (0<<26) | (0<<27) | (1<<31); //transfer size, source burst size, destination burst size, source transfer width, destination tansfer width, source increment, destination increment, terminal count interrupt enabled

   DMACC0Config = (0<<0) | (4<<1) | (0<<6) | (2<<11) | (1<<14) | (1<<15) | (0<<18); //channel 0 - do not yet enable, source peripheral -- ADC, destination peripheral -- memory, transfer type -- peripheral-to-memory, interrupt error mask - no, terminal count interrupt mask -no, halt -- enable DMA requests

   //NVIC interrupt: ID 26, exception number 42, vector offset 0xa8, GPDMA
   IPR6 = (IPR6&(~(0x1f<<19))) | (5<<19);
   ISER0 |= (1<<26); //GPDMA interrupt enable

   DMACC0Config |= (1<<0); //enable channel 0 [must be fully initialized before enabling]

   DMACConfig = (1<<0) | (0<<1); //enable DMA controller, little-endian mode
}

void DMA_IRQHandler(void) {
   if(DMACIntStat&(1<<0)) { //channel 0 has an active interrupt request
      if(DMACRawIntTCStat&(1<<0)) { //channel has an active terminal count interrupt request [prior masking]

      }
      else { //no active terminal count interrupt request [prior masking]

      }
      if(DMACIntTCStat&(1<<0)) { //channel has an active terminal count interrupt request [after masking]
         DMACIntTCClear |= (1<<0); //clear terminal count interrupt
      }
      else { //no active terminal count interrupt request [after masking]

      }
      // ***************************************************
      if(DMACRawIntErrStat&(1<<0)) { //channel has an active error interrupt request [prior masking]

      }
      else { //no active error interrupt request [prior masking]

      }
      if(DMACIntErrStat&(1<<0)) { //channel has an active error interrupt request [after masking]
         DMACIntErrClr |= (1<<0); //clear error interrupt
      }
      else { //no active error interrupt request [after masking]

      }
   }
}

//find out which dma channels are enabled: dma_enabled_channels_mask = DMACEnbldChns&0xff;
//init DMA burst request by software (not by hardware -- peripheral or timer): DMACSoftBReq |= (1<<request_line);
//init DMA single transfer request by software: DMACSoftSReq |= (1<<request_line);
//enable synchronization logic for particular request signal: DMACSync &= ~(1<<request_line);
//select between UART and MAT for DMA 8 through 15: DMAReqSel
//DMA source address: DMACC0SrcAddr
