bits 32

global _asm_setmode03
global _asm_setmode13

_mode3:
	db 67H,00H,03H,00H,03H,00H,02H
	db 5FH,4FH,50H,82H,55H,81H,0BFH,1FH,00H,4FH,0EH,0FH,00H,00H,00H,00H
	db 9CH,0EH,8FH,28H,01H,96H,0B9H,0A3H,0FFH
	DB 00H,00H,00H,00H,00H,10H,0EH,00H,0FFH
	DB 00H,01H,02H,03H,04H,05H,06H,07H,08H,09H,0AH,0BH,0CH,0DH,0EH,0FH
	DB 0CH,00H,0FH,08H,00H
_mode13h:
	db 63H, 00H,  03H,01H,0FH,00H,0EH
	db 5FH,4FH,50H,82H,54H,80H,0BFH,1FH,00H,41H,00H,00H,00H,00H,00H,00H
        DB 9CH,0EH,8FH,28H,40H,96H,0B9H,0A3H,0FFH
        DB 00H,00H,00H,00H,00H,40H,05H,0FH,0FFH
        DB 00H,01H,02H,03H,04H,05H,06H,07H,08H,09H,0AH,0BH,0CH,0DH,0EH,0FH
        DB 41H,00H,0FH,00H,00H

%define MISC_ADDR	03C2H
%define STATUS_ADDR	03DAH
%define SEQ_ADDR	03C4H
%define CRTC_ADDR	03D4H
%define GRACON_ADDR	03CEH
%define ATTRCON_ADDR	03C0H

;MISC_ADDR         EQU       03C2H^
;VGAENABLE_ADDR    EQU       03C3H^

%define IODELAY times 8 nop

showcursor:
;	mov dx,CRCT_INDEX
;	mov al,10
;	out dx,al
;	mov dx,CRTC_DATA
;	IODELAY
;	in al,dx
;	and al,~0x20
;	out dx,al
;	IODELAY
	ret

_asm_setmode03:
	mov si,_mode3
	call _setmode
	call _setfont
	ret

_asm_setmode13:
	mov si,_mode13h
	call _setmode
	ret

oldmode db 0
oldmisc db 0
oldmask db 0
oldmem db 0

_setfont:
 mov dx,GRACON_ADDR                     ;get graphics port^
 mov al,5                               ;get write mode reg^
 out dx,al                              ;select the reg^
 IODELAY                                ;delay a bit^
 inc dx                                 ;change DX^
 in al,dx                               ;get value^
 IODELAY                                ;pause^
 mov [oldmode],al                       ;store it^
 dec dx                                 ;restore DX^
 mov al,6                               ;get misc reg^
 out dx,al                              ;select the reg^
 IODELAY                                ;delay a bit^
 inc dx                                 ;change DX^
 in al,dx                               ;get value^
 IODELAY                                ;pause^
 mov [oldmisc],al                       ;store it^
 dec dx                                 ;restore DX^
 mov dx,SEQ_ADDR                        ;get sequencer port^
 mov al,2                               ;get map mask reg^
 out dx,al                              ;select the reg^
 IODELAY                                ;delay a bit^
 inc dx                                 ;change DX^
 in al,dx                               ;get value^
 IODELAY                                ;pause^
 mov [oldmask],al                       ;store it^
 dec dx                                 ;restore DX^
 mov al,4                               ;get memory selector reg^
 out dx,al                              ;select the reg^
 IODELAY                                ;delay a bit^
 inc dx                                 ;change DX^
 in al,dx                               ;get value^
 IODELAY                                ;pause^
 mov [oldmem],al                        ;store it^
 mov dx,GRACON_ADDR                     ;select graphics port^
 mov al,5                               ;get write mode reg^
 mov ah,[oldmode]                       ;get old value^
 and ah,0fch                            ;mask it^
 out dx,ax                              ;set new value^
 IODELAY                                ;pause^
 mov al,6                               ;get misc reg^
 mov ah,[oldmisc]                       ;get old value^
 and ah,0f1h                            ;mask it^
 or ah,4                                ;set a flag^
 out dx,ax                              ;set it^
 IODELAY                                ;pause^
 mov dx,SEQ_ADDR                        ;select sequencer port^
 mov al,2                             ;get mask reg^
 mov ah,4                               ;get new value^
 out dx,ax                              ;set it^
 IODELAY                                ;pause^
 mov al,4                              ;get memory reg^
 mov ah,[oldmem]                        ;get old value^
 or ah,4                                ;set flag^
 out dx,ax                              ;set value^
 IODELAY                                ;pause^

	ret


_setmode:
mov dx,MISC_ADDR
mov al,[si]
out dx,al
IODELAY
inc si
   MOV DX,STATUS_ADDR
   MOV AL,[SI]
   OUT DX,AL
   IODELAY
   INC SI

   ; Send SEQ regs
   MOV CX,0
REG_LOOP:
   MOV DX,SEQ_ADDR
   MOV AL,CL
   OUT DX,AL
   IODELAY

   MOV DX,SEQ_ADDR
   INC DX
   MOV AL,[SI]
   OUT DX,AL
   IODELAY
   INC SI
   INC CX
   CMP CL,5
   JL REG_LOOP

   ; Clear Protection bits
   MOV AH,0EH
   MOV AL,11H
   AND AH,7FH
   MOV DX,CRTC_ADDR
   OUT DX,AX
   IODELAY

   ; Send CRTC regs
   MOV CX,0
REG_LOOP2:
   MOV DX,CRTC_ADDR
   MOV AL,CL
   OUT DX,AL
   IODELAY

   MOV DX,CRTC_ADDR
   INC DX
   MOV AL,[SI]
   OUT DX,AL
   IODELAY

   INC SI
   INC CX
   CMP CL,25
   JL REG_LOOP2

   ; Send GRAPHICS regs
   MOV CX,0
REG_LOOP3:
   MOV DX,GRACON_ADDR
   MOV AL,CL
   OUT DX,AL
   IODELAY

   MOV DX,GRACON_ADDR
   INC DX
   MOV AL,[SI]
   OUT DX,AL
   IODELAY

   INC SI
   INC CX
   CMP CL,9
   JL REG_LOOP3

   MOV DX,STATUS_ADDR
   IN AL,DX
   IODELAY

   ; Send ATTRCON regs
   MOV CX,0
REG_LOOP4:
   MOV DX,ATTRCON_ADDR
   IN AX,DX

   MOV AL,CL
   OUT DX,AL
   IODELAY

   MOV AL,[SI]
   OUT DX,AL
   IODELAY

   INC SI
   INC CX
   CMP CL,21
   JL REG_LOOP4

   MOV AL,20H
   OUT DX,AL
   IODELAY

ret
