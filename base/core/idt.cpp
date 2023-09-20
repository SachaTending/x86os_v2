// Check docs/Files and dirs.md for further info
// This code handles interrupts

#include <libc.hpp>
#include <idt.hpp>
#include <io.h>

InterruptDescriptor32 idt_desc[256];
idt_ptr idt_info;

void idt_set_entry(int index, uint32_t base, uint16_t sel, uint8_t flags) {
    InterruptDescriptor32 * entry = &idt_desc[index];
    entry->offset_1 = base & 0xFFFF;
    entry->offset_2 = (base >> 16) & 0xFFFF;
    entry->zero = 0;
    entry->selector = sel;
    entry->type_attributes = flags | 0x60;
}

irq_stub stubs[256-31];

extern "C" void irq_handler(registers_t *regs) {
    if (regs->int_no > 31) {
        if (stubs[regs->int_no]) stubs[regs->int_no](regs);
    }
    printf("ERR\n");
    for(;;);
}

void IDT::SetStub(irq_stub stub, int no) {
    stubs[no] = stub;
}

extern void* isr_stub_table[];
void IDT::Init() {
    memset(&idt_desc, 0, sizeof(idt_desc));
    idt_info.base = (uint32_t)idt_desc;
    idt_info.limit = sizeof(idt_desc) - 1;
    for (int i=0;i<256;i++) {
        idt_set_entry(i, (uint32_t)isr_stub_table[i], 0x08, 0x8E);
    }
    outb(0xa1, 0xff);
    outb(0x21, 0xff);
    __asm__ volatile ("lidt %0" : : "m"(idt_info)); // load the new IDT
    __asm__ volatile ("sti"); // set the interrupt flag
}
