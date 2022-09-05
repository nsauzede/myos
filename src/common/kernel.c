#include "libc.h"
#include "vid.h"

#include "ioport.h"
#include "idt.h"
#include "kb.h"

#include "tasks.h"

static char shell_version[] = "shell version 0.1";

unsigned char stack[0x4000] asm("stack") = {
    [0] = 0xde,
    [sizeof(stack) - 1] = 0xef
};

void panic(const char *s) {
    disable();
    home();
    setattr(BG_BLACK | FG_RED);
    puts(s);
    while (1) {
        halt();
    }
}

static void int_handler( int id, uint32_t _ebp,
    uint32_t gs, uint32_t fs, uint32_t es, uint32_t ds, uint32_t ss,
    uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp,
    uint32_t code, uint32_t eip, uint32_t cs, uint32_t eflags)
{
    gotoxy(0, 1);
    printf("int#%p: ", id);
    printf("code=%x eip=%x cs=%x eflags=%x ", code, eip, cs, eflags);
    printf("gs=%x fs=%x es=%x ds=%x ss=%x ", gs, fs, es, ds, ss);
    printf("\n");
    printf("eax=%x ebx=%x ecx=%x edx=%x esi=%x edi=%x ebp=%x\n", eax, ebx, ecx, edx, esi, edi, ebp);
    setcursor(0, 0);
    panic(__func__);
}

static int jiffies = 0;
static int kbhits = 0;
static int divs = 0;

static void timer_handler(int id, uint32_t ebp) {
    jiffies++;
}

#define KB_SIZE 20
static unsigned char kb_ring[KB_SIZE];
static int kb_head = 0;
static int kb_tail = 0;
static void kb_handler( int id, uint32_t ebp) {
    unsigned char byte;
    byte = inb(KB_PORT);
    kb_ring[kb_head % KB_SIZE] = byte;
    kb_head = (kb_head + 1) % KB_SIZE;
    if (kb_head == kb_tail) {
        panic("kb_handler overflow");
    }
    kbhits++;
}

static void div_handler(int id, uint32_t ebp) {
    divs++;
}

typedef struct {
    uint8_t kc;
    uint8_t ch;
} sc_t;

#define SC(kc, ch) (sc_t){kc, ch}
static sc_t sc_std[S_LAST - S_FIRST + 1];

