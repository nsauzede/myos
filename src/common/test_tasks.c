#include <stdio.h>

#include "tasks.h"

int dputchar( int c) {
    printf("%c", c);
    return 1;
}

void dputs( const char *s) {
    printf("%s", s);
}

static int done = 0;

typedef struct {
    int lock;           // tid of locker task
    int fd; void *buf; size_t count;
    ssize_t result;
} tread_t;
static tread_t g_tread;
#define TR(p,f) (((tread_t *)arg)->f)
#define READ(f) (g_tread.f)
PT_THREAD(tread(struct pt *pt, int tid, void *arg)) {
    PT_BEGIN(pt);
    printf("%s: %d hello\n", __func__, tid);
    while (1) {
        printf("%s: %d waiting read_lock..\n", __func__, tid);
        PT_WAIT_UNTIL(pt, TR(arg, lock));
        printf("%s: %d locked by %d\n", __func__, tid, TR(arg, lock));
        TR(arg, result) = 42;
        TR(arg, lock) = 0;
    }
    delete_task(tid);
    printf("%s: %d, bye\n", __func__, tid);
    PT_END(pt);
}

PT_THREAD(tfoo(struct pt *pt, int tid, void *arg)) {
    PT_BEGIN(pt);
    static int count;
    printf("%s: %d hello #%d\n", __func__, tid, count);
    PT_WAIT_UNTIL(pt, !READ(lock));
    READ(lock) = tid;
    READ(fd) = 0;
    READ(buf) = 0;
    READ(count) = 0;
    printf("%s: waiting for read result.. #%d\n", __func__, count);
    PT_WAIT_UNTIL(pt, !READ(lock));
    int n = READ(result);
    printf("%s: read returned %d #%d\n", __func__, n, count);
    count++;
    if (count > 3) {
        done = 1;
        delete_task(tid);
    }
    printf("%s: %d bye #%d\n", __func__, tid, count);
    PT_END(pt);
}

static struct pt *g_pt = 0;
ssize_t mread(int fd, void *buf, size_t count) {
    printf("%s: hello\n", __func__);
    tread_t ltread = {.lock = 0};
    create_task(tread, &ltread);
    schedule_tasks();
    printf("%s: bye\n", __func__);
    return 43;
}

PT_THREAD(tfoo2(struct pt *pt, int tid, void *arg)) {
    PT_BEGIN(pt);
    static int count;
    printf("%s: %d hello #%d\n", __func__, tid, count);
    static int fd;
    static void *buf;
    static size_t count_;
    static int n;
    g_pt = pt;
    printf("%s: %d calling read.. #%d\n", __func__, tid, count);
    n = mread(fd, buf, count_);
    PT_WAIT_UNTIL(pt, !READ(lock));
    printf("%s: %d read returned %d #%d\n", __func__, tid, n, count);
    count++;
    if (count > 3) {
        done = 1;
        delete_task(tid);
    }
    printf("%s: %d bye #%d\n", __func__, tid, count);
    PT_END(pt);
}

PT_THREAD(tinit(struct pt *pt, int tid, void *arg)) {
    PT_BEGIN(pt);
    printf("%s: hello\n", __func__);

    done = 0;
    static int tread_tid;
#if 1
    tread_tid = create_task(tread, &g_tread);
    create_task(tfoo, 0);
#elif 0
    create_task(tfoo2, 0);
#else
    create_task(tfoo3, 0);
#endif
    while (1) {
        printf("%s: waiting for done.. (%d)\n", __func__, done);
        PT_WAIT_UNTIL(pt, done);
        if (done) {
            break;
        }
    }

    if (tread_tid) delete_task(tread_tid);
    delete_task(tid);
    printf("%s: bye\n", __func__);
    PT_END(pt);
}

void schedule() {
    int n = 1;
    while (n > 0) {
        n = schedule_tasks();
        printf("%s: schedule_tasks returned %d\n", __func__, n);
    }
}

static char banner[] =
R"(hello\nworld\n
a b c)";
int main() {
    printf("%s\n", banner);
    printf("%s: hello\n", __func__);
    init_tasks();
    create_task(tinit, 0);
    schedule();
    printf("%s: bye\n", __func__);
    return 0;
}
