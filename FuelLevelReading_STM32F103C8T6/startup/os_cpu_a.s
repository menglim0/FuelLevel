;********************************************************************************************************
;                                               uC/OS-II
;                                         The Real-Time Kernel
;
;                                  (c) Copyright 1992-2006, GLR TECHNOLOGY
;                                          All Rights Reserved
;
;                                           ARM Cortex-M3 Port
;
; File      : OS_CPU_A.s
; Version   : V2.52
; By        : GLR TECHNOLOGY
;
; For       : ARMv7M Cortex-M3
; Mode      : Thumb2
; Toolchain : KEIL MDK
;********************************************************************************************************

;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************
    EXPORT  OS_CPU_SR_Save                                      ; Functions declared in this file
    EXPORT  OS_CPU_SR_Restore											   

;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

;********************************************************************************************************
;                                   CRITICAL SECTION METHOD 3 FUNCTIONS
;
; Description: Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
;              would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
;              disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
;              disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
;              into the CPU's status register.
;
; Prototypes :     OS_CPU_SR  OS_CPU_SR_Save(void);
;                  void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
;********************************************************************************************************

OS_CPU_SR_Save
    MRS     R0, PRIMASK                                      ; Set prio int mask to mask all (except faults)
    CPSID   I
    BX      LR

OS_CPU_SR_Restore
    MSR     PRIMASK, R0
    BX      LR

    END
