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
C_SOURCES := active_buzzer.c adc.c bme280.c ds18b20.c e2prom.c fifos.c fs.c hd44780.c i2c.c iap.c led.c main.c onewire.c os.c passive_buzzer.c pll.c pwm.c rit.c rtc.c switch.c systick.c task_adc.c task_bme280.c task_command_parser.c task_ds18b20.c task_hd44780.c task_idle.c task_log.c task_oled.c task_switch.c task_uart0_output.c timer.c uart.c utils.c u8g2_bitmap.c u8g2_box.c u8g2_buffer.c u8g2_circle.c u8g2_cleardisplay.c u8g2_d_memory.c u8g2_d_setup.c u8g2_font.c u8g2_fonts.c u8g2_hvline.c u8g2_input_value.c u8g2_intersection.c u8g2_kerning.c u8g2_line.c u8g2_ll_hvline.c u8g2_message.c u8g2_polygon.c u8g2_selection_list.c u8g2_setup.c u8x8_8x8.c u8x8_byte.c u8x8_cad.c u8x8_debounce.c u8x8_display.c u8x8_d_ssd1306_128x64_noname.c u8x8_d_stdio.c u8x8_fonts.c u8x8_gpio.c u8x8_input_value.c u8x8_message.c u8x8_selection_list.c u8x8_setup.c u8x8_string.c u8x8_u16toa.c u8x8_u8toa.c
C_OBJECTS := $(C_SOURCES:.c=.o)
#HEADERS = $(C_SOURCES:.c=.h) LPC1769.h pitches.h os_asm.h utils-asm.h

GCCFLAGS = -ffreestanding -Wall -Werror -Os -I./ -I./inc/ -I/usr/arm-none-eabi/include/ -L/usr/arm-none-eabi/lib/armv7-m/ -L/usr/lib/gcc/arm-none-eabi/7.2.0/armv7-m/ -mfloat-abi=soft -mcpu=cortex-m3 -mthumb -fdata-sections -ffunction-sections
#-specs=nano.specs -specs=nosys.specs
#-g

.PHONY : all clean install vim print-%
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

clean :
	rm -f *.o *.elf *.bin *.hex *.map *.lst cscope* tags

install :
	lpc21isp $(TARGET).hex /dev/ttyUSB0 115200 4000

vim :
	ctags -R --extra=+f *
	find . -name "*.[csh]" > cscope.files
	cscope -q -R -b -i cscope.files

print-% :
	@echo $* = $($*)
	$(info TARGET is $(TARGET))
