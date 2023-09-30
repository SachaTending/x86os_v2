#include <libc.hpp>
#include <logger.hpp>
#include <stddef.h>

void vsprintf(char * str, void (*putchar)(char), const char * format, va_list arg);
void putc(char c);

logger::logger(const char *n) {
    this->name = n;
}

extern uint32_t fg;
extern uint32_t bg;

#define ARGB(a, r, g, b) (a << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF)
int timer_tick;
#define LOGURU_GREEN ARGB(255, 21, 124, 17);
#define LOGURU_BLUE ARGB(255, 41, 95, 193);
#define LOGURU_WHITE ARGB(255, 185, 185, 185);
#define LOGURU_RED ARGB(255, 177, 61, 71);
#define LOGURU_CRITICAL ARGB(255, 152, 20, 31);
#define LOGURU_YELLOW ARGB(255, 190, 184, 129)

#define lock() asm volatile("" : : : "memory"); \
    while (this->lock); \
    this->lock = true;

#define unlock this->lock = false;

void logger::info(const char *fmt, ...) {
    lock();
    fg = LOGURU_GREEN;
    printf("[%d]", timer_tick);
    fg = LOGURU_WHITE;
    printf("[%s][INFO]: ", this->name);
    va_list ap;
    va_start(ap, fmt);
    vsprintf(NULL, putc, fmt, ap);
    va_end(ap);
    unlock;
}

void logger::warning(const char *fmt, ...) {
    lock();
    fg = LOGURU_GREEN;
    printf("[%d]", timer_tick);
    fg = LOGURU_YELLOW;
    printf("[%s][WARNING]: ", this->name);
    va_list ap;
    va_start(ap, fmt);
    vsprintf(NULL, putc, fmt, ap);
    va_end(ap);
    fg = LOGURU_WHITE;
    unlock;
}

void logger::error(const char *fmt, ...) {
    lock();
    fg = LOGURU_GREEN;
    printf("[%d]", timer_tick);
    fg = LOGURU_RED;
    printf("[%s][ERROR]: ", this->name);
    va_list ap;
    va_start(ap, fmt);
    vsprintf(NULL, putc, fmt, ap);
    va_end(ap);
    fg = LOGURU_WHITE;
    unlock;
}

void logger::debug(const char *fmt, ...) {
    lock();
    fg = LOGURU_GREEN;
    printf("[%d]", timer_tick);
    fg = LOGURU_BLUE;
    printf("[%s][DEBUG]: ", this->name);
    va_list ap;
    va_start(ap, fmt);
    vsprintf(NULL, putc, fmt, ap);
    va_end(ap);
    fg = LOGURU_WHITE;
    unlock;
}

void logger::critical(const char *fmt, ...) {
    fg = LOGURU_GREEN;
    printf("[%d]", timer_tick);
    fg = LOGURU_WHITE;
    bg = LOGURU_CRITICAL;
    printf("[%s][CRITICAL]: ", this->name);
    va_list ap;
    va_start(ap, fmt);
    vsprintf(NULL, putc, fmt, ap);
    va_end(ap);
    fg = LOGURU_WHITE;
    bg = 0;
}