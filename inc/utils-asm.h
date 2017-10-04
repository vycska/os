#ifndef UTILS_ASM_H
#define UTILS_ASM_H

#include <stdint.h>

void DisableInterrupts(void);
void EnableInterrupts(void);
uint32_t StartCritical(void);
void EndCritical(int);
void WaitForInterrupt(void);
uint32_t GetPSR(void);
void _DSB(void);

#endif
