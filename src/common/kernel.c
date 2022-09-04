#include "libc.h"
#include "vid.h"

#include "ioport.h"
#include "idt.h"

#include "tasks.h"

unsigned char stack[0x4000] asm("stack") = {
    [0] = 0xde,
    [sizeof(stack) - 1] = 0xef
};

static void panic(const char *s) {
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

#define KB_PORT 0x60
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

/*
scan codes set 1
Below are pressed scan codes
Release codes are pressed+0x80
Ignore extended codes ({0xe0,XX} and {0xe1,XX,YY})
*/
#define S_FIRST 0x01
#define S_ESC   0x01
#define S_1     0x02
#define S_2     0x03
#define S_3     0x04
#define S_4     0x05
#define S_5     0x06
#define S_6     0x07
#define S_7     0x08
#define S_8     0x09
#define S_9     0x0a
#define S_0     0x0b
//...
#define S_TAB   0x0f
#define S_Q     0x10
#define S_W     0x11
#define S_E     0x12
//...
#define S_A     0x1e
#define S_S     0x1f
#define S_D     0x20
//...
#define S_BTICK 0x29
//...
#define S_CAPSL 0x3a
#define S_F1    0x3b
#define S_F2    0x3c
#define S_F3    0x3d
//...
#define S_F11   0x57
#define S_F12   0x58
#define S_LAST  0x58
typedef struct {
    uint8_t kc;
    uint8_t ch;
} sc_t;
#define SC(kc, ch) (sc_t){kc, ch}
static sc_t sc_std[S_LAST - S_FIRST + 1];
/*
key codes are 8-bits, where high 3 bits are 'row', and 5 low bits are 'col'
row 1: esc f1 f2 f3 f4 f5 f6 f7 f8 f9 f10 f11 f12 prscr scrlk pause                     => 16 cols
row 2: btick 1 2 3 4 5 6 7 8 9 0 - = back ins home pgup numlk kpdiv kpmul kpmin         => 21 cols
row 3: tab q w e r t y u i o p [ ] enter del end pgdn kp7 kp8 kp9 kpplus                => 21 cols
row 4: capsl a s d f g h j k l ; ' \ kp4 kp5 kp6                                        => 16 cols
row 5: lshft < z x c v b n m , . / rshft up 1 2 3 kpent                                 => 18 cols
row 6: lctrl lwin alt space altgr rwin menu rctrl left down right kp0 kpdot             => 13 cols
                0bRRRCCCCC
*/
#define K(r,c) ((r << 5) + c)
#define K_ESC   K(0,0)
#define K_F1    K(0,1)
#define K_F2    K(0,2)
#define K_F3    K(0,3)
#define K_F4    K(0,4)
#define K_F5    K(0,5)
#define K_F6    K(0,6)
#define K_F7    K(0,7)
#define K_F8    K(0,8)
#define K_F9    K(0,9)
#define K_F10    K(0,10)
//...
#define K_BTICK K(1,0)
#define K_1     K(1,1)
#define K_2     K(1,2)
#define K_3     K(1,3)
#define K_4     K(1,4)
#define K_5     K(1,5)
#define K_6     K(1,6)
#define K_7     K(1,7)
#define K_8     K(1,8)
#define K_9     K(1,9)
#define K_0     K(1,10)
#define K_MINUS K(1,11)
#define K_EQUAL K(1,12)
#define K_BACK  K(1,13)
#define K_INS   K(1,14)
#define K_HOME  K(1,15)
#define K_PGUP  K(1,16)
#define K_NUMLK K(1,17)
#define K_KPDIV K(1,18)
#define K_KPMUL K(1,19)
#define K_KPMIN K(1,20)
//...
#define K_TAB   K(2,0)
#define K_Q     K(2,1)
#define K_W     K(2,2)
#define K_E     K(2,3)
#define K_R     K(2,4)
#define K_T     K(2,5)
#define K_Y     K(2,6)
#define K_U     K(2,7)
#define K_I     K(2,8)
#define K_O     K(2,9)
#define K_P     K(2,10)
#define K_LBRAK K(2,11)
#define K_RBRAK K(2,12)
#define K_ENTER K(2,13)
//...
#define K_CAPSL K(3,0)
#define K_A     K(3,1)
#define K_S     K(3,2)
#define K_D     K(3,3)
#define K_F     K(3,4)
#define K_G     K(3,5)
#define K_H     K(3,6)
#define K_J     K(3,7)
#define K_K     K(3,8)
#define K_L     K(3,9)
#define K_COL   K(3,10)
#define K_QUOTE K(3,11)
#define K_BSLSH K(3,12)
//...
#define K_LSHFT K(4,0)
#define K_INF   K(4,1)
#define K_Z     K(4,2)
#define K_X     K(4,3)
#define K_C     K(4,4)
#define K_V     K(4,5)
#define K_B     K(4,6)
#define K_N     K(4,7)
#define K_M     K(4,8)
#define K_COMMA K(4,9)
#define K_DOT   K(4,10)
#define K_SLASH K(4,11)
#define K_RSHFT K(4,12)
//...
#define K_LCTRL K(5,0)
#define K_LWIN  K(5,1)
#define K_ALT   K(5,2)
#define K_SPACE K(5,3)
//...
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
    sc_std[i++] = SC(K_BACK, -1);

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
    sc_std[i++] = SC(K_ENTER, 'E');

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

static uint8_t kbflags[256];       // 1: pressed 0: released/non-pressed
PT_THREAD(tkb(struct pt *pt, int tid, void *arg)) {
    PT_BEGIN(pt);
    static int startl = 0;
    startl = 6 + 3 * tid;
    kb_init();
    while (1) {
        static int count = 0;
        static int len = 0;
        static uint8_t kcodes[20];
        static sc_t k;

        gotoxy(0, startl + 0);
        printf("%s: waiting keypresses.. kc=%s kb=%p len=%d count=%d     \n", __func__, kcodes, kbhits, len, count);
        printf("%s: sc_std %x %x %x %x %x  kc=%x ch=%x   \n", __func__, sc_std[0], sc_std[1], sc_std[2], sc_std[3], sc_std[4], k.kc, k.ch);
        PT_WAIT_UNTIL(pt, kb_head != kb_tail);
        disable();
#if 0
        if (kb_head > kb_tail) {
            len = kb_head - kb_tail;
        } else {
            len = kb_tail - kb_head;
        }
        kb_tail = kb_head;
        count++;
#else
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
                    kbflags[(int)k.kc] = flags;
                    if (flags) {
                        if (k.ch != 0xff)
                            kcodes[len++] = k.ch;
                    }
                } else {
                    panic("Unknown scancode");
                }
            }
            kb_tail = (kb_tail + 1) % KB_SIZE;
            count++;
        }
