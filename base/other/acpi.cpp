#include <stdint.h>
#include <base.hpp>
#include <libc.hpp>
#include <logger.hpp>

static logger log("ACPI");

uint32_t *acpi = (uint32_t *)0x000E0000;

struct RSDP_t {
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
} __attribute__ ((packed));

struct ACPISDTHeader {
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
};

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
        rsdt = (ACPISDTHeader *)rsdp->RsdtAddress;
        log.info("RSDT: Checksum: %s\n", doChecksum(rsdt) ? "Valid" : "Not valid");
        log.info("RSDT: OEMID: ");
        for (int i=0;i<6;i++) printf("%c", rsdt->OEMID[i]);
        printf("\n");
        rsdt_fields = (rsdt->Length - sizeof(ACPISDTHeader)) / 4;
        log.info("Fields: %d\n", rsdt_fields);
        rsdt_pointers = (uint32_t *)(((int)rsdt) + sizeof(ACPISDTHeader));
        #define macro(a) ((ACPISDTHeader *)rsdt_pointers[i])->Signature[a]
        for (int i=0;i<rsdt_fields;i++) {
            log.info("%d. %c%c%c%c\n", i, macro(0), macro(1), macro(2), macro(3));
        }
    }
} // namespace ACPI
