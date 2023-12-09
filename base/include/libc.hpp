#pragma once
#include <stdint.h>

// String operations
size_t strlen(const char *txt);
int strcpy(char *dst, const char *src);
int strcmp(const char *dst, char *src);
int strncmp(const char *dst, char *src);
int strtok(const char *str, char tok);
char *strdup(char *str);

// Conversion
void itoa(char *buf, unsigned long int n, int base);
int atoi(char * string);

// Memory operations
extern "C" void *memset(void *dst, char v, int n);
extern "C" int memcmp(uint8_t * data1, uint8_t * data2, int n);
extern "C" int memcmpn(uint8_t * data1, uint8_t * data2, int n);
extern "C" int memcpy(void *dst, const void *src, int num);

// Allocations
void *malloc(size_t size);
void free(void *addr);
#define alloc_type(type) (type *)malloc(sizeof(type))

// Other
extern "C" void printf(const char *fmt, ...);

// Kernel stuff(very important)
void panic(const char *fmt, ...);