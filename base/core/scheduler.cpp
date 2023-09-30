#include <idt.hpp>
#include <libc.hpp>

enum TASK_STATE {
    TASK_CREATED,
    TASK_RUNNING,
    TASK_PENDING
};

struct tast_struct
{
    int pid;
    const char *name;
    registers_t regs;
    uint32_t stack, entry;
    tast_struct *next;
    TASK_STATE state;
};

tast_struct *root_task = 0;
tast_struct *current_task = 0;
int nextpid = 0;

void idle() {
    for(;;) asm volatile ("hlt"); // Idle task, just executes hlt in loop
}
extern int timer_tick;
void sched(registers_t *regs) {
    timer_tick++;
    if (current_task->state == TASK_CREATED) {
        regs->eax = 0;
        regs->ebx = 0;
        regs->edi = 0;
        regs->ecx = 0;
        regs->ebp = 0;
        regs->esp = current_task->stack;
        regs->eip = current_task->entry;
        current_task->state = TASK_RUNNING;
        //printf("eip: 0x%x\n", regs->eip);
        return;
    }
    memcpy(&current_task->regs, regs, sizeof(registers_t));
    current_task->state = TASK_PENDING;
    current_task = current_task->next;
    if (current_task == 0) current_task = root_task;
    if (current_task->state == TASK_CREATED) {
        regs->eax = 0;
        regs->ebx = 0;
        regs->edi = 0;
        regs->ecx = 0;
        regs->ebp = 0;
        regs->esp = current_task->stack;
        regs->eip = current_task->entry;
        current_task->state = TASK_RUNNING;
        return;
    }
    memcpy(regs, &current_task->regs, sizeof(registers_t));
    current_task->state = TASK_RUNNING;
}
typedef void (*task_entry)();
void create_task(task_entry entry, const char *name) {
    tast_struct *task = new tast_struct;
    task->entry = (uint32_t)entry;
    task->name = name;
    task->stack = (uint32_t)new uint32_t[16384/4];
    task->next = 0;
    task->state = TASK_CREATED;
    task->pid = nextpid;
    nextpid++;
    tast_struct *idk = root_task;
    while (idk->next != 0)
    {
        idk = idk->next;
    }
    idk->next = task;
}

void sched_init() {
    current_task = new tast_struct; // Create first task
    memset(current_task, 0, sizeof(tast_struct));
    current_task->entry = (uint32_t)idle; // First task gonna be idle
    current_task->stack = (uint32_t)new uint32_t[16384/4];
    current_task->name = "Idle task";
    current_task->pid = nextpid;
    current_task->state = TASK_CREATED;
    root_task = current_task;
    nextpid++;
}

void sched_start() {
    IDT::SetStub(sched, 0);
}