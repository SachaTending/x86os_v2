#pragma once
#include <stdint.h>

// String operations
size_t strlen(const char *txt);
int strcpy(char *dst, const char *src);
int strcmp(const char *dst, char *src);
int strncmp(const char *dst, char *src);

// Memory operations
void *memset(void *dst, char v, int n);
int memcmp(uint8_t * data1, uint8_t * data2, int n);
int memcmpn(uint8_t * data1, uint8_t * data2, int n);

// Allocations
void *malloc(size_t size);
void free(void *addr);