#include "tasks.h"
#include "pt.h"
#include "libc.h"

#define MAX_TASK 10
enum { STATE_BUSY = 0x1 };
typedef struct task {
	int state;
	struct pt pt;
	pt_fun_t fun;
	void *arg;
} task_t;
task_t tasks[MAX_TASK];
int ntasks = 0;
int first_free = 0;
int tid = 0;

int init_tasks()
{
	memset( tasks, 0, sizeof( tasks));
	return 0;
}

int schedule_tasks()
{
	if (ntasks > 0)
	{
		while (!(tasks[tid].state & STATE_BUSY))
			tid = (tid + 1) % MAX_TASK;
		run_task( tid);
		tid = (tid + 1) % MAX_TASK;
	}
	return 0;
}

int create_task( pt_fun_t fun, void *arg)
{
	if (ntasks >= MAX_TASK)
	{
		return -1;
	}
	int i;
	for (i = first_free; i < MAX_TASK; i++)
	{
		if (!(tasks[i].state & STATE_BUSY))
			break;
	}
	if (i >= MAX_TASK)
		return -1;
	tasks[i].state = STATE_BUSY;
	PT_INIT( &(tasks[i].pt));
	tasks[i].fun = fun;
	tasks[i].arg = arg;

	return ntasks++;
}

int run_task( int tid)
{
	tasks[tid].fun( &(tasks[tid].pt), tid, tasks[tid].arg);

	return 0;
}

int delete_task( int tid)
{
	if ((tid >= 0) && (tid < MAX_TASK))
	{
		if (tasks[tid].state & STATE_BUSY)
		{
			tasks[tid].state &= ~STATE_BUSY;
			if (tid < first_free)
				first_free = tid;
			ntasks--;
		}
	}
	
	return 0;
}
