#pragma once
#include <stdint.h>

typedef struct gdt_entry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct gdt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_t;


namespace GDT
{
    void Init();
    void SetEntry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
} // namespace GDT
