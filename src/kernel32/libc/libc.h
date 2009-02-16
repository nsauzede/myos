
typedef unsigned long size_t;

#define memset mmemset
void *mmemset( void *_s, int c, size_t n);

void console_init();
void cls();

#define puts dputs
void dputs( const char *s);

#define printf mprintf
int mprintf( const char *fmt, ...);
