#ifndef __LPC1769_H__
#define __LPC1769_H__

#include <stdint.h>

#define FLASHCFG                       (*((volatile uint32_t*)0x400FC000))

#define STIR                           (*((volatile uint32_t*)0xE000EF00))     // Software Trigger Interrupt Register

#define AIRCR                          (*((volatile uint32_t*)0xE000ED0C))     // Application Interrupt and Reset Control Register
#define ICSR                           (*((volatile uint32_t*)0xE000ED04))     // Interrupt Control and State Register

#define SHPR3                          (*((volatile uint32_t*)0xE000ED20))

#define STCTRL                         (*((volatile uint32_t*)0xE000E010))
#define STRELOAD                       (*((volatile uint32_t*)0xE000E014))
#define STCURR                         (*((volatile uint32_t*)0xE000E018))
#define STCALIB                        (*((volatile uint32_t*)0xE000E01C))

#define RICOMPVAL                      (*((volatile uint32_t*)0x400B0000))
#define RIMASK                         (*((volatile uint32_t*)0x400B0004))
#define RICTRL                         (*((volatile uint32_t*)0x400B0008))
#define RICOUNTER                      (*((volatile uint32_t*)0x400B000C))

#define PCONP                          (*((volatile uint32_t*)0x400FC0C4))
#define PCLKSEL0                       (*((volatile uint32_t*)0x400FC1A8))
#define PCLKSEL1                       (*((volatile uint32_t*)0x400FC1AC))

#define SCS                            (*((volatile uint32_t*)0x400FC1A0))
#define CCLKCFG                        (*((volatile uint32_t*)0x400FC104))
#define USBCLKCFG                      (*((volatile uint32_t*)0x400FC108))
#define CLKSRCSEL                      (*((volatile uint32_t*)0x400FC10C))
#define PLL0CON                        (*((volatile uint32_t*)0x400FC080))
#define PLL0CFG                        (*((volatile uint32_t*)0x400FC084))
#define PLL0STAT                       (*((volatile uint32_t*)0x400FC088))
#define PLL0FEED                       (*((volatile uint32_t*)0x400FC08C))

#define PINSEL0                        (*((volatile uint32_t*)0x4002C000))
#define PINMODE0                       (*((volatile uint32_t*)0x4002C040))
#define PINSEL1                        (*((volatile uint32_t*)0x4002C004))
#define PINMODE1                       (*((volatile uint32_t*)0x4002C044))
#define PINSEL3                        (*((volatile uint32_t*)0x4002C00C))
#define PINMODE3                       (*((volatile uint32_t*)0x4002C04C))
#define PINSEL4                        (*((volatile uint32_t*)0x4002C010))
#define PINMODE4                       (*((volatile uint32_t*)0x4002C050))

#define PINMODE_OD0                    (*((volatile uint32_t*)0x4002C068))
#define PINMODE_OD1                    (*((volatile uint32_t*)0x4002C06C))

#define PINSEL7                        (*((volatile uint32_t*)0x4002C024))
#define PINMODE7                       (*((volatile uint32_t*)0x4002C05C))

#define PWM1IR                         (*((volatile uint32_t*)0x40018000))
#define PWM1TCR                        (*((volatile uint32_t*)0x40018004))
#define PWM1TC                         (*((volatile uint32_t*)0x40018008))
#define PWM1PR                         (*((volatile uint32_t*)0x4001800C))
#define PWM1PC                         (*((volatile uint32_t*)0x40018010))
#define PWM1MCR                        (*((volatile uint32_t*)0x40018014))
#define PWM1MR0                        (*((volatile uint32_t*)0x40018018))
#define PWM1MR1                        (*((volatile uint32_t*)0x4001801C))
#define PWM1MR2                        (*((volatile uint32_t*)0x40018020))
#define PWM1PCR                        (*((volatile uint32_t*)0x4001804C))
#define PWM1LER                        (*((volatile uint32_t*)0x40018050))
#define PWM1CTCR                       (*((volatile uint32_t*)0x40018070))

#define STCTRL                         (*((volatile uint32_t*)0xE000E010))
#define STRELOAD                       (*((volatile uint32_t*)0xE000E014))

