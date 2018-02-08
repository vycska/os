.syntax unified			@thumb-2 are only supported in unified syntax mode
.cpu cortex-m3
.thumb

.global Reset_Handler

.weak NMI_Handler,HardFault_Handler,MemManage_Handler,BusFault_Handler,UsageFault_Handler,SVC_Handler,DebugMon_Handler,PendSV_Handler,SysTick_Handler,WDT_IRQHandler,TIMER0_IRQHandler,TIMER1_IRQHandler,TIMER2_IRQHandler,TIMER3_IRQHandler,UART0_IRQHandler,UART1_IRQHandler,UART2_IRQHandler,UART3_IRQHandler,PWM1_IRQHandler,I2C0_IRQHandler,I2C1_IRQHandler,I2C2_IRQHandler,SPI_IRQHandler,SSP0_IRQHandler,SSP1_IRQHandler,PLL0_IRQHandler,RTC_IRQHandler,EINT0_IRQHandler,EINT1_IRQHandler,EINT2_IRQHandler,EINT3_IRQHandler,ADC_IRQHandler,BOD_IRQHandler,USB_IRQHandler,CAN_IRQHandler,DMA_IRQHandler,I2S_IRQHandler,ENET_IRQHandler,RIT_IRQHandler,MCPWM_IRQHandler,QEI_IRQHandler,PLL1_IRQHandler,USBActivity_IRQHandler,CANActivity_IRQHandler

.section .intvecs, "ax"
.word _stack_end
.word Reset_Handler
.word NMI_Handler
.word HardFault_Handler		@ irq -13, exception 3, offset 0x0c
.word MemManage_Handler
.word BusFault_Handler
.word UsageFault_Handler
.word 0
.word 0
.word 0
.word 0
.word SVC_Handler
.word DebugMon_Handler
.word 0
.word PendSV_Handler
.word SysTick_Handler
@ vendor specific interrupts
.word WDT_IRQHandler		@ irq 0, exception 16, offset 0x40
.word TIMER0_IRQHandler
.word TIMER1_IRQHandler
.word TIMER2_IRQHandler
.word TIMER3_IRQHandler
.word UART0_IRQHandler
.word UART1_IRQHandler
.word UART2_IRQHandler
.word UART3_IRQHandler
.word PWM1_IRQHandler
.word I2C0_IRQHandler
.word I2C1_IRQHandler
.word I2C2_IRQHandler
.word SPI_IRQHandler
.word SSP0_IRQHandler
.word SSP1_IRQHandler
.word PLL0_IRQHandler
.word RTC_IRQHandler
.word EINT0_IRQHandler
.word EINT1_IRQHandler
.word EINT2_IRQHandler
.word EINT3_IRQHandler
.word ADC_IRQHandler
.word BOD_IRQHandler
.word USB_IRQHandler
.word CAN_IRQHandler
.word DMA_IRQHandler
.word I2S_IRQHandler
.word ENET_IRQHandler
.word RIT_IRQHandler
.word MCPWM_IRQHandler
.word QEI_IRQHandler
.word PLL1_IRQHandler
.word USBActivity_IRQHandler
.word CANActivity_IRQHandler		@ irq 34, exception 50, offset 0xc8

.text
.thumb_func
.type Reset_Handler, %function
Reset_Handler:
bl init
bl main
b .

.thumb_func
NMI_Handler:
.thumb_func
HardFault_Handler:
.thumb_func
MemManage_Handler:
.thumb_func
BusFault_Handler:
.thumb_func
UsageFault_Handler:
.thumb_func
SVC_Handler:
.thumb_func
DebugMon_Handler:
.thumb_func
PendSV_Handler:
.thumb_func
SysTick_Handler:
.thumb_func
WDT_IRQHandler:
.thumb_func
TIMER0_IRQHandler:
.thumb_func
TIMER1_IRQHandler:
.thumb_func
TIMER2_IRQHandler:
.thumb_func
TIMER3_IRQHandler:
.thumb_func
UART0_IRQHandler:
.thumb_func
UART1_IRQHandler:
.thumb_func
UART2_IRQHandler:
.thumb_func
UART3_IRQHandler:
.thumb_func
PWM1_IRQHandler:
.thumb_func
I2C0_IRQHandler:
.thumb_func
I2C1_IRQHandler:
.thumb_func
I2C2_IRQHandler:
.thumb_func
SPI_IRQHandler:
.thumb_func
SSP0_IRQHandler:
.thumb_func
SSP1_IRQHandler:
.thumb_func
PLL0_IRQHandler:
.thumb_func
RTC_IRQHandler:
.thumb_func
EINT0_IRQHandler:
.thumb_func
EINT1_IRQHandler:
.thumb_func
EINT2_IRQHandler:
.thumb_func
EINT3_IRQHandler:
.thumb_func
ADC_IRQHandler:
.thumb_func
BOD_IRQHandler:
.thumb_func
USB_IRQHandler:
.thumb_func
CAN_IRQHandler:
.thumb_func
DMA_IRQHandler:
.thumb_func
I2S_IRQHandler:
.thumb_func
ENET_IRQHandler:
.thumb_func
RIT_IRQHandler:
.thumb_func
MCPWM_IRQHandler:
.thumb_func
QEI_IRQHandler:
.thumb_func
PLL1_IRQHandler:
.thumb_func
USBActivity_IRQHandler:
.thumb_func
CANActivity_IRQHandler:
b .

.end
