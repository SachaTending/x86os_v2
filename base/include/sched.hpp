#pragma once

typedef void (*task_entry)();
void create_task(task_entry entry, const char *name);
void sched_init();
void sched_start();