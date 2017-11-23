.syntax unified
.cpu cortex-m3
.thumb

.text

.global OS_Start
.global SysTick_Handler

.thumb_func
OS_Start:
ldr r0,=RunPt
ldr r2,[r0]
ldr sp,[r2]
pop {r4-r11}
pop {r0-r3}
pop {r12}
add sp,sp,#4
pop {lr}
add sp,sp,#4
push {r0,lr}
mov r0,#10
bl SysTick_Init
bl RIT_Init
pop {r0,lr}
cpsie i
bx lr

.thumb_func
SysTick_Handler:
cpsid i
push {r4-r11}
ldr r0,=RunPt
ldr r1,[r0]
str sp,[r1]
push {r0,lr}
bl Scheduler
pop {r0,lr}
ldr r1,[r0]
ldr sp,[r1]
pop {r4-r11}
cpsie i
bx lr

.end
