#ifndef __DMA_H__
#define __DMA_H__

struct __attribute__ ((aligned (4))) __attribute__ ((packed)) DMA_LLI {
   unsigned int SrcAddr,DestAddr,LLI,Control;
};
void DMA_Init(void);
void DMA_ADC_Init(void);

#endif
