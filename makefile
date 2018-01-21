# make all
# make clean
# make install
# make ctags
# make cscope
# make print-<variable>
#/usr/arm-none-eabi/lib/armv7-m/libc_nano.a /usr/lib/gcc/arm-none-eabi/7.2.0/armv7-m/libgcc.a /usr/arm-none-eabi/lib/armv7-m/libnosys.a
#-specs=nano.specs -specs=nosys.specs -g

################################################################################
################################################################################
################################################################################
################################################################################
################################################################################
################################################################################
################################################################################

TARGET := os

DEPDIR := .deps
OBJDIR := .objs

vpath %s src
vpath %c src:src/u8g2
vpath %h inc:inc/u8g2

HEADERS := $(sort $(shell find . -name '*.h' -printf '%f '))
A_SOURCES := $(sort $(shell find . -name '*.s' -printf '%f '))
C_SOURCES := $(sort $(shell find . -name '*.c' -printf '%f '))
DEPS := $(patsubst %.c,$(DEPDIR)/%.d,$(C_SOURCES))
A_OBJECTS := $(addprefix $(OBJDIR)/,$(A_SOURCES:.s=.o))
C_OBJECTS := $(addprefix $(OBJDIR)/,$(C_SOURCES:.c=.o))
OBJS := $(A_OBJECTS) $(C_OBJECTS)

GCCFLAGS = -ffreestanding \
	   -Wall \
	   -Werror \
	   -Os \
	   -I./inc \
	   -I./inc/u8g2 \
	   -I/usr/arm-none-eabi/include \
	   -L/usr/arm-none-eabi/lib/armv7-m \
	   -L/usr/lib/gcc/arm-none-eabi/7.2.0/armv7-m \
	   -mfloat-abi=soft \
	   -mcpu=cortex-m3 \
	   -mthumb \
	   -fdata-sections \
	   -ffunction-sections

.PHONY : all clean install vim print-%

all : $(TARGET).elf

$(TARGET).elf : $(OBJS)
	arm-none-eabi-gcc $(GCCFLAGS) -nostdlib -nostartfiles -Wl,-Map=$(TARGET).map -Wl,-Os -Wl,-gc-sections -o $@ $^ -lgcc -lc_nano -lnosys -lm -T $(TARGET).ld 
	arm-none-eabi-objdump -D $@ > $(TARGET).lst
	arm-none-eabi-objcopy -O binary $@ $(TARGET).bin
	arm-none-eabi-objcopy -I ihex $@ $(TARGET).hex

$(OBJDIR)/%.o : %.s
	@if [ ! -d "$(OBJDIR)" ]; then mkdir -p "$(OBJDIR)"; fi
	arm-none-eabi-gcc -c $(GCCFLAGS) -Wa,--warn -Wa,--fatal-warnings -o $@ $^

$(C_OBJECTS) :
	@if [ ! -d "$(OBJDIR)" ]; then mkdir -p "$(OBJDIR)"; fi
	arm-none-eabi-gcc -c $(GCCFLAGS) -o $@ $<

$(DEPDIR)/%.d : %.c $(HEADERS)
	@if [ ! -d "$(DEPDIR)" ]; then mkdir -p "$(DEPDIR)"; fi
	arm-none-eabi-gcc -I./inc -I./inc/u8g2 -E -MM -MT $(OBJDIR)/$(patsubst %.c,%.o,$(notdir $<)) -MF $@ $<

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif

clean :
	rm -rf *.o *.elf *.bin *.hex *.map *.lst cscope* tags $(DEPDIR) $(OBJDIR)

install :
	lpc21isp $(TARGET).hex /dev/ttyUSB0 115200 4000

vim :
	ctags -R --extra=+f *
	find . -name "*.[csh]" > cscope.files
	cscope -q -R -b -i cscope.files

print-% :
	@echo $* = $($*)
	$(info TARGET is $(TARGET))
