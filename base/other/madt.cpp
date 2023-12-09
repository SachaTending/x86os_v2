#include <acpi.hpp>
#include <logger.hpp>
#include <libc.hpp>

static logger log("MADT(ACPI)");

struct madt_record
{
    uint8_t type;
    uint8_t len;
};


struct madt_header
{
    ACPISDTHeader header;
    uint32_t lapic_addr;
    uint32_t flags;
    madt_record records[];
};

struct lapic
{
    madt_record header;
    uint8_t cpu_id;
    uint8_t apic_id;
    uint32_t flags;
};

struct ioapic
{
    madt_record header;
    uint8_t apic_id;
    uint8_t reserver;
    uint32_t addr;
    uint32_t gsi_base;
};


#define APIC_TYPE_LOCAL_APIC            0
#define APIC_TYPE_IO_APIC               1
#define APIC_TYPE_INTERRUPT_OVERRIDE    2

const char *typetostr(uint8_t type) {
    switch (type)
    {
        case APIC_TYPE_LOCAL_APIC:
            return "LAPIC";
            break;
        
        case APIC_TYPE_IO_APIC:
            return "IOAPIC";
            break;
        
        case APIC_TYPE_INTERRUPT_OVERRIDE:
            return "Interrupt override";
            break;

    default:
        return "Unknown";
        break;
    }
}
bool bsp_l = false;
void bsp_lock() {
    while (bsp_l);
    bsp_l = true;
}

void bsp_unlock() {
    bsp_l = false;
}

int cpuid = 0; // 0 is main cpu that runs kernel

extern "C" {
    volatile uint8_t aprunning = 0;  // count how many APs have started
    uint8_t bspid, bspdone = 0;      // BSP id and spinlock flag
    void ap_startup(int ap_id) {
        bsp_lock();
        //log.info("CPU%d started\n", cpuid);
        cpuid++;
        bsp_unlock();
        for(;;);
    }
}

uint8_t lapic_ids[256]={0};
madt_header *madt = 0;
int records_len = 0;
extern "C" void ap_trampoline();
extern "C" void lai_busy_wait_pm_timer(uint64_t);
void madt_setup() {
    log.info("Searching for MADT...\n");
    madt = (madt_header *)ACPI::GetTable((char *)"APIC", 0);
    if (!madt) {
        log.error("MADT Not found. Exiting.\n");
        return;
    }
    #define m(a) madt->header.Signature[a]
    log.info("Signature: %c%c%c%c\n", m(0), m(1), m(2), m(3));
    log.info("LAPIC Addr: 0x%x\n", madt->lapic_addr);
    records_len = madt->header.Length-(sizeof(ACPISDTHeader)+(sizeof(uint32_t)*2));
    log.info("Length without header and other things: %d\n", records_len);
    log.info("flags: %d\n", madt->flags);
    madt_record *m = 0;
    lapic *l = 0;
    ioapic *ioapic2 = 0;
    uint8_t *p=(uint8_t *)(madt+1);
    uint8_t *end=(uint8_t *)madt+madt->header.Length;
    int numcores = 0;
    int lapic_ptr=0;       // pointer to the Local APIC MMIO registers
    lapic_ptr = madt->lapic_addr;
    while (p < end) {
        m = (madt_record *)p;
        //log.info("type: %d %s\n", m->type, typetostr(m->type));
        switch (m->type)
        {
            case APIC_TYPE_LOCAL_APIC:
                l = (lapic *)p;
                //log.info("LAPIC:\n");
                //log.info("CPU ID: %d\n", l->cpu_id);
                //log.info("APIC ID: %d\n", l->apic_id);
                lapic_ids[numcores] = l->apic_id;
                numcores++;
                break;
            case APIC_TYPE_IO_APIC:
                ioapic2 = (ioapic *)p;
                //log.info("IOAPIC:\n");
                //log.info("CPU%d: 0x%x\n", ioapic2->apic_id, ioapic2->addr);
                break;
        }
        p += m->len;
    }
    int i, j;
    // get the BSP's Local APIC ID
    __asm__ __volatile__ ("mov $1, %%eax; cpuid; shrl $24, %%ebx;": "=b"(bspid) : : );
    
    // copy the AP trampoline code to a fixed address in low conventional memory (to address 0x0800:0x0000)
    memcpy((void*)0x8000, (const void *)&ap_trampoline, 4096);
    
    // for each Local APIC ID we do...
    for(i = 0; i < numcores; i++) {
        log.info("Starting CPU%d...\n", i);
        // do not start BSP, that's already running this code
        if(lapic_ids[i] == bspid) continue;
        // send INIT IPI
        *((volatile uint32_t*)(lapic_ptr + 0x280)) = 0;                                                                             // clear APIC errors
        *((volatile uint32_t*)(lapic_ptr + 0x310)) = (*((volatile uint32_t*)(lapic_ptr + 0x310)) & 0x00ffffff) | (i << 24);         // select AP
        *((volatile uint32_t*)(lapic_ptr + 0x300)) = (*((volatile uint32_t*)(lapic_ptr + 0x300)) & 0xfff00000) | 0x00C500;          // trigger INIT IPI
        do { __asm__ __volatile__ ("pause" : : : "memory"); }while(*((volatile uint32_t*)(lapic_ptr + 0x300)) & (1 << 12));         // wait for delivery
        *((volatile uint32_t*)(lapic_ptr + 0x310)) = (*((volatile uint32_t*)(lapic_ptr + 0x310)) & 0x00ffffff) | (i << 24);         // select AP
        *((volatile uint32_t*)(lapic_ptr + 0x300)) = (*((volatile uint32_t*)(lapic_ptr + 0x300)) & 0xfff00000) | 0x008500;          // deassert
        do { __asm__ __volatile__ ("pause" : : : "memory"); }while(*((volatile uint32_t*)(lapic_ptr + 0x300)) & (1 << 12));         // wait for delivery
        lai_busy_wait_pm_timer(10);                                                                                                                 // wait 10 msec
        // send STARTUP IPI (twice)
        for(j = 0; j < 2; j++) {
            *((volatile uint32_t*)(lapic_ptr + 0x280)) = 0;                                                                     // clear APIC errors
            *((volatile uint32_t*)(lapic_ptr + 0x310)) = (*((volatile uint32_t*)(lapic_ptr + 0x310)) & 0x00ffffff) | (i << 24); // select AP
            *((volatile uint32_t*)(lapic_ptr + 0x300)) = (*((volatile uint32_t*)(lapic_ptr + 0x300)) & 0xfff0f800) | 0x000608;  // trigger STARTUP IPI for 0800:0000
            //udelay(200);                                                                                                        // wait 200 usec
            for (int i=0;i<200;i++) asm volatile("pause" : : : "memory");
            do { __asm__ __volatile__ ("pause" : : : "memory"); }while(*((volatile uint32_t*)(lapic_ptr + 0x300)) & (1 << 12)); // wait for delivery
        }
    }
    // release the AP spinlocks
    bspdone = 1;
    // now you'll have the number of running APs in 'aprunning'
    while (cpuid < aprunning);
    log.info("CPUs running: %d\n", aprunning);
    log.info("All done!\n");
}