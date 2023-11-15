#include <multiboot.h>
#include <libc.hpp>
#include <common.hpp>
#include <krnl.hpp>
#include <idt.hpp>
#include <gdt.hpp>
#include <io.h>
#include <logger.hpp>

static logger log("Bootstrap");

extern uint32_t fb;

static int *screen = (int *)mbi->framebuffer_addr;
void flush_fb();
void putpixel(int x, int y, uint32_t color) {
    if (color == 0) {
        return;
    }
    size_t where = x + y * (mbi->framebuffer_pitch / sizeof(uint32_t));
    #define SET(off,c) screen[where + off] = c;
    for (int i=0;i<1;i++) {
        SET(i, color);
        //SET(i, where);
        ((uint32_t *)fb)[i+where] = color;
    }
}

void dbgputchar(char c) {
    outb(0x3f8, c);
}
void putc(char c);
void printdbg(const char *txt) {
    while (*txt) {
        putc(*txt);
        txt++;
    }
}

multiboot_info_t *mbi;

typedef void (*constructor)();
extern constructor start_ctors;
extern constructor end_ctors;

void callConstructors(void)
{
    for(constructor* i = &start_ctors;i != &end_ctors; i++)
        (*i)();
}

void psf_init();
void putc_init();
extern "C" uint8_t init_main(multiboot_info_t *mbi2) {
    mbi = mbi2;
    Kernel::pmm_setup();
    psf_init();
    callConstructors();
    putc_init();
    screen = (int *)mbi->framebuffer_addr;
    log.info("Oh, welcome to x86OS. Currently, kernel is setupping enviroment, then, kernel gonna do all things\n");
    IDT::Init();
    log.info("IDT: Init done.\n");
    GDT::Init();
    log.info("GDT: Init done.\n");
    log.info("All done for kernel main, jumping to it!\n");
    Kernel::Main();
    return 1;
}