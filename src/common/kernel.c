#include "libc.h"
#include "vid.h"

#include "ioport.h"
#include "idt.h"
#include "kb.h"

#include "tasks.h"

static char version[] = "0.1";

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
static sc_t sc1_std[S_LAST - S_FIRST + 1];

// TODO: remove below kb_init once static sc1_std initializer bytes is honored by the loader (??)
void kb_init() {
    int i = 0;
    memset(sc1_std, 0xff, sizeof(sc1_std));
    sc1_std[i++] = SC(K_ESC, -1);
    sc1_std[i++] = SC(K_1, '1');
    sc1_std[i++] = SC(K_2, '2');
    sc1_std[i++] = SC(K_3, '3');
    sc1_std[i++] = SC(K_4, '4');
    sc1_std[i++] = SC(K_5, '5');
    sc1_std[i++] = SC(K_6, '6');
    sc1_std[i++] = SC(K_7, '7');
    sc1_std[i++] = SC(K_8, '8');
    sc1_std[i++] = SC(K_9, '9');
    sc1_std[i++] = SC(K_0, '0');
    sc1_std[i++] = SC(K_MINUS, '-');
    sc1_std[i++] = SC(K_EQUAL, '=');
    sc1_std[i++] = SC(K_BACKS, -1);

    sc1_std[i++] = SC(K_TAB, '\t');
    sc1_std[i++] = SC(K_Q, 'q');
    sc1_std[i++] = SC(K_W, 'w');
    sc1_std[i++] = SC(K_E, 'e');
    sc1_std[i++] = SC(K_R, 'r');
    sc1_std[i++] = SC(K_T, 't');
    sc1_std[i++] = SC(K_Y, 'y');
    sc1_std[i++] = SC(K_U, 'u');
    sc1_std[i++] = SC(K_I, 'i');
    sc1_std[i++] = SC(K_O, 'o');
    sc1_std[i++] = SC(K_P, 'p');
    sc1_std[i++] = SC(K_LBRAK, '[');
    sc1_std[i++] = SC(K_RBRAK, ']');
    sc1_std[i++] = SC(K_ENTER, '\n');

    sc1_std[i++] = SC(K_LCTRL, -1);
    sc1_std[i++] = SC(K_A, 'a');
    sc1_std[i++] = SC(K_S, 's');
    sc1_std[i++] = SC(K_D, 'd');
    sc1_std[i++] = SC(K_F, 'f');
    sc1_std[i++] = SC(K_G, 'g');
    sc1_std[i++] = SC(K_H, 'h');
    sc1_std[i++] = SC(K_J, 'j');
    sc1_std[i++] = SC(K_K, 'k');
    sc1_std[i++] = SC(K_L, 'l');
    sc1_std[i++] = SC(K_COL, ';');
    sc1_std[i++] = SC(K_QUOTE, '\'');
    sc1_std[i++] = SC(K_BTICK, '`');

    sc1_std[i++] = SC(K_LSHFT, -1);
    sc1_std[i++] = SC(K_BSLSH, '\\');
    sc1_std[i++] = SC(K_Z, 'z');
    sc1_std[i++] = SC(K_X, 'x');
    sc1_std[i++] = SC(K_C, 'c');
    sc1_std[i++] = SC(K_V, 'v');
    sc1_std[i++] = SC(K_B, 'b');
    sc1_std[i++] = SC(K_N, 'n');
    sc1_std[i++] = SC(K_M, 'm');
    sc1_std[i++] = SC(K_COMMA, ',');
    sc1_std[i++] = SC(K_DOT, '.');
    sc1_std[i++] = SC(K_SLASH, '/');
    sc1_std[i++] = SC(K_RSHFT, -1);
    sc1_std[i++] = SC(K_KPMUL, '*');

    sc1_std[i++] = SC(K_ALT, -1);
    sc1_std[i++] = SC(K_SPACE, ' ');
    sc1_std[i++] = SC(K_CAPSL, -1);

    sc1_std[i++] = SC(K_F1, -1);
    sc1_std[i++] = SC(K_F2, -1);
    sc1_std[i++] = SC(K_F3, -1);
    sc1_std[i++] = SC(K_F4, -1);
    sc1_std[i++] = SC(K_F5, -1);
    sc1_std[i++] = SC(K_F6, -1);
    sc1_std[i++] = SC(K_F7, -1);
    sc1_std[i++] = SC(K_F8, -1);
    sc1_std[i++] = SC(K_F9, -1);
    sc1_std[i++] = SC(K_F10, -1);

    sc1_std[i++] = SC(K_NUMLK, -1);
    sc1_std[i++] = SC(K_SCRLK, -1);

    sc1_std[i++] = SC(K_KP7, '7');
    sc1_std[i++] = SC(K_KP8, '8');
    sc1_std[i++] = SC(K_KP9, '9');
    sc1_std[i++] = SC(K_KPMIN, '-');

    sc1_std[i++] = SC(K_KP4, '4');
    sc1_std[i++] = SC(K_KP4, '5');
    sc1_std[i++] = SC(K_KP6, '6');
    sc1_std[i++] = SC(K_KPPLU, '+');

    sc1_std[i++] = SC(K_KP1, '1');
    sc1_std[i++] = SC(K_KP2, '2');
    sc1_std[i++] = SC(K_KP3, '3');
    sc1_std[i++] = SC(K_KP0, '0');
    sc1_std[i++] = SC(K_KPDOT, '.');

    sc1_std[i++] = SC(0xfa, 'a');
    sc1_std[i++] = SC(0xfb, 'b');
    sc1_std[i++] = SC(K_BSLSH, '\\');

    sc1_std[i++] = SC(K_F11, -1);
    sc1_std[i++] = SC(K_F12, -1);
};

