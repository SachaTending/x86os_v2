#include <multiboot.h>
#include <libc.hpp>
#include <common.hpp>
#include <krnl.hpp>

static int *screen = (int *)mbi->framebuffer_addr;

void putpixel(int x, int y, uint32_t color) {
    size_t where = y + x * (mbi->framebuffer_pitch / sizeof(uint32_t));
    #define SET(off) screen[where + off] = color;
    for (int i=0;i<1;i++) SET(i);
}
static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) :"memory");
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

void dbgputchar(char c) {
    outb(0xe9, c);
}

void printdbg(const char *txt) {
    for (int i=0;i<strlen(txt);i++) {
        dbgputchar(txt[i]);
    }
}
#define PORT 0x3f8          // COM1
multiboot_info_t *mbi;
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))
extern "C" void init_main(multiboot_info_t *mbi2) {
    mbi = mbi2;
    screen = (int *)mbi->framebuffer_addr;
    Kernel::pmm_setup();
    for (int i=0;i<50;i++) {
        for (int y=0;y<50;y++) {
            putpixel(i, y, 0xff+(i*y));
        }
    }
}