#include <stdint.h>
#include <base.hpp>
#include <libc.hpp>
#include <logger.hpp>
#include <io.h>
#include <idt.hpp>
#include <acpi.hpp>

static logger log("ACPI");

uint32_t *acpi = (uint32_t *)0x000E0000;

struct RSDP_t {
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
} __attribute__ ((packed));

RSDP_t *rsdp = 0;

bool doChecksum(ACPISDTHeader *tableHeader)
{
    unsigned char sum = 0;
 
    for (int i = 0; i < tableHeader->Length; i++)
    {
        sum += ((char *) tableHeader)[i];
    }
 
    return sum == 0;
}

ACPISDTHeader *rsdt = 0;
uint32_t *rsdt_pointers = 0;
int rsdt_fields = 0;

struct GenericAddressStructure
{
    uint8_t AddressSpace;
    uint8_t BitWidth;
    uint8_t BitOffset;
    uint8_t AccessSize;
    uint64_t Address;
};

struct FADT
{
    struct   ACPISDTHeader h;
    uint32_t FirmwareCtrl;
    uint32_t Dsdt;
 
    // field used in ACPI 1.0; no longer in use, for compatibility only
    uint8_t  Reserved;
 
    uint8_t  PreferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t  AcpiEnable;
    uint8_t  AcpiDisable;
    uint8_t  S4BIOS_REQ;
    uint8_t  PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t  PM1EventLength;
    uint8_t  PM1ControlLength;
    uint8_t  PM2ControlLength;
    uint8_t  PMTimerLength;
    uint8_t  GPE0Length;
    uint8_t  GPE1Length;
    uint8_t  GPE1Base;
    uint8_t  CStateControl;
    uint16_t WorstC2Latency;
    uint16_t WorstC3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t  DutyOffset;
    uint8_t  DutyWidth;
    uint8_t  DayAlarm;
    uint8_t  MonthAlarm;
    uint8_t  Century;
 
    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t BootArchitectureFlags;
 
    uint8_t  Reserved2;
    uint32_t Flags;
 
    // 12 byte structure; see below for details
    GenericAddressStructure ResetReg;
 
    uint8_t  ResetValue;
    uint8_t  Reserved3[3];
 
    // 64bit pointers - Available on ACPI 2.0+
    uint64_t                X_FirmwareControl;
    uint64_t                X_Dsdt;
 
    GenericAddressStructure X_PM1aEventBlock;
    GenericAddressStructure X_PM1bEventBlock;
    GenericAddressStructure X_PM1aControlBlock;
    GenericAddressStructure X_PM1bControlBlock;
    GenericAddressStructure X_PM2ControlBlock;
    GenericAddressStructure X_PMTimerBlock;
    GenericAddressStructure X_GPE0Block;
    GenericAddressStructure X_GPE1Block;
};
int t = 100;

typedef union pci_dev {
    uint32_t bits;
    struct {
        uint32_t always_zero    : 2;
        uint32_t field_num      : 6;
        uint32_t function_num   : 3;
        uint32_t device_num     : 5;
        uint32_t bus_num        : 8;
        uint32_t reserved       : 7;
        uint32_t enable         : 1;
    };
} pci_dev_t;

extern "C" {
    int lai_start_pm_timer();
    void lai_set_acpi_revision(int);
    void lai_create_namespace();
    void laihost_log(int lvl, const char *msg) {
        log.info("LAI: Level: %d, MSG: %s\n", lvl, msg);
    }
    uint32_t laihost_scan(char *sig, size_t index) {
        //log.debug("LAIHOST_SCAN: SIG: %s INDEX: %d\n", sig, index);
        size_t ind = 0;
        if (sig[0] == 'D') {
            //log.debug("Searching for FADP...\n");
            for (int i=0;i<rsdt_fields;i++) {
            if (!strcmp((const char *)rsdt_pointers[i], (char *)"FACP")) {
                FADT *fadt = (FADT *)rsdt_pointers[i];
                #define macro(a) ((ACPISDTHeader *)fadt->Dsdt)->Signature[a]
                if (rsdp->Revision >= 2) {
                    return (uint32_t)fadt->X_Dsdt;
                }
                //log.debug("Found DSDT At 0x%x\n", fadt->Dsdt);
                //log.debug("Signature: %c%c%c%c\n", macro(0), macro(1), macro(2), macro(3));
                return fadt->Dsdt;
            }
        }
        }
        for (int i=0;i<rsdt_fields;i++) {
            if (!strcmp((const char *)rsdt_pointers[i], sig)) {
                if (ind == index) {
                    //log.debug("Found %s at 0x%x\n", sig, rsdt_pointers[i]);
                    return rsdt_pointers[i];
                }
                ind++;
            }
        }
        log.warning("Table %s not found in acpi\n", sig);
        return 0;
    }
    void laihost_panic(const char *msg) {
        log.critical("LAI: PANIC: %s\n", msg);
        for(;;);
    }
    void laihost_sleep(uint64_t ms) {
        log.debug("LAI: laihost_sleep(%u)\n", ms);
    }
    uint64_t laihost_timer() {
        return t+=100;
    }
    void laihost_outb(uint16_t port, uint8_t val) {
        //log.debug("LAI: OUTB: 0x%x to 0x%x\n", val, port);
        outb(port, val);
    }
    void laihost_outw(uint16_t port, uint16_t val) {
        //log.debug("LAI: OUTW: 0x%x to 0x%x\n", val, port);
        outw(port, val);
    }
    void laihost_outd(uint16_t port, uint32_t val) {
        //log.debug("LAI: OUTL: 0x%x to 0x%x\n", val, port);
        outl(port, val);
    }
    uint8_t laihost_inb(uint16_t port) {
        //log.debug("LAI: INB 0x%x = ", port);
        uint8_t val = inb(port);
        //printf("0x%x\n", val);
        return val;
    }
    uint16_t laihost_inw(uint16_t port) {
        //log.debug("LAI: INW 0x%x = ", port);
        uint16_t val = inw(port);
        //printf("0x%x\n", val);
        return val;
    }
    uint32_t laihost_ind(uint16_t port) {
        //log.debug("LAI: INL 0x%x = ", port);
        uint32_t val = inl(port);
        //printf("0x%x\n", val);
        return val;
    }
    void laihost_handle_amldebug(void *ptr) {
        //log.debug("LAI: DBG: 0x%x\n", ptr);
    }
    int lai_enable_acpi(uint32_t);
    int lai_disable_acpi(void);

    uint8_t laihost_pci_readb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t off) {
        pci_dev_t dev;
        dev.bus_num = bus;
        dev.device_num = slot;
        dev.function_num = fun;
        (void)seg;
        dev.field_num = (off & 0xFC) >> 2;
        outl(0xCF8, dev.bits);
        return inb(0xCFC + (off & 3));
    }

    uint16_t laihost_pci_readw(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t off) {
        pci_dev_t dev;
        dev.bus_num = bus;
        dev.device_num = slot;
        dev.function_num = fun;
        (void)seg;
        dev.field_num = (off & 0xFC) >> 2;
        outl(0xCF8, dev.bits);
        return inw(0xCFC + (off & 2));
    }
    void *laihost_map(size_t addr, size_t count) {
        return (void *)addr;
    }
    uint32_t laihost_pci_readd(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t off) {
        pci_dev_t dev;
        dev.bus_num = bus;
        dev.device_num = slot;
        dev.function_num = fun;
        (void)seg;
        dev.field_num = (off & 0xFC) >> 2;
        outl(0xCF8, dev.bits);
        return inl(0xCFC);
    }

    uint16_t lai_get_sci_event(void);
    void lai_set_sci_event(uint16_t);
    void lai_acpi_reset();
    void lai_enter_sleep(uint8_t sleep_state);
}

