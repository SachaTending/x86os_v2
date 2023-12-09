#include <libc.hpp>

uint32_t page_directory[1024] __attribute__((aligned(4096)));

uint32_t first_page_table[1024] __attribute__((aligned(4096)));

extern "C" {
    void loadPageDirectory(unsigned int*);
    void enablePaging();
}

void page_setup() {
    //set each entry to not present
    unsigned int i;
    for(i = 0; i < 1024; i++)
    {
        // This sets the following flags to the pages:
        //   Supervisor: Only kernel-mode can access them
        //   Write Enabled: It can be both read from and written to
        //   Not Present: The page table is not present
        page_directory[i] = 0x00000002;
    }
    
    //we will fill all 1024 entries in the table, mapping 4 megabytes
    for(i = 0; i < 1024; i++)
    {
        // As the address is page aligned, it will always leave 12 bits zeroed.
        // Those bits are used by the attributes ;)
        first_page_table[i] = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
        //printf("map: 0x%p -> 0x%p\n", i, i);
    }
    printf("directory location: 0x%p\n", &page_directory);
    printf("SCARY PART: Starting paging...\n");
    loadPageDirectory(page_directory);
    printf("calling enablePaging()...\n");
    enablePaging();
    printf("test123\nif you see this, nothing go wrong.\n");
}