#define T0IR                           (*((volatile uint32_t*)0x40004000))
#define T1IR                           (*((volatile uint32_t*)0x40008000))
#define T2IR                           (*((volatile uint32_t*)0x40090000))
#define T3IR                           (*((volatile uint32_t*)0x40094000))
#define T0TCR                          (*((volatile uint32_t*)0x40004004))
#define T1TCR                          (*((volatile uint32_t*)0x40008004))
#define T2TCR                          (*((volatile uint32_t*)0x40090004))
#define T3TCR                          (*((volatile uint32_t*)0x40094004))
#define T0TC                           (*((volatile uint32_t*)0x40004008))
#define T1TC                           (*((volatile uint32_t*)0x40008008))
#define T2TC                           (*((volatile uint32_t*)0x40090008))
#define T3TC                           (*((volatile uint32_t*)0x40094008))
#define T0PR                           (*((volatile uint32_t*)0x4000400C))
#define T1PR                           (*((volatile uint32_t*)0x4000800C))
#define T2PR                           (*((volatile uint32_t*)0x4009000C))
#define T3PR                           (*((volatile uint32_t*)0x4009400C))
#define T0MCR                          (*((volatile uint32_t*)0x40004014))
#define T1MCR                          (*((volatile uint32_t*)0x40008014))
#define T2MCR                          (*((volatile uint32_t*)0x40090014))
#define T3MCR                          (*((volatile uint32_t*)0x40094014))
#define T0MR0                          (*((volatile uint32_t*)0x40004018))
#define T1MR0                          (*((volatile uint32_t*)0x40008018))
#define T2MR0                          (*((volatile uint32_t*)0x40090018))
#define T3MR0                          (*((volatile uint32_t*)0x40094018))
#define T0MR1                          (*((volatile uint32_t*)0x4000401C))
#define T1MR1                          (*((volatile uint32_t*)0x4000801C))
#define T2MR1                          (*((volatile uint32_t*)0x4009001C))
#define T3MR1                          (*((volatile uint32_t*)0x4009401C))
#define T0CTCR                         (*((volatile uint32_t*)0x40004070))
#define T1CTCR                         (*((volatile uint32_t*)0x40008070))
#define T2CTCR                         (*((volatile uint32_t*)0x40090070))
#define T3CTCR                         (*((volatile uint32_t*)0x40094070))

#define FIO0DIR                        (*((volatile uint32_t*)0x2009C000))
#define FIO1DIR                        (*((volatile uint32_t*)0x2009C020))
#define FIO2DIR                        (*((volatile uint32_t*)0x2009C040))
#define FIO0MASK                       (*((volatile uint32_t*)0x2009C010))
#define FIO1MASK                       (*((volatile uint32_t*)0x2009C030))
#define FIO2MASK                       (*((volatile uint32_t*)0x2009C050))
#define FIO0PIN                        (*((volatile uint32_t*)0x2009C014))
#define FIO1PIN                        (*((volatile uint32_t*)0x2009C034))
#define FIO2PIN                        (*((volatile uint32_t*)0x2009C054))
#define FIO3PIN                        (*((volatile uint32_t*)0x2009C074))
#define FIOSET                         (*((volatile uint32_t*)0x2009C018))
#define FIO1SET                        (*((volatile uint32_t*)0x2009C038))
#define FIO2SET                        (*((volatile uint32_t*)0x2009C058))
#define FIOCLR                         (*((volatile uint32_t*)0x2009C01C))
#define FIO1CLR                        (*((volatile uint32_t*)0x2009C03C))
#define FIO2CLR                        (*((volatile uint32_t*)0x2009C05C))

#define FIO0CLR                        (*((volatile uint32_t*)0x2009C01C))
#define FIO0SET                        (*((volatile uint32_t*)0x2009C018))

#define FIO3DIR                        (*((volatile uint32_t*)0x2009C060))
#define FIO3MASK                       (*((volatile uint32_t*)0x2009C070))

#define FIO3CLR                        (*((volatile uint32_t*)0x2009C07C))
#define FIO3SET                        (*((volatile uint32_t*)0x2009C078))

#define IO0IntEnR                      (*((volatile uint32_t*)0x40028090))
#define IO0IntEnF                      (*((volatile uint32_t*)0x40028094))
#define IO0IntClr                      (*((volatile uint32_t*)0x4002808C))
#define IO0IntStatR                    (*((volatile uint32_t*)0x40028084))
#define IO0IntStatF                    (*((volatile uint32_t*)0x40028088))

#define IO2IntEnR                      (*((volatile uint32_t*)0x400280B0))
#define IO2IntEnF                      (*((volatile uint32_t*)0x400280B4))
#define IO2IntClr                      (*((volatile uint32_t*)0x400280AC))
#define IO2IntStatR                    (*((volatile uint32_t*)0x400280A4))
#define IO2IntStatF                    (*((volatile uint32_t*)0x400280A8))

