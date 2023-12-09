#pragma once
#include <idt.hpp>

enum TASK_STATE {
    TASK_CREATED,
    TASK_RUNNING,
    TASK_PENDING,
    TASK_KILLED
};

struct task_struct
{
    int pid;
    const char *name;
    registers_t regs;
    uint32_t stack, entry;
    task_struct *next;
    TASK_STATE state;
};

extern task_struct *root_task;
extern task_struct *current_task;

typedef void (*task_entry)();
void create_task(task_entry entry, const char *name);
void sched_init();
void sched_start();

int getpid();

void kill_task(int pid);