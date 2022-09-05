#ifndef KB_H_
#define KB_H_

#define KB_PORT 0x60

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
#define S_MINUS 0x0c
#define S_EQUAL 0x0d
#define S_BACKS 0x0e

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
#define K_F10   K(0,10)
#define K_F11   K(0,11)
#define K_F12   K(0,12)
#define K_PRSCR K(0,13)
#define K_SCRLK K(0,14)
#define K_PAUSE K(0,15)

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
#define K_BACKS K(1,13)
#define K_INS   K(1,14)
#define K_HOME  K(1,15)
#define K_PGUP  K(1,16)
#define K_NUMLK K(1,17)
#define K_KPDIV K(1,18)
#define K_KPMUL K(1,19)
#define K_KPMIN K(1,20)

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
#define K_DELET K(2,14)
#define K_END   K(2,15)
#define K_PGDN  K(2,16)
#define K_KP7   K(2,17)
#define K_KP8   K(2,18)
#define K_KP9   K(2,19)
#define K_KPPLU K(2,20)

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
#define K_KP4   K(3,13)
#define K_KP5   K(3,14)
#define K_KP6   K(3,15)

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
#define K_UP    K(4,13)
#define K_KP1   K(4,14)
#define K_KP2   K(4,15)
#define K_KP3   K(4,16)
#define K_KPENT K(4,17)

#define K_LCTRL K(5,0)
#define K_LWIN  K(5,1)
#define K_ALT   K(5,2)
#define K_SPACE K(5,3)
#define K_ALTGR K(5,3)
#define K_RWIN  K(5,3)
#define K_MENU  K(5,3)
#define K_RCTRL K(5,3)
#define K_LEFT  K(5,3)
#define K_DOWN  K(5,3)
#define K_RIGHT K(5,3)
#define K_KP0   K(5,3)
#define K_KPDOT K(5,3)
//...

#endif/*KB_H_*/