#define AD0CR                          (*((volatile uint32_t*)0x40034000))
#define AD0INTEN                       (*((volatile uint32_t*)0x4003400C))
#define AD0GDR                         (*((volatile uint32_t*)0x40034004))
#define AD0DR0                         (*((volatile uint32_t*)0x40034010))
#define AD0DR1                         (*((volatile uint32_t*)0x40034014))
#define AD0DR2                         (*((volatile uint32_t*)0x40034018))
#define AD0DR3                         (*((volatile uint32_t*)0x4003401C))
#define AD0DR4                         (*((volatile uint32_t*)0x40034020))
#define AD0DR5                         (*((volatile uint32_t*)0x40034024))
#define AD0DR6                         (*((volatile uint32_t*)0x40034028))
#define AD0DR7                         (*((volatile uint32_t*)0x4003403C))
#define AD0STAT                        (*((volatile uint32_t*)0x40034030))

#define ISER0                          (*((volatile uint32_t*)0xE000E100))
#define ICER0                          (*((volatile uint32_t*)0xE000E180))
#define IPR0                           (*((volatile uint32_t*)0xE000E400))
#define IPR1                           (*((volatile uint32_t*)0xE000E404))
#define IPR2                           (*((volatile uint32_t*)0xE000E408))
#define IPR3                           (*((volatile uint32_t*)0xE000E40C))
#define IPR4                           (*((volatile uint32_t*)0xE000E410))
#define IPR5                           (*((volatile uint32_t*)0xE000E414))
#define IPR6                           (*((volatile uint32_t*)0xE000E418))
#define IPR7                           (*((volatile uint32_t*)0xE000E41C))

#define ICPR0                          (*((volatile uint32_t*)0xE000E280))

#define DACR                           (*((volatile uint32_t*)0x4008C000))
#define DACCTRL                        (*((volatile uint32_t*)0x4008C004))
#define DACCNTVAL                      (*((volatile uint32_t*)0x4008C008))

#define U0LCR                          (*((volatile uint32_t*)0x4000C00C))
#define U0DLL                          (*((volatile uint32_t*)0x4000C000))
#define U0DLM                          (*((volatile uint32_t*)0x4000C004))
#define U0FDR                          (*((volatile uint32_t*)0x4000C028))
#define U0FCR                          (*((volatile uint32_t*)0x4000C008))
#define U0IER                          (*((volatile uint32_t*)0x4000C004))
#define U0RBR                          (*((volatile uint32_t*)0x4000C000))
#define U0THR                          (*((volatile uint32_t*)0x4000C000))
#define U0TER                          (*((volatile uint32_t*)0x4000C030))
#define U0LSR                          (*((volatile uint32_t*)0x4000C014))
#define U0IIR                          (*((volatile uint32_t*)0x4000C008))

#define ILR                            (*((volatile uint32_t*)0x40024000))
#define CCR                            (*((volatile uint32_t*)0x40024008))
#define CIIR                           (*((volatile uint32_t*)0x4002400C))
#define AMR                            (*((volatile uint32_t*)0x40024010))
#define CTIME0                         (*((volatile uint32_t*)0x40024014))
#define CTIME1                         (*((volatile uint32_t*)0x40024018))
#define CTIME2                         (*((volatile uint32_t*)0x4002401C))
#define SEC                            (*((volatile uint32_t*)0x40024020))
#define MIN                            (*((volatile uint32_t*)0x40024024))
#define HOUR                           (*((volatile uint32_t*)0x40024028))
#define DOM                            (*((volatile uint32_t*)0x4002402C))
#define DOW                            (*((volatile uint32_t*)0x40024030))
#define DOY                            (*((volatile uint32_t*)0x40024034))
#define MONTH                          (*((volatile uint32_t*)0x40024038))
#define YEAR                           (*((volatile uint32_t*)0x4002403C))
#define ALSEC                          (*((volatile uint32_t*)0x40024060))
#define ALMIN                          (*((volatile uint32_t*)0x40024064))
#define ALHOUR                         (*((volatile uint32_t*)0x40024068))
#define ALDOM                          (*((volatile uint32_t*)0x4002406C))
#define ALDOW                          (*((volatile uint32_t*)0x40024070))
#define ALDOY                          (*((volatile uint32_t*)0x40024074))
#define ALMON                          (*((volatile uint32_t*)0x40024078))
#define ALYEAR                         (*((volatile uint32_t*)0x4002407C))

