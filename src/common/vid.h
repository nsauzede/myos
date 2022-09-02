#ifndef __VID_H__
#define __VID_H__

#define MAX_ROW 25
#define MAX_COL 80

/*
0 black
1 blue dark
2 green dark
3 blue light
4 red dark
5 pink
6 brown light
7 grey light
8 dark grey
9 blue mid
a green light
b turquoise
c red light
d pink light
e yellow
f white
*/
#define BLACK 0
#define RED 0xC
#define GREEN 0x2
#define BG_BLACK (BLACK << 4)
#define FG_RED (RED)
#define FG_GREEN (GREEN)
#define DEFAULT_ATTR    (BG_BLACK | FG_GREEN)

void home();
void gotoxy( int x, int y);
void setcursor( int x, int y);
void setmode03( void);
void setmode13( void);
void setattr( int _attr);
void console_init();
void cls();
int dputchar( int c);
void dputs( const char *s);

#endif/*__VID_H__*/
