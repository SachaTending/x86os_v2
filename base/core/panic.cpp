#include <logger.hpp>
#include <libc.hpp>
#include <stddef.h>

static logger log("Kernel");
void vsprintf(char * str, void (*putchar)(char), const char * format, va_list arg);
void dummy_putc(char c) {
    (void)c;
}
void panic(const char *fmt, ...) {
    char buf[8192]; // panic msg can be looooooooooooooooong
    va_list lst;
    va_start(lst, fmt);
    vsprintf((char *)buf, dummy_putc, fmt, lst);
    va_end(lst);
    log.critical("KERNEL CRASHED!!! %s\n", &buf);
    // So... kernel crashed, and we need to stop everything.
    asm volatile ("cli");
    for (;;) asm volatile ("hlt"); // handle smi
}