#define GPREG0                         (*((volatile uint32_t*)0x40024044))
#define GPREG1                         (*((volatile uint32_t*)0x40024048))
#define GPREG2                         (*((volatile uint32_t*)0x4002404C))
#define GPREG3                         (*((volatile uint32_t*)0x40024050))
#define GPREG4                         (*((volatile uint32_t*)0x40024054))

#define SSP0CR0                        (*((volatile uint32_t*)0x40088000))
#define SSP0CR1                        (*((volatile uint32_t*)0x40088004))
#define SSP0DR                         (*((volatile uint32_t*)0x40088008))
#define SSP0SR                         (*((volatile uint32_t*)0x4008800C))
#define SSP0CPSR                       (*((volatile uint32_t*)0x40088010))
#define SSP0IMSC                       (*((volatile uint32_t*)0x40088014))
#define SSP0RIS                        (*((volatile uint32_t*)0x40088018))
#define SSP0MIS                        (*((volatile uint32_t*)0x4008801C))
#define SSP0ICR                        (*((volatile uint32_t*)0x40088020))

#define I2CPADCFG                      (*((volatile uint32_t*)0x4002C07C))
#define I2C0CONSET                     (*((volatile uint32_t*)0x4001C000))
#define I2C1CONSET                     (*((volatile uint32_t*)0x4005C000))
#define I2C2CONSET                     (*((volatile uint32_t*)0x400A0000))
#define I2C0CONCLR                     (*((volatile uint32_t*)0x4001C018))
#define I2C1CONCLR                     (*((volatile uint32_t*)0x4005C018))
#define I2C2CONCLR                     (*((volatile uint32_t*)0x400A0018))
#define I2C0STAT                       (*((volatile uint32_t*)0x4001C004))
#define I2C1STAT                       (*((volatile uint32_t*)0x4005C004))
#define I2C2STAT                       (*((volatile uint32_t*)0x400A0004))
#define I2C0DAT                        (*((volatile uint32_t*)0x4001C008))
#define I2C1DAT                        (*((volatile uint32_t*)0x4005C008))
#define I2C2DAT                        (*((volatile uint32_t*)0x400A0008))
#define I2C0SCLH                       (*((volatile uint32_t*)0x4001C010))
#define I2C1SCLH                       (*((volatile uint32_t*)0x4005C010))
#define I2C2SCLH                       (*((volatile uint32_t*)0x400A0010))
#define I2C0SCLL                       (*((volatile uint32_t*)0x4001C014))
#define I2C1SCLL                       (*((volatile uint32_t*)0x4005C014))
#define I2C2SCLL                       (*((volatile uint32_t*)0x400A0014))

#define DMACIntStat                    (*((volatile uint32_t*)0x50004000)) //DMA Interrupt Status Register
#define DMACIntTCStat                  (*((volatile uint32_t*)0x50004004)) //DMA Interrupt Terminal Count Request Status Register
#define DMACIntTCClear                 (*((volatile uint32_t*)0x50004008))
#define DMACIntErrStat                 (*((volatile uint32_t*)0x5000400C))
#define DMACIntErrClr                  (*((volatile uint32_t*)0x50004010))
#define DMACRawIntTCStat               (*((volatile uint32_t*)0x50004014))
#define DMACRawIntErrStat              (*((volatile uint32_t*)0x50004018))
#define DMACEnbldChns                  (*((volatile uint32_t*)0x5000401C))
#define DMACSoftBReq                   (*((volatile uint32_t*)0x50004020))
#define DMACSoftSReq                   (*((volatile uint32_t*)0x50004024))
#define DMACSoftLBReq                  (*((volatile uint32_t*)0x50004028))
#define DMACSoftLSReq                  (*((volatile uint32_t*)0x5000402C))
#define DMACConfig                     (*((volatile uint32_t*)0x50004030))
#define DMACSync                       (*((volatile uint32_t*)0x50004034))
#define DMAREQSEL                      (*((volatile uint32_t*)0x400FC1C4))

#define DMACC0SrcAddr                  (*((volatile uint32_t*)0x50004100)) //DMA Channel 0 Source Address Register
#define DMACC0DestAddr                 (*((volatile uint32_t*)0x50004104)) //DMA Channel 0 Destination Address Register
#define DMACC0LLI                      (*((volatile uint32_t*)0x50004108)) //DMA Channel 0 Linked List Item Register
#define DMACC0Control                  (*((volatile uint32_t*)0x5000410C)) //DMA Channel 0 Control Register
#define DMACC0Config                   (*((volatile uint32_t*)0x50004110)) //DMA Channel 0 Configuration Register

