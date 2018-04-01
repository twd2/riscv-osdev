GCC_PREFIX := riscv64-unknown-elf-
C := $(GCC_PREFIX)gcc
CC := $(GCC_PREFIX)g++
LD := $(GCC_PREFIX)ld
AS := $(GCC_PREFIX)as
OBJDUMP := $(GCC_PREFIX)objdump
OBJCOPY := $(GCC_PREFIX)objcopy
QEMU := qemu-system-riscv64

CFLAGS  := -mcmodel=medany -O2 -std=gnu99 -Wno-unused
CFLAGS	+= -fno-builtin -Wall -nostdinc
CFLAGS	+= -fno-stack-protector -ffunction-sections -fdata-sections

LDFLAGS	:= -m elf64lriscv
LDFLAGS	+= -nostdlib --gc-sections

.PHONY: all
all: image.bin

.PHONY: qemu
qemu: image.bin disk.img
	$(QEMU) -machine virt -nographic \
	-kernel $< \
	-smp 4 \
	-m 1024 \
	-s \
	-drive file=disk.img,format=raw,id=hd0 \
	-device virtio-blk-device,drive=hd0 #\
#	-netdev type=tap,script=./ifup,downscript=./ifdown,id=net0 \
#	-device virtio-net-device,netdev=net0

.PHONY: killqemu
killqeum:
	-killall qemu-system-riscv64

image.bin: kernel.elf
	cd riscv-pk && \
	rm -rf build && \
	mkdir build && \
	cd build && \
	../configure \
		--disable-fp-emulation \
		--enable-logo \
		--enable-print-device-tree \
		--disable-vm \
		--host=riscv64-unknown-elf \
		--with-payload=../../kernel.elf && \
	make && \
	cp bbl ../../$@

disk.img:
	dd if=/dev/zero of=disk.img bs=512 count=128k

kernel.elf: kernel.ld init.S.o trap_entry.S.o kmain.c.o test.c.o spinlock.S.o fdt.c.o printfmt.c.o string.c.o stdio.c.o
	$(LD) $(LDFLAGS) -o $@ -T $^
	$(OBJDUMP) -D $@ > kernel.elf.txt

%.cc.o: %.cc
	$(CC) -c $^ $(CCFLAGS) -o $@

%.S.o: %.S
	$(C) -c $^ $(CFLAGS) -o $@

%.c.o: %.c
	$(C) -c $^ $(CFLAGS) -o $@

.PHONY: clean
clean:
	-rm kernel.elf kernel.elf.txt
	-rm *.o
	-rm *.bin *.img
