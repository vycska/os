# make all libs clean install tags print-<variable>

################################################################################

TARGET := os

ASRCS := $(sort $(shell find . -name '*.s' -printf '%f '))
AOBJS := $(addprefix objs/,$(ASRCS:.s=.o))

CSRCS := $(sort $(shell find src -path src/u8g2 -prune -o -name '*.c' -printf '%f '))
CDEPS := $(patsubst %.c,deps/%.d,$(CSRCS))
COBJS := $(addprefix objs/,$(CSRCS:.c=.o))

U8G2SRCS := $(sort $(shell find src/u8g2 -name '*.c' -printf '%f '))
U8G2DEPS := $(patsubst %.c,deps/%.d,$(U8G2SRCS))
U8G2OBJS := $(addprefix objs/,$(U8G2SRCS:.c=.o))

ASFLAGS := -Wa,--warn -Wa,--fatal-warnings
CPPFLAGS := -I inc -I inc/u8g2 -I /usr/arm-none-eabi/include
CFLAGS := -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -mlittle-endian -ffreestanding -fdata-sections -ffunction-sections -Os -Wall -Werror
LDFLAGS := -nostdlib -nostartfiles -Llibs -L/usr/arm-none-eabi/lib/armv7-m -L/usr/lib/gcc/arm-none-eabi/7.2.0/armv7-m -T $(TARGET).ld -Wl,-Map=$(TARGET).map -Wl,-Os -Wl,-gc-sections
LDLIBS := -lgcc -lc_nano -lnosys -lm -lu8g2

vpath %.h inc:inc/u8g2
vpath %.s src
vpath %.c src:src/u8g2

$(shell if [ ! -d deps ]; then mkdir -p deps; fi)
$(shell if [ ! -d objs ]; then mkdir -p objs; fi)
$(shell if [ ! -d libs ]; then mkdir -p libs; fi)

################################################################################

$(TARGET).elf : $(AOBJS) $(COBJS)
	$(info Linking $(TARGET))
	arm-none-eabi-gcc $^ $(LDFLAGS) $(LDLIBS) -o $@
	arm-none-eabi-objdump -D $@ > $(TARGET).lst
	arm-none-eabi-objcopy -O binary $@ $(TARGET).bin
	arm-none-eabi-objcopy -I ihex $@ $(TARGET).hex
	arm-none-eabi-size --format=sysv --common -d $(TARGET).elf

objs/%.o : %.s
	arm-none-eabi-gcc $< -c $(CFLAGS) $(ASFLAGS) -o $@

objs/%.o : %.c deps/%.d
	arm-none-eabi-gcc $< -c $(CPPFLAGS) $(CFLAGS) -o $@
	arm-none-eabi-gcc $< -E $(CPPFLAGS) -MM -MP -MT $@ -MF deps/$*.d && touch $@

deps/%.d : ;

.PRECIOUS : deps/%.d

################################################################################

.PHONY : all libs clean install tags print-%

all : $(TARGET).elf

libs : libs/libu8g2.a

libs/libu8g2.a : $(U8G2OBJS)
	arm-none-eabi-ar -rcsDv --target=elf32-littlearm $@ $^

clean :
	rm -rf *.o *.elf *.bin *.hex *.map *.lst cscope* tags deps objs

install : all
	lpc21isp $(TARGET).hex /dev/ttyUSB0 115200 4000

tags :
	ctags -R --extra=+f *
	find . -name '*.[csh]' > cscope.files
	cscope -q -R -b -i cscope.files

print-% :
	@echo $* = $($*)

################################################################################

-include $(CDEPS)
-include $(U8G2DEPS)

################################################################################

#-specs=nano.specs
#-specs=nosys.specs
#-g
#/usr/lib/gcc/arm-none-eabi/7.2.0/armv7-m/libgcc.a
#/usr/arm-none-eabi/lib/armv7-m/libc_nano.a
#/usr/arm-none-eabi/lib/armv7-m/libnosys.a