#define DMACC1SrcAddr                  (*((volatile uint32_t*)0x50004120)) //DMA Channel 1 Source Address Register
#define DMACC1DestAddr                 (*((volatile uint32_t*)0x50004124)) //DMA Channel 1 Destination Address Register
#define DMACC1LLI                      (*((volatile uint32_t*)0x50004128)) //DMA Channel 1 Linked List Item Register
#define DMACC1Control                  (*((volatile uint32_t*)0x5000412C)) //DMA Channel 1 Control Register
#define DMACC1Config                   (*((volatile uint32_t*)0x50004130)) //DMA Channel 1 Configuration Register

#define DMACC2SrcAddr                  (*((volatile uint32_t*)0x50004140)) //DMA Channel 2 Source Address Register
#define DMACC2DestAddr                 (*((volatile uint32_t*)0x50004144)) //DMA Channel 2 Destination Address Register
#define DMACC2LLI                      (*((volatile uint32_t*)0x50004148)) //DMA Channel 2 Linked List Item Register
#define DMACC2Control                  (*((volatile uint32_t*)0x5000414C)) //DMA Channel 2 Control Register
#define DMACC2Config                   (*((volatile uint32_t*)0x50004150)) //DMA Channel 2 Configuration Register

#define DMACC3SrcAddr                  (*((volatile uint32_t*)0x50004160)) //DMA Channel 3 Source Address Register
#define DMACC3DestAddr                 (*((volatile uint32_t*)0x50004164)) //DMA Channel 3 Destination Address Register
#define DMACC3LLI                      (*((volatile uint32_t*)0x50004168)) //DMA Channel 3 Linked List Item Register
#define DMACC3Control                  (*((volatile uint32_t*)0x5000416C)) //DMA Channel 3 Control Register
#define DMACC3Config                   (*((volatile uint32_t*)0x50004170)) //DMA Channel 3 Configuration Register

#define DMACC4SrcAddr                  (*((volatile uint32_t*)0x50004180)) //DMA Channel 4 Source Address Register
#define DMACC4DestAddr                 (*((volatile uint32_t*)0x50004184)) //DMA Channel 4 Destination Address Register
#define DMACC4LLI                      (*((volatile uint32_t*)0x50004188)) //DMA Channel 4 Linked List Item Register
#define DMACC4Control                  (*((volatile uint32_t*)0x5000418C)) //DMA Channel 4 Control Register
#define DMACC4Config                   (*((volatile uint32_t*)0x50004190)) //DMA Channel 4 Configuration Register

#define DMACC5SrcAddr                  (*((volatile uint32_t*)0x500041A0)) //DMA Channel 5 Source Address Register
#define DMACC5DestAddr                 (*((volatile uint32_t*)0x500041A4)) //DMA Channel 5 Destination Address Register
#define DMACC5LLI                      (*((volatile uint32_t*)0x500041A8)) //DMA Channel 5 Linked List Item Register
#define DMACC5Control                  (*((volatile uint32_t*)0x500041AC)) //DMA Channel 5 Control Register
#define DMACC5Config                   (*((volatile uint32_t*)0x500041B0)) //DMA Channel 5 Configuration Register

#define DMACC6SrcAddr                  (*((volatile uint32_t*)0x500041C0)) //DMA Channel 6 Source Address Register
#define DMACC6DestAddr                 (*((volatile uint32_t*)0x500041C4)) //DMA Channel 6 Destination Address Register
#define DMACC6LLI                      (*((volatile uint32_t*)0x500041C8)) //DMA Channel 6 Linked List Item Register
#define DMACC6Control                  (*((volatile uint32_t*)0x500041CC)) //DMA Channel 6 Control Register
#define DMACC6Config                   (*((volatile uint32_t*)0x500041D0)) //DMA Channel 6 Configuration Register

#define DMACC7SrcAddr                  (*((volatile uint32_t*)0x500041E0)) //DMA Channel 7 Source Address Register
#define DMACC7DestAddr                 (*((volatile uint32_t*)0x500041E4)) //DMA Channel 7 Destination Address Register
#define DMACC7LLI                      (*((volatile uint32_t*)0x500041E8)) //DMA Channel 7 Linked List Item Register
#define DMACC7Control                  (*((volatile uint32_t*)0x500041EC)) //DMA Channel 7 Control Register
#define DMACC7Config                   (*((volatile uint32_t*)0x500041F0)) //DMA Channel 7 Configuration Register

#endif