#endif
        enable();
    }
    PT_END(pt);
}

PT_THREAD(t1(struct pt *pt, int tid, void *arg)) {
    PT_BEGIN(pt);
    static int startl = 0;
    startl = 6 + 3 * tid;

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

PT_THREAD(t2(struct pt *pt, int tid, void *arg)) {
    PT_BEGIN(pt);
    static int startl = 0;
    startl = 6 + 3 * tid;

    while (1) {
        static int count = 0;

        gotoxy(0, startl + 0);
        printf("%s: looping.. #%08lx jif=%08lx kb=%x divs=%x", __func__, (void *)(uintptr_t)count++, jiffies, kbhits, divs);
        PT_YIELD(pt);
    }
    PT_END(pt);
}

PT_THREAD(tshell(struct pt *pt, int tid, void *arg))
{
    PT_BEGIN(pt);
    static int startl;
    startl = 6 + 3 * tid;
    static int valid;
    static unsigned char buf[128];
    static char prompt[10];
    prompt[0] = '>';
    prompt[1] = ' ';
    prompt[2] = 0;
    memset(buf, ' ', sizeof(buf) - 2);
    buf[sizeof(buf) - 1] = 0;
    while (1) {
        if (!valid && kbhits >= 10)
            valid++;
        gotoxy(0, startl + 0);
        if (valid) {
            printf("%s: buf validated (valid=%d) [%s]\n", __func__, valid, buf);
        } else {
            //printf("%s: %s%s (valid=%d)", __func__, prompt, buf, valid);
            printf("%s: (valid=%d) %s%s", __func__, valid, prompt, buf);
//            printf("%s: (valid=%d)", __func__, valid);
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
    create_task(t1, 0);
    create_task(t2, 0);
    create_task(tkb, 0);
    create_task(tshell, 0);

    schedule();                // schedule tasks forever..
}
