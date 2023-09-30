#include <multiboot.h>
#include <libc.hpp>
#include <common.hpp>
#include <krnl.hpp>
#include <idt.hpp>
#include <gdt.hpp>
#include <io.h>
#include <logger.hpp>

static logger log("Bootstrap");

static int *screen = (int *)mbi->framebuffer_addr;

void putpixel(int x, int y, uint32_t color) {
    size_t where = x + y * (mbi->framebuffer_pitch / sizeof(uint32_t));
    #define SET(off) screen[where + off] = color;
    for (int i=0;i<1;i++) SET(i);
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
extern "C" void init_main(multiboot_info_t *mbi2) {
    mbi = mbi2;
    screen = (int *)mbi->framebuffer_addr;
    Kernel::pmm_setup();
    psf_init();
    callConstructors();
    log.info("Oh, welcome to x86OS. Currently, kernel is setupping enviroment, then, kernel gonna do all things\n");
    IDT::Init();
    log.info("IDT: Init done.\n");
    GDT::Init();
    log.info("GDT: Init done.\n");
    log.info("All done for kernel main, jumping to it!\n");
    Kernel::Main();
}