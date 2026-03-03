#include "sys.h"

// THUMB指令不支持汇编内联
// 采用如下方法实现执行汇编指令WFI

void WFI_SET(void) { __asm volatile("WFI"); }

void INTX_DISABLE(void) { __asm volatile("CPSID I"); }

void INTX_ENABLE(void) { __asm volatile("CPSIE I"); }

void MSR_MSP(u32 addr) { __asm volatile("MSR MSP, %0" : : "r"(addr)); }
