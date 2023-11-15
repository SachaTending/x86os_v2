#include <stdint.h>
#include <libc.hpp>
#include <common.hpp>
#include <io.h>

int x, y, fx, fy; // psf

uint32_t fg = 0xffffff;
uint32_t bg = 0;

#define PORT 0x3f8          // COM1
 
static int init_serial() {
    outb(PORT + 1, 0x00);    // Disable all interrupts
    outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00);    //                  (hi byte)
    outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
    outb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
    
    // Check if serial is faulty (i.e: not same byte as sent)
    if(inb(PORT + 0) != 0xAE) {
        return 1;
    }
    
    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(PORT + 4, 0x0F);
    return 0;
}
bool serial_done = false;
void putchar(
    /* note that this is int, not char as it's a unicode character */
    unsigned short int c,
    /* cursor position on screen, in characters not in pixels */
    int cx, int cy,
    /* foreground and background colors, say 0xFFFFFF and 0x000000 */
    uint32_t fg, uint32_t bg);
void dbgputchar(char c);

uint32_t fb = 0;

void putc_init() {
    fb = (uint32_t)malloc(mbi->framebuffer_pitch*mbi->framebuffer_height);
    //mbi->framebuffer_addr = (uint32_t)malloc(mbi->framebuffer_pitch*mbi->framebuffer_height);
    memset((void *)fb, 0, mbi->framebuffer_pitch*mbi->framebuffer_height);
}

void flush_fb() {
    memcpy((void *)fb, (const void *)mbi->framebuffer_addr, mbi->framebuffer_pitch*mbi->framebuffer_height);
}

void putc(char c) {
    if (c == '\n') dbgputchar('\r');
    dbgputchar(c);
    if (c == '\n') {
        //flush_fb();
        y+=14;
        x = 0;
        return;
    }
    putchar(c, x, y, fg, bg);
    x+=8;
    if (x > 1024) {
        x = 0;
        y+=14;
    } if (y > 768) {
        //memcpy((void *)mbi->framebuffer_addr, (void *)(mbi->framebuffer_addr+(mbi->framebuffer_pitch*14)), mbi->framebuffer_pitch*1024-14);
        memcpy((void *)fb, (void *)(fb+(mbi->framebuffer_pitch*14)), mbi->framebuffer_pitch*1024-14);
        memcpy((void *)mbi->framebuffer_addr, (const void *)fb, mbi->framebuffer_pitch*1024-14);
        y -= 14;
        x = 0;
        putchar(c, x, y, fg, bg);
        x+=8;
    }
    if (!serial_done) {
        init_serial();
        serial_done = true;
    }
}