FADT *fadt = 0;

void acpi_event(registers_t *regs) {
    (void)regs;
    log.debug("ACPI Event: 0x%x\n", fadt->PM1aEventBlock);
}

void madt_setup();

namespace ACPI
{
    void Setup() {
        for (int i=0;i<(0x000FFFFF-0x000E0000);i+=16) {
            if (strncmp((const char *)&acpi[i], (char *)"RSD PTR ")) {
                log.info("Found RSDP at 0x%x\n", &acpi[i]);
                acpi = &acpi[i];
                break;
            }
        }
        rsdp = (RSDP_t *)acpi;
        log.info("OEMID: "); 
        for (int i=0;i<6;i++) printf("%c", rsdp->OEMID[i]);
        printf("\n");
        log.info("Revisison: %d\n", rsdp->Revision);
        rsdt = (ACPISDTHeader *)rsdp->RsdtAddress;
        log.info("RSDT: Checksum: %s\n", doChecksum(rsdt) ? "Valid" : "Not valid");
        log.info("RSDT: OEMID: ");
        for (int i=0;i<6;i++) printf("%c", rsdt->OEMID[i]);
        printf("\n");
        rsdt_fields = (rsdt->Length - sizeof(ACPISDTHeader)) / 4;
        log.info("Fields: %d\n", rsdt_fields);
        rsdt_pointers = (uint32_t *)(((int)rsdt) + sizeof(ACPISDTHeader));
        #undef macro
        #define macro(a) ((ACPISDTHeader *)rsdt_pointers[i])->Signature[a]
        for (int i=0;i<rsdt_fields;i++) {
            log.info("%d. %c%c%c%c\n", i, macro(0), macro(1), macro(2), macro(3));
        }
        fadt = (FADT *)laihost_scan((char *)"FACP", 0);
        IDT::SetStub(acpi_event, fadt->SCI_Interrupt);
        log.info("SCI Interrupt: %d\n", fadt->SCI_Interrupt);
        lai_set_acpi_revision(rsdp->Revision);
        lai_create_namespace();
        lai_start_pm_timer();
        log.info("LAI Loaded.\n");
        lai_enable_acpi(0);
        madt_setup();
        //outb(fadt->SMI_CommandPort, fadt->AcpiEnable);
    }
    uint32_t GetTable(char *sig, size_t index) {
        //log.debug("GetTable: SIG: %s INDEX: %d\n", sig, index);
        size_t ind = 0;
        if (sig[0] == 'D') {
            log.debug("Searching for FADP...\n");
            for (int i=0;i<rsdt_fields;i++) {
            if (!strcmp((const char *)rsdt_pointers[i], (char *)"FACP")) {
                FADT *fadt = (FADT *)rsdt_pointers[i];
                #define macro(a) ((ACPISDTHeader *)fadt->Dsdt)->Signature[a]
                log.debug("Found DSDT At 0x%x\n", fadt->Dsdt);
                log.debug("Signature: %c%c%c%c\n", macro(0), macro(1), macro(2), macro(3));
                return fadt->Dsdt;
            }
        }
        }
        for (int i=0;i<rsdt_fields;i++) {
            if (!strcmp((const char *)rsdt_pointers[i], sig)) {
                if (ind == index) {
                    //log.debug("Found %s at 0x%x\n", sig, rsdt_pointers[i]);
                    return rsdt_pointers[i];
                }
                ind++;
            }
        }
        log.warning("Table %s not found in acpi\n", sig);
        return 0;
    }
} // namespace ACPI
