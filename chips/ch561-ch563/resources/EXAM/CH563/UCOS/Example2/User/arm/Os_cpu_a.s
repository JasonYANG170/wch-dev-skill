;
;********************************************************************************************************
;                                                uC/OS-II
;                                          The Real-Time Kernel
;                                      primary task switch asm code
;
;                          (c) Copyright 1992-2001, Jean J. Labrosse, Weston, FL
;                                           All Rights Reserved
;
; File : os_cpu_a.s
; By   : Jean J. Labrosse
;********************************************************************************************************

                PRESERVE8
                

        AREA  UCOS_ARM, CODE, READONLY

;*********************************************************************************************************
;                                          START MULTITASKING
;                                       void OSStartHighRdy(void)
;
; Note : OSStartHighRdy() MUST:
;           a) Call OSTaskSwHook() then,
;           b) Set OSRunning to TRUE,
;           c) Switch to the highest priority task.
; 此函数是在osstart（）多任务启动之后，负责从最高优先级任务的tcb控制块中获得该任务的堆栈指针sp,
; 通过sp 依次将cpu 现场恢复，这时系统就将控制权交给用户创建的该任务进程， 
; 直到该任务被阻塞或者被其他更高优先级的任务抢占cpu。 
; 该函数仅仅在多任务启动时被执行一次，用来启动第一个，也就是最高优先级的任务执行， 
; 之后多任务的调度和切换就是由下面的函数来实现。 
;*********************************************************************************************************

        IMPORT  OSTaskSwHook
        IMPORT  OSRunning
        IMPORT  OSTCBHighRdy

        EXPORT  OSStartHighRdy

OSStartHighRdy  

        bl OSTaskSwHook             ; Call user defined task switch hook

        ldr r4,=OSRunning           ; Indicate that multitasking has started
        mov r5,#1
        strb r5,[r4]

        ldr r4,=OSTCBHighRdy        ; Get highest priority task TCB address

        ldr r4,[r4]                 ; get stack pointer
        ldr sp,[r4]                 ; switch to the new stack

        ldmfd sp!,{r4}              ; pop new task's spsr
        msr SPSR_cxsf,r4
        ldmfd sp!,{r4}              ; pop new task's psr
        msr CPSR_cxsf,r4
        ldmfd sp!,{r0-r12,lr,pc}    ; pop new task's r0-r12,lr & pc

;*********************************************************************************************************
;                                PERFORM A CONTEXT SWITCH (From task level)
;                                           void OSCtxSw(void)
;
; Note(s):    Upon entry, 
;             OSTCBCur     points to the OS_TCB of the task to suspend
;             OSTCBHighRdy points to the OS_TCB of the task to resume
;
; 任务级的上下文切换，它是当任务因为被阻塞而主动请求cpu 调度时被执行， 
; 由于此时的任务切换都是在非异常模式下进行的，因此区别于中断级别的任务切换。   
; 它的工作是先将当前任务的cpu 现场保存到该任务堆栈中，然后获得最高优先级任务的堆栈指针，	 
; 从该堆栈中恢复此任务的cpu 现场，使之继续执行。这样就完成了一次任务切换。	 
;*********************************************************************************************************

        IMPORT  OSTCBCur
        IMPORT  OSTaskSwHook
        IMPORT  OSTCBHighRdy
        IMPORT  OSPrioCur
        IMPORT  OSPrioHighRdy

        EXPORT  OSCtxSw

OSCtxSw
; Special optimised code below:
        stmfd sp!,{lr}              ; push pc (lr should be pushed in place of PC)
        stmfd sp!,{r0-r12,lr}       ; push lr & register file
        mrs r4,cpsr
        stmfd sp!,{r4}              ; push current psr
        mrs r4,spsr
        stmfd sp!,{r4}              ; push current spsr

        ; OSPrioCur = OSPrioHighRdy
        ldr r4,=OSPrioCur
        ldr r5,=OSPrioHighRdy
        ldrb r6,[r5]
        strb r6,[r4]
        
        ; Get current task TCB address
        ldr r4,=OSTCBCur
        ldr r5,[r4]
        str sp,[r5]                 ; store sp in preempted tasks's TCB

        bl OSTaskSwHook             ; call Task Switch Hook

        ; Get highest priority task TCB address
        ldr r6,=OSTCBHighRdy
        ldr r6,[r6]
        ldr sp,[r6]                 ; get new task's stack pointer

        ; OSTCBCur = OSTCBHighRdy
        str r6,[r4]                 ; set new current task TCB address

        ldmfd sp!,{r4}              ; pop new task's spsr
        msr SPSR_cxsf,r4
        ldmfd sp!,{r4}              ; pop new task's psr
        msr CPSR_cxsf,r4
        ldmfd sp!,{r0-r12,lr,pc}    ; pop new task's r0-r12,lr & pc