static int stdin_tail = 0;
static int stdin_head = 0;
#define STDIN_SIZE 40
static char stdin_ring[STDIN_SIZE];
static int klen = 0;
#define MAXK 20
static uint8_t kcodes[MAXK];
static uint8_t kflags[256];       // 1: pressed 0: released/non-pressed

void sh_help() {
    printf("help        Show this help\n");
    printf("version     Show version\n");
}

void sh_version() {
    printf("MyOS %s\n", version);
}

void shell(char *s, int len) {
    cls();
//    printf("%s: s=[%s] len=%d\n", __func__, s, len);
    if (!strncmp(s, "help", len)) {
        sh_help();
    } else if (!strncmp(s, "version", len)) {
        sh_version();
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
        gotoxy(0, startl + 2);
        printf("shell> %s ", kcodes);
        static int klen_;
        PT_WAIT_UNTIL(pt, klen_ != klen || stdin_head != stdin_tail);
        if (klen_ != klen) {
            klen_ = klen;
            continue;
        }
        stdin = &stdin_ring[stdin_tail];
        if (stdin_head > stdin_tail) {
            stdin_len = stdin_head - stdin_tail;
        } else {
            stdin_len = stdin_tail - stdin_head;
        }
        gotoxy(0, startl + 3);
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
        printf("%s: kc=%x ch=%x ovf=%d  \n", __func__, k.kc, k.ch, ovf);
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
                    k = sc1_std[sc - S_FIRST];
                    if (k.kc == 0xff) panic("Invalid keycode");
                    kflags[(int)k.kc] = flags;
                    if (flags) {
                        if (k.kc == K_BACKS && klen > 0)
                            kcodes[--klen] = 0;
                        else if (k.ch == '\n') {
                            static int i;
                            for (i = 0; i < klen; i++) {
                                stdin_ring[stdin_head % STDIN_SIZE] = kcodes[i];
                                kcodes[i] = 0;
                                stdin_head = (stdin_head + 1) % STDIN_SIZE;
                                if (stdin_head == stdin_tail) {
                                    panic("stdin overflow");
                                }
                            }
                            klen = 0;
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
        gotoxy(0, 2);             // leave first lines for interrupts
        printf("Hello kernel%d - Copyright (C) Nicolas Sauzede 2009-2022.\n", sizeof(void *) * 8);
        halt();
        schedule_tasks();
    }
}

void kernel_main() asm("kernel_main");
void kernel_main() {
    console_init();             // this will initialize internal console data for subsequent printouts


    idt_setup();                // init idt with default int handlers

    exception_set_handler(EXCEPTION_DIVIDE, div_handler);
    exception_set_handler(EXCEPTION_DIVIDE, (idt_handler_t)int_handler);

    irq_setup();
    irq_set_handler(IRQ_KB, kb_handler);
    irq_set_handler(IRQ_TIMER, timer_handler);
    i8254_set_freq(1);

    setcursor(0, 0);
    enable();

    init_tasks();
    create_task(tints, 0);      // dummy task to show jiffies, keypresses etc..
    create_task(tkb, 0);        // mandatory task to process kb buffer
    create_task(tshell, 0);

    schedule();                // schedule tasks forever..
}
