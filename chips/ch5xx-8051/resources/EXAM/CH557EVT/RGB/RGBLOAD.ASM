; RGB PWM LED interrupt
;
$NOMOD51
#include "CH557.INC"
;
NAME	REGINT
;
				CSEG	AT	INT_ADDR_PWM_I2C	;RGB LED interrupt
				JMP     LED_INT
;
;
?PR?LED_INT?REGINT		SEGMENT CODE
;
EXTRN	BIT		(IsCode)
EXTRN	DATA	(BufferAddr)
;PUBLIC			LED_INT
;
				RSEG	?PR?LED_INT?REGINT
LED_INT:
				PUSH  PSW
				PUSH  ACC
;				MOV   A,I2CX_INT
;				JNB   ACC.1,SKIP_LED_INT	;check interrupt request, not RGB LED
				INC   LED_COMMON	;switch COMMON, clear interrupt flag
				MOV   A,LED_COMMON
				JNZ   LED_INT_COM	;next COMMON
				MOV   LED_COMMON,#10H	;restart
				MOV   LED_DMA_L,BufferAddr+1	;reset buffer start address
				MOV   LED_DMA_H,BufferAddr
LED_INT_COM:	JB    IsCode,LED_INT_CODE	;load data from ROM
;				MOV   A,R7			;load data from XRAM
;				PUSH  ACC
;				MOV   R7,#32/8		;total 32bytes, PWM pins[0~7]*4(intensity,R,G,B)
				SETB  E_DIS			;disable global interrupt for RGB LED DMA
				SETB  P				;copy LED_DMA into DPTR1
LED_INT_XRAM:
				MOVX  A,@DPTR		;load 1 byte for PWM pin[0] intensity
				MOVX  A,@DPTR		;load 1 byte for PWM pin[0] R
				MOVX  A,@DPTR		;load 1 byte for PWM pin[0] G
				MOVX  A,@DPTR		;load 1 byte for PWM pin[0] B
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				NOP               	;reserved for DMA       
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				NOP               	;reserved for DMA       
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				NOP               	;reserved for DMA       
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR		;load 1 byte for PWM pin[7] intensity
				MOVX  A,@DPTR
				MOVX  A,@DPTR
				MOVX  A,@DPTR
;				DJNZ  R7,LED_INT_XRAM
				CLR   E_DIS
				MOV   LED_PWM_OE,#0FFH	;enable PWM output
;				POP   ACC
;				MOV   R7,A
SKIP_LED_INT:	POP   ACC
				POP   PSW
				RETI
LED_INT_CODE:
				CLR   A				;for MOVC only
				SETB  E_DIS			;disable global interrupt for RGB LED DMA
				SETB  P				;copy LED_DMA into DPTR1
				MOVC  A,@A+DPTR		;load 4 bytes (intensity,R,G,B) for PWM pin[0]
				MOVC  A,@A+DPTR		;load 4 bytes
				MOVC  A,@A+DPTR		;load 4 bytes
				MOVC  A,@A+DPTR		;load 4 bytes
				MOVC  A,@A+DPTR		;load 4 bytes
				MOVC  A,@A+DPTR		;load 4 bytes
				MOVC  A,@A+DPTR		;load 4 bytes
				MOVC  A,@A+DPTR		;load 4 bytes for PWM pin[7]
				CLR   E_DIS
				MOV   LED_PWM_OE,#0FFH	;enable PWM output
				POP   ACC
				POP   PSW
				RETI
;
END