// TODO: remove below kb_init once static sc_std initializer bytes is honored by the loader (??)
void kb_init() {
    int i = 0;
    memset(sc_std, 0xff, sizeof(sc_std));
    sc_std[i++] = SC(K_ESC, -1);
    sc_std[i++] = SC(K_1, '1');
    sc_std[i++] = SC(K_2, '2');
    sc_std[i++] = SC(K_3, '3');
    sc_std[i++] = SC(K_4, '4');
    sc_std[i++] = SC(K_5, '5');
    sc_std[i++] = SC(K_6, '6');
    sc_std[i++] = SC(K_7, '7');
    sc_std[i++] = SC(K_8, '8');
    sc_std[i++] = SC(K_9, '9');
    sc_std[i++] = SC(K_0, '0');
    sc_std[i++] = SC(K_MINUS, '-');
    sc_std[i++] = SC(K_EQUAL, '=');
    sc_std[i++] = SC(K_BACKS, -1);

    sc_std[i++] = SC(K_TAB, 'T');
    sc_std[i++] = SC(K_Q, 'q');
    sc_std[i++] = SC(K_W, 'w');
    sc_std[i++] = SC(K_E, 'e');
    sc_std[i++] = SC(K_R, 'r');
    sc_std[i++] = SC(K_T, 't');
    sc_std[i++] = SC(K_Y, 'y');
    sc_std[i++] = SC(K_U, 'u');
    sc_std[i++] = SC(K_I, 'i');
    sc_std[i++] = SC(K_O, 'o');
    sc_std[i++] = SC(K_P, 'p');
    sc_std[i++] = SC(K_LBRAK, '[');
    sc_std[i++] = SC(K_RBRAK, ']');
    sc_std[i++] = SC(K_ENTER, '\n');

    sc_std[i++] = SC(K_LCTRL, -1);
    sc_std[i++] = SC(K_A, 'a');
    sc_std[i++] = SC(K_S, 's');
    sc_std[i++] = SC(K_D, 'd');
    sc_std[i++] = SC(K_F, 'f');
    sc_std[i++] = SC(K_G, 'g');
    sc_std[i++] = SC(K_H, 'h');
    sc_std[i++] = SC(K_J, 'j');
    sc_std[i++] = SC(K_K, 'k');
    sc_std[i++] = SC(K_L, 'l');
    sc_std[i++] = SC(K_COL, ';');
    sc_std[i++] = SC(K_QUOTE, '\'');
    sc_std[i++] = SC(K_BTICK, '`');

    sc_std[i++] = SC(K_LSHFT, -1);
    sc_std[i++] = SC(K_BSLSH, '\\');
    sc_std[i++] = SC(K_Z, 'z');
    sc_std[i++] = SC(K_X, 'x');
    sc_std[i++] = SC(K_C, 'c');
    sc_std[i++] = SC(K_V, 'v');
    sc_std[i++] = SC(K_B, 'b');
    sc_std[i++] = SC(K_N, 'n');
    sc_std[i++] = SC(K_M, 'm');
    sc_std[i++] = SC(K_COMMA, ',');
    sc_std[i++] = SC(K_DOT, '.');
    sc_std[i++] = SC(K_SLASH, '/');
    sc_std[i++] = SC(K_RSHFT, -1);
    sc_std[i++] = SC(K_KPMUL, '*');

    sc_std[i++] = SC(K_ALT, -1);
    sc_std[i++] = SC(K_SPACE, ' ');
    sc_std[i++] = SC(K_CAPSL, ' ');

    sc_std[i++] = SC(K_F1, '1');
    sc_std[i++] = SC(K_F2, '2');
    sc_std[i++] = SC(K_F3, '3');
    sc_std[i++] = SC(K_F4, '4');
    sc_std[i++] = SC(K_F5, '5');
    sc_std[i++] = SC(K_F6, '6');
    sc_std[i++] = SC(K_F7, '7');
    sc_std[i++] = SC(K_F8, '8');
    sc_std[i++] = SC(K_F9, '9');
    sc_std[i++] = SC(K_F10, 'a');

    sc_std[i++] = SC(K_INS, -1);
    sc_std[i++] = SC(K_HOME, -1);
    sc_std[i++] = SC(K_PGUP, -1);
    sc_std[i++] = SC(K_NUMLK, -1);
    sc_std[i++] = SC(K_KPDIV, '/');
    sc_std[i++] = SC(K_KPMUL, '*');
    sc_std[i++] = SC(K_KPMIN, '-');
//    sc_std[i++] = S_3;
};

static int stdin_tail = 0;
static int stdin_head = 0;
#define STDIN_SIZE 1024
static char stdin_ring[STDIN_SIZE];
static int klen = 0;
#define MAXK 20
static uint8_t kcodes[MAXK];
static uint8_t kflags[256];       // 1: pressed 0: released/non-pressed

void help() {
    printf("help        Show this help\n");
    printf("version     Show shell version\n");
}

void version() {
    printf("%s\n", shell_version);
}

void shell(char *s, int len) {
    cls();
//    printf("%s: s=[%s] len=%d\n", __func__, s, len);
    if (!strncmp(s, "help", len)) {
        help();
    } else if (!strncmp(s, "version", len)) {
        version();
    } else {
        //panic("invalid shell command");
        printf("%s: invalid shell command [%s]\n", __func__, s);
        printf("%s: try [help]\n", __func__);
    }
}

int tid_start(int tid) {
    return 4 + 3 * tid;
}

PT_THREAD(tshell(struct pt *pt, int tid, void *arg))
{
    PT_BEGIN(pt);
    static int startl;
    startl = tid_start(tid);
    static char *stdin;
    static int stdin_len;
    while (1) {
        gotoxy(0, startl + 0);
        printf("%s: (stdin=%p len=%d tail=%d head=%d) [%s]\n", __func__, stdin, stdin_len, stdin_tail, stdin_head, stdin_ring);
        PT_WAIT_UNTIL(pt, stdin_head != stdin_tail);
        stdin = &stdin_ring[stdin_tail];
        if (stdin_head > stdin_tail) {
            stdin_len = stdin_head - stdin_tail;
        } else {
            stdin_len = stdin_tail - stdin_head;
        }
        gotoxy(0, startl + 1);
        shell(stdin, stdin_len);
        stdin_tail = stdin_head;
    }
    PT_END(pt);
}

