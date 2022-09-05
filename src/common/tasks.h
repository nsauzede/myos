#ifndef __TASKS_H__
#define __TASKS_H__

#include "pt.h"

typedef char (*pt_fun_t)(struct pt *pt, int tid, void *arg);

extern int ntasks;
int init_tasks();
int create_task(pt_fun_t fun, void *arg);
int run_task(int tid);
int delete_task(int tid);
int schedule_tasks();

#endif/*__TASKS_H__*/
