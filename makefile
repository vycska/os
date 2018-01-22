# make all clean install tags print-<variable>

################################################################################

vpath %s src
vpath %c src:src/u8g2
vpath %h inc:inc/u8g2

TARGET := os

DEPDIR := .deps
OBJDIR := .objs

HDRS := $(sort $(shell find . -name '*.h' -printf '%f '))
ASRCS := $(sort $(shell find . -name '*.s' -printf '%f '))
CSRCS := $(sort $(shell find . -name '*.c' -printf '%f '))
DEPS := $(patsubst %.c,$(DEPDIR)/%.d,$(CSRCS))
AOBJS := $(addprefix $(OBJDIR)/,$(ASRCS:.s=.o))
COBJS := $(addprefix $(OBJDIR)/,$(CSRCS:.c=.o))
OBJS := $(AOBJS) $(COBJS)

ASFLAGS := -Wa,--warn -Wa,--fatal-warnings
CPPFLAGS := -I inc -I inc/u8g2 -I /usr/arm-none-eabi/include
CFLAGS := -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -mlittle-endian -ffreestanding -fdata-sections -ffunction-sections -Os -Wall -Werror
LDFLAGS := -nostdlib -nostartfiles -L/usr/arm-none-eabi/lib/armv7-m -L/usr/lib/gcc/arm-none-eabi/7.2.0/armv7-m -T $(TARGET).ld -Wl,-Map=$(TARGET).map -Wl,-Os -Wl,-gc-sections
LDLIBS := -lgcc -lc_nano -lnosys -lm

################################################################################

$(TARGET).elf : $(OBJS)
	$(info Linking $(TARGET))
	arm-none-eabi-gcc $^ $(LDFLAGS) $(LDLIBS) -o $@
	arm-none-eabi-objdump -D $@ > $(TARGET).lst
	arm-none-eabi-objcopy -O binary $@ $(TARGET).bin
	arm-none-eabi-objcopy -I ihex $@ $(TARGET).hex

$(OBJDIR)/%.o : %.s | objdir
	arm-none-eabi-gcc $< -c $(CFLAGS) $(ASFLAGS) -o $@

$(COBJS) : | objdir
	arm-none-eabi-gcc $< -c $(CPPFLAGS) $(CFLAGS) -o $@

$(DEPDIR)/%.d : %.c $(HDRS) | depdir
	arm-none-eabi-gcc $< -E -MM -MT $(OBJDIR)/$(patsubst %.c,%.o,$(notdir $<)) -MF $@ $(CPPFLAGS)

################################################################################

.PHONY : all clean install tags depdir objdir print-%

all : $(TARGET).elf

clean :
	rm -rf *.o *.elf *.bin *.hex *.map *.lst cscope* tags $(DEPDIR) $(OBJDIR)

install :
	lpc21isp $(TARGET).hex /dev/ttyUSB0 115200 4000

tags :
	ctags -R --extra=+f *
	find . -name "*.[csh]" > cscope.files
	cscope -q -R -b -i cscope.files

depdir :
	@if [ ! -d "$(DEPDIR)" ]; then mkdir -p "$(DEPDIR)"; fi

objdir:
	@if [ ! -d "$(OBJDIR)" ]; then mkdir -p "$(OBJDIR)"; fi

print-% :
	@echo $* = $($*)

################################################################################

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif

################################################################################

#-specs=nano.specs
#-specs=nosys.specs
#-g
#/usr/lib/gcc/arm-none-eabi/7.2.0/armv7-m/libgcc.a
#/usr/arm-none-eabi/lib/armv7-m/libc_nano.a
#/usr/arm-none-eabi/lib/armv7-m/libnosys.a
