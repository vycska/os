# make all
# make clean
# make install
# make ctags
# make cscope
# make print-<variable>

#todo: automatic source's header file dependencies

TARGET := os
A_SOURCES := os-asm.s startup.s utils-asm.s
A_OBJECTS := $(A_SOURCES:.s=.o)
C_SOURCES := active_buzzer.c adc.c bme280.c ds18b20.c e2prom.c fifos.c fs.c hd44780.c i2c.c iap.c led.c main.c onewire.c os.c passive_buzzer.c pll.c pwm.c rit.c rtc.c switch.c systick.c task_adc.c task_bme280.c task_command_parser.c task_ds18b20.c task_hd44780.c task_idle.c task_log.c task_switch.c task_uart0_output.c timer.c uart.c utils.c
C_OBJECTS := $(C_SOURCES:.c=.o)
HEADERS = $(C_SOURCES:.c=.h) LPC1769.h pitches.h os_asm.h utils-asm.h

GCCFLAGS = -ffreestanding -Wall -Werror -Os -I./ -I./inc/ -I/usr/arm-none-eabi/include/ -L/usr/arm-none-eabi/lib/armv7-m/ -L/usr/lib/gcc/arm-none-eabi/7.2.0/armv7-m/ -mfloat-abi=soft -mcpu=cortex-m3 -mthumb -fdata-sections -ffunction-sections
#-specs=nano.specs -specs=nosys.specs
#-g

.PHONY : all
all : $(TARGET).elf

%.o : %.s
	arm-none-eabi-gcc -c -o $@ $(GCCFLAGS) -Wa,--warn -Wa,--fatal-warnings $^

%.o : %.c
	arm-none-eabi-gcc -c -o $@ $(GCCFLAGS) $<

$(TARGET).elf : $(A_OBJECTS) $(C_OBJECTS) $(TARGET).ld
	arm-none-eabi-gcc -o $(TARGET).elf $(GCCFLAGS) -nostdlib -nostartfiles -T $(TARGET).ld -Wl,-Map=$(TARGET).map -Wl,-Os $(A_OBJECTS) $(C_OBJECTS) -lm -lgcc -lc_nano -lnosys -Wl,-gc-sections
	#/usr/arm-none-eabi/lib/armv7-m/libc_nano.a /usr/lib/gcc/arm-none-eabi/7.2.0/armv7-m/libgcc.a /usr/arm-none-eabi/lib/armv7-m/libnosys.a
	arm-none-eabi-objdump -D $(TARGET).elf > $(TARGET).lst
	arm-none-eabi-objcopy -O binary $(TARGET).elf $(TARGET).bin
	arm-none-eabi-objcopy -I ihex $(TARGET).elf $(TARGET).hex

.PHONY : clean
clean :
	rm -f *.o *.elf *.bin *.hex *.map *.lst cscope* tags

.PHONY : install
install :
	lpc21isp $(TARGET).hex /dev/ttyUSB0 115200 4000

.PHONY : ctags
ctags :
	ctags -R *

.PHONY : cscope
cscope :
	find . -name "*.[csh]" > cscope.files
	cscope -q -R -b -i cscope.files

.PHONY : print-%
print-% :
	@echo $* = $($*)
	$(info TARGET is $(TARGET))
