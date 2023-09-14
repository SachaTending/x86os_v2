CC=gcc
NASM=nasm
LD=ld

NFLAGS = -felf32 -g
CFLAGS = -m32 -march=i386 -c -fno-leading-underscore -I base/include -g -ffreestanding
LFLAGS = -Tlink.ld -melf_i386 -A i386 -g

OBJ = 

build: image.iso

-include targets/*.mk
.PHONY: build image.iso kernel.elf
image.iso: kernel.elf
	@cp kernel.elf iso
	@xorriso -as mkisofs -b limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso -o image.iso

kernel.elf: $(OBJ)
	@echo "  [  LD] kernel.elf"
	@$(LD) $(LFLAGS) $(OBJ) -o kernel.elf

%.o: %.c
	@echo "  [  CC] $@"
	@$(CC) $(CFLAGS) -o $@ $<

%.o: %.cpp
	@echo "  [ C++] $@"
	@g++ $(CFLAGS) -o $@ $<

%.o: %.asm
	@echo "  [NASM] $@"
	@$(NASM) $(NFLAGS) -o $@ $<

clean:
	@-rm $(OBJ) image.iso iso/kernel.elf kernel.elf

run: build
	@qemu-system-i386 -cdrom image.iso -m 512M -debugcon stdio