CC=gcc
NASM=nasm
LD=ld

NFLAGS = -felf32 -g
CFLAGS = -m32 -march=i586 -c -fno-leading-underscore -I base/include -ffreestanding -O2 -Wall -Wextra -fpermissive -g -fno-stack-protector -fno-stack-check
LFLAGS = -Tlink.ld -melf_i386 -A i586 -g

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
	@g++ $(CFLAGS) -fno-exceptions -fno-rtti -o $@ $<

%.o: %.asm
	@echo "  [NASM] $@"
	@$(NASM) $(NFLAGS) -o $@ $<

%.o: %.S
	@echo "  [ GAS] $@"
	@$(CC) $(CFLAGS) -o $@ $<

clean:
	@-rm $(OBJ) image.iso iso/kernel.elf kernel.elf

run: build
	@qemu-system-i386 -cdrom image.iso -m 512M -serial stdio -smp cores=2