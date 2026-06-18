; CH56x ARM START CODE
; V1.01 2011.10
;

; configuration
SWI_USE_ASM     EQU     1           ;ASM code for SoftwareInterrupt
IRQ_USE_ASM     EQU     0           ;ASM code for IRQ_Handler
FIQ_USE_ASM     EQU     0           ;ASM code for FIQ_Handler
RESERVED_VECTOR EQU     0x00000042  ;reserved vector
;                                      if bit31:30=0:1, then
;                                        bit29=0 for 32K-CODE-RAM, bit29=1 for 64K/96K-CODE-RAM, default is 1
;                                        bit28=0 for disable ICE/debug, bit28=1 for enable ICE/debug, default is 0
;                                      if bit23:16=0xFF, then enable code ROM reading by external master, else disable
;
; define stack size
UND_STACK_SIZE  EQU     0x00000000  ;stack size for UndefineInstruction
ABT_STACK_SIZE  EQU     0x00000000  ;stack size for PrefetchAbort/DataAbort
SVC_STACK_SIZE  EQU     0x00000010  ;stack size for SoftwareInterrupt
IRQ_STACK_SIZE  EQU     0x00000100  ;stack size for IRQ_Handler
FIQ_STACK_SIZE  EQU     0x00000100  ;stack size for FIQ_Handler
SYS_STACK_SIZE  EQU     0x00000400  ;stack size for system/user
ALL_STACK_SIZE  EQU     (UND_STACK_SIZE+ABT_STACK_SIZE+SVC_STACK_SIZE+IRQ_STACK_SIZE+FIQ_STACK_SIZE+SYS_STACK_SIZE)
HEAP_SIZE       EQU     0x00000000  ;heap size
;
;***************************************************************************************************
;
                PRESERVE8
                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Top       SPACE   0x400
__initial_sp    EQU     0x81FC00           ; equal to the start of the stack
                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base                                ; equal to the start of the heap   
Heap_Bottom     SPACE   HEAP_SIZE
__heap_limit    EQU     0x81FC00,DATA      ; equal to the end of the heap

;
;***************************************************************************************************
;
                EXPORT  Reset
                EXPORT  __rt_div0
;
                AREA    RESET,CODE,READONLY
                ENTRY
                CODE32
;
Reset
                B       ResetInit
                B       UndefineInstruction
  IF SWI_USE_ASM = 0
                IMPORT  SoftwareInterrupt
  ENDIF
                B       SoftwareInterrupt
                B       PrefetchAbort
                B       DataAbort
                DCD     RESERVED_VECTOR
  IF IRQ_USE_ASM = 0
                IMPORT  IRQ_Handler
  ENDIF
                B       IRQ_Handler
  IF FIQ_USE_ASM = 0
                IMPORT  FIQ_Handler
                B       FIQ_Handler
  ELSE
;                B       FIQ_Handler
FIQ_Handler
                SUB     LR, LR, #4
                STMFD   SP!, {R0-R7, LR}
;                MRS     R1, SPSR
;                STMFD   SP!, {R1}
                BL      FIQ_Exception
;                LDMFD   SP!, {R1}
;                MSR     SPSR_CXSF, R1
                LDMFD   SP!, {R0-R7, PC}^
  ENDIF
;
UndefineInstruction
;                STMFD   SP!, {R0-R12, LR}
;;                MRS     R1, SPSR
;;                STMFD   SP!, {R1}
;                BL      UndefineInstrHand
;;                LDMFD   SP!, {R1}
;;                MSR     SPSR_CXSF, R1
;                LDMFD   SP!, {R0-R12, PC}^
                B       UndefineInstruction
;
PrefetchAbort
;                SUB     LR, LR, #4
;                STMFD   SP!, {R0-R12, LR}
;;                MRS     R1, SPSR
;;                STMFD   SP!, {R1}
;                BL      PrefetchAbortHand
;;                LDMFD   SP!, {R1}
;;                MSR     SPSR_CXSF, R1
;                LDMFD   SP!, {R0-R12, PC}^
                B       PrefetchAbort
;
DataAbort
;                SUB     LR, LR, #8
;                STMFD   SP!, {R0-R12, LR}
;;                MRS     R1, SPSR
;;                STMFD   SP!, {R1}
;                BL      DataAbortHand
;;                LDMFD   SP!, {R1}
;;                MSR     SPSR_CXSF, R1
;                LDMFD   SP!, {R0-R12, PC}^
                B       DataAbort
;
  IF SWI_USE_ASM <> 0
SoftwareInterrupt
;                STMFD   SP!, {R0-R12, LR}
;;                MRS     R1, SPSR
;;                STMFD   SP!, {R1}
;                BL      SoftwareInt
;;                LDMFD   SP!, {R1}
;;                MSR     SPSR_CXSF, R1
;                LDMFD   SP!, {R0-R12, PC}^
                B       SoftwareInterrupt
  ENDIF
;
  IF IRQ_USE_ASM <> 0
IRQ_Handler
                SUB     LR, LR, #4
                STMFD   SP!, {R0-R12, LR}
;                MRS     R1, SPSR
;                STMFD   SP!, {R1}
                BL      IRQ_Exception
;                LDMFD   SP!, {R1}
;                MSR     SPSR_CXSF, R1
                LDMFD   SP!, {R0-R12, PC}^
  ENDIF
;
;***************************************************************************************************
                IF      :DEF:__MICROLIB       ; use microlib   
                
                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap
                LDR     R0, = Heap_Bottom
                LDR     R1, = Stack_Top - ALL_STACK_SIZE + SYS_STACK_SIZE
                LDR     R2, = Heap_Bottom + HEAP_SIZE
                LDR     R3, = Stack_Top - ALL_STACK_SIZE
                BX      LR
 
                ENDIF
;***************************************************************************************************
__rt_div0
                B       __rt_div0
;***************************************************************************************************
ResetInit
                LDR     R0, =Stack_Top
                MSR     CPSR_c, #0xDB
                MOV     SP, R0
                SUB     R0, R0, #UND_STACK_SIZE
                MSR     CPSR_c, #0xD7
                MOV     SP, R0
                SUB     R0, R0, #ABT_STACK_SIZE
                MSR     CPSR_c, #0xD3
                MOV     SP, R0
                SUB     R0, R0, #SVC_STACK_SIZE
                MSR     CPSR_c, #0xD2
                MOV     SP, R0
                SUB     R0, R0, #IRQ_STACK_SIZE
                MSR     CPSR_c, #0xD1
                MOV     SP, R0
                SUB     R0, R0, #FIQ_STACK_SIZE
;                MSR     CPSR_c, #0xDF
                MSR     CPSR_c, #0x1F
                MOV     SP, R0
                SUB     SL, SP, #SYS_STACK_SIZE
                IMPORT  SysFreq
                BLX     SysFreq
                IMPORT  __main
                BLX     __main
MainExit
                B       MainExit
;***************************************************************************************************
                END
;
