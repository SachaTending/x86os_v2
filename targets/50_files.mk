OBJ += base/core/psfdraw.o font.o

font.o:
	objcopy -O elf32-i386 -B i386 -I binary font.psf font.o