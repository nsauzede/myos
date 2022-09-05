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
#define BLACK 0x0
#define GREEN 0x2
#define BROWN 0x6
#define LRED 0xc
#define WHITE 0xf

#define BG_FG(bg, fg) ((bg << 4) | (fg))

#define DEFAULT_BG BLACK
#define DEFAULT_FG GREEN

#define DEFAULT_ATTR    BG_FG(DEFAULT_BG, DEFAULT_FG)

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