;*********************************************************************************************************
;                                PERFORM A CONTEXT SWITCH (From an ISR)
;                                        void OSIntCtxSw(void)
;
; Note(s): This function only flags a context switch to the ISR Handler
;
; 中断级的任务切换，它是在时钟中断isr（中断服务例程）中发现有高优先级任务等待的时钟信号到来，   
; 则需要在中断退出后并不返回被中断任务，而是直接调度就绪的高优先级任务执行。		    
; 这样做的目的主要是能够尽快地让高优先级的任务得到响应，保证系统的实时性能。		  
; 它的原理基本上与任务级的切换相同，但是由于进入中断时已经保存过了被中断任务的cpu 现场，	 
; 因此这里就不用再进行类似的操作，只需要对堆栈指针做相应的调整，原因是函数的嵌套。  
;*********************************************************************************************************

        IMPORT  OSIntCtxSwFlag

        EXPORT  OSIntCtxSw

OSIntCtxSw


        ;OSIntCtxSwFlag = True
        ldr r0,=OSIntCtxSwFlag
        mov r1,#1
        str r1,[r0]
        ;mov pc,lr
		BX	LR
        

;*********************************************************************************************************
;                                            IRQ HANDLER
;
;        This handles all the IRQs
;        Note: FIQ Handler should be written similar to this
;
;*********************************************************************************************************
        IMPORT  OSIntEnter
        IMPORT  OSIntExit

        IMPORT  OSIntCtxSwFlag
        IMPORT  OSTCBCur
        IMPORT  OSTaskSwHook
        IMPORT  OSTCBHighRdy
        IMPORT  OSPrioCur
        IMPORT  OSPrioHighRdy
        EXPORT  _IntCtxSw

NOINT   EQU 0xc0


_IntCtxSw
        mov r1,#0
        str r1,[r0]

        ldmfd sp!,{r0-r3,r12,lr}
        stmfd sp!,{r0-r3}
        mov r1,sp
        add sp,sp,#16
        sub r2,lr,#4

        mrs r3,spsr
        orr r0,r3,#NOINT
        msr spsr_c,r0

        ldr r0,=.+8
        movs pc,r0

        stmfd sp!,{r2}              ; push old task's pc
        stmfd sp!,{r4-r12,lr}       ; push old task's lr,r12-r4
        mov r4,r1                   ; Special optimised code below
        mov r5,r3
        ldmfd r4!,{r0-r3}
        stmfd sp!,{r0-r3}           ; push old task's r3-r0
        stmfd sp!,{r5}              ; push old task's psr
        mrs r4,spsr
        stmfd sp!,{r4}              ; push old task's spsr
        
        ; OSPrioCur = OSPrioHighRdy
        ldr r4,=OSPrioCur
        ldr r5,=OSPrioHighRdy
        ldrb r5,[r5]
        strb r5,[r4]
        
        ; Get current task TCB address
        ldr r4,=OSTCBCur
        ldr r5,[r4]
        str sp,[r5]                 ; store sp in preempted tasks's TCB

        bl OSTaskSwHook             ; call Task Switch Hook

        ; Get highest priority task TCB address
        ldr r6,=OSTCBHighRdy
        ldr r6,[r6]
        ldr sp,[r6]                 ; get new task's stack pointer

        ; OSTCBCur = OSTCBHighRdy
        str r6,[r4]                 ; set new current task TCB address

        ldmfd sp!,{r4}              ; pop new task's spsr
        msr SPSR_cxsf,r4
        ldmfd sp!,{r4}              ; pop new task's psr
        msr CPSR_cxsf,r4

        ldmfd sp!,{r0-r12,lr,pc}    ; pop new task's r0-r12,lr & pc

;*********************************************************************************************************
;                                   CRITICAL SECTION METHOD 3 FUNCTIONS
;
; Description: Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
;              would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
;              disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to 
;              disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
;              into the CPU's status register.
;
;              OS_CPU_SR OSCPUSaveSR()
; Arguments  : none
;
; Returns    : OS_CPU_SR
;
;              OSCPURestoreSR(OS_CPU_SR cpu_sr)
; Arguments  : OS_CPU_SR
;
; Returns    : none
;
; Note(s)    : These functions are used in general like this,
;
;            void Task (void *data)
;            {
;                    #if OS_CRITICAL_METHOD == 3 /* Allocate storage for CPU status register */
;                          OS_CPU_SR  cpu_sr;
;                    #endif
;                         :
;                         :
;                    OS_ENTER_CRITICAL(); /* cpu_sr = OSCPUSaveSR(); */
;                         :
;                         :
;                    OS_EXIT_CRITICAL();  /* OSCPURestoreSR(cpu_sr); */
;                         :
;                         :
;            }
;*********************************************************************************************************

        EXPORT  OSCPUSaveSR
OSCPUSaveSR

        mrs r0,CPSR
        orr r1,r0,#NOINT
        msr CPSR_c,r1
        MRS R1,CPSR                     ; Confirm that CPSR contains the proper interrupt disable flags
        AND R1,R1,#NOINT
        CMP R1,#NOINT
        BNE OSCPUSaveSR              ; Not properly disabled (try again)
        ;mov pc,lr
		BX	LR


        EXPORT  OSCPURestoreSR
OSCPURestoreSR

        msr CPSR_c,r0
        ;mov pc,lr
		BX	LR


        END