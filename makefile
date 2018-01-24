# make all libs clean install tags print-<variable>

################################################################################

vpath %s src
vpath %c src:src/u8g2
vpath %h inc:inc/u8g2

TARGET := os

SRCDIR := src
INCDIR := inc
DEPDIR := deps
OBJDIR := objs
LIBDIR := libs

HDRS := $(sort $(shell find . -name '*.h' -printf '%f '))

ASRCS := $(sort $(shell find . -name '*.s' -printf '%f '))
AOBJS := $(addprefix $(OBJDIR)/,$(ASRCS:.s=.o))

CSRCS := $(sort $(shell find . -path ./$(SRCDIR)/u8g2 -prune -o -name '*.c' -printf '%f '))
CDEPS := $(patsubst %.c,$(DEPDIR)/%.d,$(CSRCS))
COBJS := $(addprefix $(OBJDIR)/,$(CSRCS:.c=.o))

U8G2SRCS := $(sort $(shell find $(SRCDIR)/u8g2 -name '*.c' -printf '%f '))
U8G2DEPS := $(patsubst %.c,$(DEPDIR)/%.d,$(U8G2SRCS))
U8G2OBJS := $(addprefix $(OBJDIR)/,$(U8G2SRCS:.c=.o))

ASFLAGS := -Wa,--warn -Wa,--fatal-warnings
CPPFLAGS := -I inc -I inc/u8g2 -I /usr/arm-none-eabi/include
CFLAGS := -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -mlittle-endian -ffreestanding -fdata-sections -ffunction-sections -Os -Wall -Werror
LDFLAGS := -nostdlib -nostartfiles -L $(LIBDIR) -L/usr/arm-none-eabi/lib/armv7-m -L/usr/lib/gcc/arm-none-eabi/7.2.0/armv7-m -T $(TARGET).ld -Wl,-Map=$(TARGET).map -Wl,-Os -Wl,-gc-sections
LDLIBS := -lgcc -lc_nano -lnosys -lm -lu8g2

################################################################################

$(TARGET).elf : $(AOBJS) $(COBJS)
	$(info Linking $(TARGET))
	arm-none-eabi-gcc $^ $(LDFLAGS) $(LDLIBS) -o $@
	arm-none-eabi-objdump -D $@ > $(TARGET).lst
	arm-none-eabi-objcopy -O binary $@ $(TARGET).bin
	arm-none-eabi-objcopy -I ihex $@ $(TARGET).hex

$(OBJDIR)/%.o : %.s | objdir
	arm-none-eabi-gcc $< -c $(CFLAGS) $(ASFLAGS) -o $@

$(OBJDIR)/%.o : | objdir
	arm-none-eabi-gcc $< -c $(CPPFLAGS) $(CFLAGS) -o $@

$(DEPDIR)/%.d : %.c $(HDRS) | depdir
	arm-none-eabi-gcc $< -E $(CPPFLAGS) -MM -MT $(OBJDIR)/$(patsubst %.c,%.o,$(notdir $<)) -MF $@

################################################################################

.PHONY : all libs clean install tags depdir objdir libdir print-%

all : $(TARGET).elf

libs : $(LIBDIR)/libu8g2.a

$(LIBDIR)/libu8g2.a : $(U8G2OBJS) | libdir
	arm-none-eabi-ar -rcsDv --target=elf32-littlearm $@ $^

clean :
	rm -rf *.o *.elf *.bin *.hex *.map *.lst cscope* tags $(DEPDIR) $(OBJDIR)

install : all
	lpc21isp $(TARGET).hex /dev/ttyUSB0 115200 4000

tags :
	ctags -R --extra=+f *
	find . -name '*.[csh]' > cscope.files
	cscope -q -R -b -i cscope.files

depdir :
	@if [ ! -d '$(DEPDIR)' ]; then mkdir -p '$(DEPDIR)'; fi

objdir :
	@if [ ! -d '$(OBJDIR)' ]; then mkdir -p '$(OBJDIR)'; fi

libdir :
	@if [ ! -d '$(LIBDIR)' ]; then mkdir -p '$(LIBDIR)'; fi

print-% :
	@echo $* = $($*)

################################################################################

ifeq (,$(filter $(MAKECMDGOALS),clean tags libs $(LIBDIR)/libu8g2.a))
   include $(CDEPS)
endif

ifeq ($(MAKECMDGOALS),$(filter $(MAKECMDGOALS),libs $(LIBDIR)/libu8g2.a))
   include $(U8G2DEPS)
endif

################################################################################

#-specs=nano.specs
#-specs=nosys.specs
#-g
#/usr/lib/gcc/arm-none-eabi/7.2.0/armv7-m/libgcc.a
#/usr/arm-none-eabi/lib/armv7-m/libc_nano.a
#/usr/arm-none-eabi/lib/armv7-m/libnosys.a
