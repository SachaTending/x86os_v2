// Check docs/Files and dirs.md for further info
// This code handles interrupts

#include <libc.hpp>
#include <idt.hpp>
#include <io.h>

InterruptDescriptor32 idt_desc[256];
idt_ptr idt_info;
//a
void idt_set_entry(int index, uint32_t base, uint16_t sel, uint8_t flags) {
    InterruptDescriptor32 * entry = &idt_desc[index];
    entry->offset_1 = base & 0xFFFF;
    entry->offset_2 = (base >> 16) & 0xFFFF;
    entry->zero = 0;
    entry->selector = sel;
    entry->type_attributes = flags | 0x60;
}

extern int timer_tick;
void timer(registers_t *regs) {
    (void)regs;
    timer_tick++;
}

irq_stub stubs[256-31] {timer};

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define PIC_EOI		0x20		/* End-of-interrupt command code */
 
void PIC_sendEOI(unsigned char irq)
{
	if(irq >= 8)
		outb(PIC2_COMMAND,PIC_EOI);
 
	outb(PIC1_COMMAND,PIC_EOI);
}
extern "C" void irq_handler(registers_t *regs) {
    if (regs->int_no > 31) {
        if (stubs[regs->int_no-32]) stubs[regs->int_no-32](regs);
        else if (regs->int_no-32 == 1) {
            printf("kbd: 0x%x\n", inb(0x60));
        }
        else {
            printf("unknown int %d\n", regs->int_no-32);
        }
        PIC_sendEOI(regs->int_no);
        return;
    }
    printf("ERR %d %u 0x%x\n", regs->int_no, regs->err_code, regs->eip);
    for(;;);
}

void IDT::SetStub(irq_stub stub, int no) {
    stubs[no] = stub;
}



/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */
 
#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */
 
/*
arguments:
	offset1 - vector offset for master PIC
		vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
*/
void io_wait() {

}
void PIC_remap(int offset1, int offset2)
{
	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	outb(PIC1_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

void IRQ_clear_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}

extern void* isr_stub_table[];
void IDT::Init() {
    memset(&idt_desc, 0, sizeof(idt_desc));
    idt_info.base = (uint32_t)idt_desc;
    idt_info.limit = sizeof(idt_desc) - 1;
    for (int i=0;i<64;i++) {
        idt_set_entry(i, (uint32_t)isr_stub_table[i], 0x08, 0x8E);
    }
    PIC_remap(32, 32+8);
    for (int i=0;i<16;i++) {
        IRQ_clear_mask(i);
    }
    __asm__ volatile ("lidt %0" : : "m"(idt_info)); // load the new IDT
    __asm__ volatile ("sti"); // set the interrupt flag
}
