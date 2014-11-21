TARGET := 3DS_multitool_thing

SOURCES = source
INCLUDES = include

C_FILES := $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
S_FILES := $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.S))
OBJS    := $(C_FILES:.c=.o) $(S_FILES:.S=.o)

PREFIX=arm-none-eabi
CC=$(PREFIX)-gcc
LD=$(PREFIX)-ld
STRIP=$(PREFIX)-strip
OBJCOPY=$(PREFIX)-objcopy

ARCH    = -marm -mcpu=arm946e-s -march=armv5te -mlittle-endian -fshort-wchar -fno-zero-initialized-in-bss
ASFLAGS = $(ARCH)
CFLAGS  = -Os -Wall $(ASFLAGS) -I$(INCLUDES)
LDFLAGS = -nostartfiles -nostdlib
KEY     = 580006192800C5F0FBFB04E06A682088
IV      = 00000000000000000000000000000000

all: Launcher.dat

Launcher.dat: $(TARGET).bin
	python tools/3dsploit.py $(TARGET).bin MsetForBoss.dat
	openssl enc -aes-128-cbc -K $(KEY) -iv $(IV) -in MsetForBoss.dat -out Launcher.dat

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -S -O binary $(TARGET).elf $(TARGET).bin
$(TARGET).elf: $(OBJS)
	$(LD) $(LDFLAGS) -T linker.x $(OBJS) -o $(TARGET).elf

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S
	$(CC) $(ASFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJS) $(TARGET).elf $(TARGET).bin MsetForBoss.dat Launcher.dat
	@echo "Cleaned!"
