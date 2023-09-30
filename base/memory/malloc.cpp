#include <libc.hpp>
#include <common.hpp>
#include <krnl.hpp>
static int alloc_id;

struct alloc_struct
{
    bool allocated;
    int id;
    int size;
    char magic[5];
    alloc_struct *prev;
    alloc_struct *next;
};

int avaible_mem, total_mem;

static multiboot_mmap_entry big;
extern int kernel_start;
extern int kernel_end;
static alloc_struct *root;
void printdbg(const char *c);
void Kernel::pmm_setup() {
    multiboot_mmap_entry *mmap = (multiboot_mmap_entry *)mbi->mmap_addr;
    mmap = (multiboot_mmap_entry *)mbi->mmap_addr;
    for (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
           (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
           mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
                                    + mmap->size + sizeof (mmap->size))) {
        //total_mem += mmap->len;
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            avaible_mem += mmap->len;
            if (mmap->len > big.len) {
                big = *mmap;
            }
        }
    }
    if (big.addr == (uint64_t)&kernel_start) {
        big.addr = (uint64_t)&kernel_end+1;
        big.len = big.len - (&kernel_end - &kernel_start);
    }
    root = (alloc_struct *)big.addr;
    root->allocated = false;
    strcpy((char *)&root->magic, "FLOPPA");
    root->allocated = false;
    root->next = root;
    root->prev = root;
    root->id = alloc_id;
    root->size = 0;
    alloc_id++;
}

static bool check_magic(alloc_struct *entr) {
    return (entr->magic[0] == 'F' && entr->magic[1] == 'L' && entr->magic[2] == 'O' && entr->magic[3] == 'P' && entr->magic[4] == 'P' && entr->magic[5] == 'A');
}

void debug_null(const char *m, ...) {

}

#define debug debug_null

static void set_signature(alloc_struct *alloc) {
    strcpy((char *)&alloc->magic, "FLOPPA");
}

void * malloc(size_t size) {
    alloc_struct *d = (alloc_struct *)(big.addr + sizeof(alloc_struct));
    alloc_struct *prev;
    debug("Searching block size: %u\n", size);
    while ((int)d < (big.len + big.addr)) {
        debug("Addr: 0x%x\n", d);
        if (check_magic(d)) {
            // This is allocated block
            debug("Found block\n");
            if (d->allocated == false && d->size >= size) {
                // Found free block, set allocated to true and return it
                debug("Block is unallocated");
                d->allocated = true;
                return d += sizeof(alloc_struct);
            } else {
                debug("Block allocated.\n");
                prev = d;
                d += d->size + sizeof(alloc_struct);
            }
        } else {
            // Found unallocated space
            if (((int)d + sizeof(alloc_struct) + size) > (big.addr + big.len)) {
                // bruh
                return (void *)-1;
            } else {
                // Allocate space
                debug("Allocating block at 0x%x size %d...\n", d, size);
                d->allocated = true;
                d->prev = prev;
                prev->next = d;
                set_signature(d);
                d->size = size;
                d->id = alloc_id;
                alloc_id++;
                return (void *)((uint32_t *)d + sizeof(alloc_struct));
            }
        }
    }
    debug("No free block found.\n");
    printf("WE FUCKED UP, NO MEMORY.\n");
    return (void *)-1;
}
void free(void *mem) {
    alloc_struct *d= (alloc_struct *)((uint32_t *)mem - sizeof(alloc_struct));
    if (check_magic(d)) {
        if (d->allocated) {
            d->allocated = false;
        }
    }
}