PT_THREAD(tkb(struct pt *pt, int tid, void *arg)) {
    PT_BEGIN(pt);
    static int startl = 0;
    startl = tid_start(tid);
    kb_init();
    while (1) {
        static int count = 0;
        static int ovf = 0;
        static sc_t k;

        gotoxy(0, startl + 0);
        printf("%s: waiting keypresses.. kc=[%s] kb=%p klen=%d count=%d     \n", __func__, kcodes, kbhits, klen, count);
        printf("%s: sc_std %x %x %x %x %x  kc=%x ch=%x ovf=%d  \n", __func__, sc_std[0], sc_std[1], sc_std[2], sc_std[3], sc_std[4], k.kc, k.ch, ovf);
        PT_WAIT_UNTIL(pt, kb_head != kb_tail);
        disable();
        static int skip;
        skip = 0;
        while (kb_tail != kb_head) {
            if (skip) {
                skip--;
            } else {
                static uint8_t flags;
                static uint8_t sc;
                sc = kb_ring[kb_tail];
                if (sc & 0x80) {
                    flags = 0;
                } else {
                    flags = 1;
                }
                sc &= ~0x80;
                if (sc == (0xe0 & ~0x80)) {
                    skip = 1;
                } else if (sc == (0xe1 & ~0x80)) {
                    skip = 2;
                } else if (sc >= S_FIRST && sc <= S_LAST) {
                    k = sc_std[sc - S_FIRST];
                    if (k.kc == 0xff) panic("Invalid keycode");
                    kflags[(int)k.kc] = flags;
                    if (flags) {
                        if (k.kc == K_BACKS && klen > 0)
                            kcodes[--klen] = 0;
                        else if (k.ch == '\n') {
                            static int i;
                            for (i = 0; i < klen; i++) {
                                stdin_ring[stdin_head % STDIN_SIZE] = kcodes[i];
                                stdin_head = (stdin_head + 1) % STDIN_SIZE;
                                if (stdin_head == stdin_tail) {
                                    panic("stdin overflow");
                                }
                            }
                            klen = 0;
                            kcodes[0] = 0;
                        }
                        else if (klen >= MAXK - 1)
                            ovf++;
                            //panic("tkb overflow");
                        else if (k.ch != 0xff) {
                            kcodes[klen++] = k.ch;
                        }
                    }
                } else {
                    panic("Unknown scancode");
                }
            }
            kb_tail = (kb_tail + 1) % KB_SIZE;
            count++;
        }
        enable();
    }
    PT_END(pt);
}

PT_THREAD(tints(struct pt *pt, int tid, void *arg)) {
    PT_BEGIN(pt);
    static int startl = 0;
    startl = tid_start(tid);

    while (1) {
        static int count = 0;
        static int div = 0;

        gotoxy(0, startl + 0);
        printf("%s: looping.. #%08lx jif=%08lx kb=%p divs=%p div=%x", __func__, (void *)(uintptr_t)count++, jiffies, kbhits, divs, div);
        gotoxy(0, startl + 1);
        static int j;
        for (j = 0; j < KB_SIZE; j++) {
            printf("%c%02x%c", j == kb_head ? '>' : ' ', kb_ring[j], j == kb_head ? '<' : ' ');
        }
        PT_YIELD(pt);
    }
    PT_END(pt);
}

static void schedule() {
    while (1) {
        static int count = 0;
        gotoxy(0, 3);
        printf("%s: having some halt.. %x\n", __func__, count++);
        halt();
        schedule_tasks();
    }
}

void kernel_main() asm("kernel_main");
void kernel_main() {
    console_init();             // this will initialize internal console data for subsequent printouts

    printf("\n\n");             // leave first lines for interrupts
    printf("hello kernel%d - kernel_main=[%p]\n", sizeof(void *) * 8, kernel_main);

    idt_setup();                // init idt with default int handlers

    exception_set_handler(EXCEPTION_DIVIDE, div_handler);
    exception_set_handler(EXCEPTION_DIVIDE, (idt_handler_t)int_handler);

    irq_setup();
    irq_set_handler(IRQ_KB, kb_handler);
    irq_set_handler(IRQ_TIMER, timer_handler);
    i8254_set_freq(1);

    enable();

    init_tasks();
    create_task(tints, 0);
    create_task(tkb, 0);
    create_task(tshell, 0);

    schedule();                // schedule tasks forever..
}
