/* startup.c */

/* Peripheral includes */
#include "uart.h"
#include "lpspi.h"
#include <stdio.h>

/* FreeRTOS interrupt handlers */
extern void vPortSVCHandler( void );
extern void xPortPendSVHandler( void );
extern void xPortSysTickHandler( void );

/* Memory section markers from linker script */
extern uint32_t _estack;     /* Stack top          */
extern uint32_t _sidata;     /* .data LMA (Flash)  */
extern uint32_t _sdata;      /* .data VMA (RAM)    */
extern uint32_t _edata;      /* End of .data       */
extern uint32_t _sbss;       /* Start of .bss      */
extern uint32_t _ebss;       /* End of .bss        */

/* Exception handlers */
void Reset_Handler(void) __attribute__((naked));
static void HardFault_Handler(void) __attribute__((naked));
static void MemManage_Handler(void) __attribute__((naked));
static void Default_Handler(void);

/* MPU initialization */
// extern void vPortSetupMPU(void);

/* Main application entry */
extern int main(void);

/* Fault diagnostic functions */
void prvGetRegistersFromStack(uint32_t *pulFaultStackAddress) __attribute__((used));
void print_fault_info(uint32_t cfsr, uint32_t mmfar, uint32_t bfar);

/*-----------------------------------------------------------------------------------------*/
/* Reset Handler - Core initialization sequence                                            */
/*-----------------------------------------------------------------------------------------*/

void Reset_Handler(void) 
{
    /* 1. Initialize main stack pointer */
    __asm volatile (
        "ldr r0, =_estack\n\t"
        "msr msp, r0"
    );

    /* 2. MPU Setup - Must happen before any memory access */
    // vPortSetupMPU();

    /* 3. Copy data segment from flash to RAM */
    uint32_t *data_load = &_sidata;
    uint32_t *data_vma = &_sdata;
    while(data_vma < &_edata) *data_vma++ = *data_load++;

    /* 4. Zero-initialize BSS segment */
    uint32_t *bss_start = &_sbss;
    uint32_t *bss_end = &_ebss;
    while(bss_start < bss_end) *bss_start++ = 0;

    /* 5. Call platform initialization */
    extern void SystemInit(void);
    SystemInit();

    /* 6. Jump to main application */
    main();

    /* 7. Fallback if main returns */
    while(1);
}

/*-----------------------------------------------------------------------------------------*/
/* Memory Management Fault Handler (MPU violations)                                        */
/*-----------------------------------------------------------------------------------------*/

void MemManage_Handler(void)
{
    __asm volatile (
        " tst lr, #4              \n"
        " ite eq                  \n"
        " mrseq r0, msp           \n"
        " mrsne r0, psp           \n"
        " ldr r1, [r0, #24]       \n"
        " ldr r2, =prvGetRegistersFromStack \n"
        " bx r2                   \n"
        " .ltorg                  \n"
    );
}

/*-----------------------------------------------------------------------------------------*/
/* Hard Fault Handler - Generic fault catcher                                              */
/*-----------------------------------------------------------------------------------------*/

void HardFault_Handler(void)
{
    __asm volatile (
        " tst lr, #4              \n"
        " ite eq                  \n"
        " mrseq r0, msp           \n"
        " mrsne r0, psp           \n"
        " ldr r1, [r0, #24]       \n"
        " ldr r2, =prvGetRegistersFromStack \n"
        " bx r2                   \n"
        " .ltorg                  \n"
    );
}

/*-----------------------------------------------------------------------------------------*/
/* Fault Register Analysis                                                                 */
/*-----------------------------------------------------------------------------------------*/

void print_fault_info(uint32_t cfsr, uint32_t mmfar, uint32_t bfar)
{
    char buf[64];
    
    /* CFSR Decoding */
    UART_printf("\nConfigurable Fault Status Register:\n");
    snprintf(buf, sizeof(buf), "  CFSR: 0x%08lX\n", cfsr);
    UART_printf(buf);
    
    /* Memory Management Faults */
    if(cfsr & 0xFF) {
        UART_printf("  Memory Management Fault:\n");
        if(cfsr & (1 << 0)) UART_printf("    IACCVIOL: Instruction access violation\n");
        if(cfsr & (1 << 1)) UART_printf("    DACCVIOL: Data access violation\n");
        if(cfsr & (1 << 3)) UART_printf("    MUNSTKERR: MemManage on exception return\n");
        if(cfsr & (1 << 4)) UART_printf("    MSTKERR: MemManage on exception entry\n");
        if(cfsr & (1 << 7)) {
            snprintf(buf, sizeof(buf), "    MMFAR: 0x%08lX\n", mmfar);
            UART_printf(buf);
        }
    }
}

/*-----------------------------------------------------------------------------------------*/
/* Register Dump from Fault Context                                                        */
/*-----------------------------------------------------------------------------------------*/

void prvGetRegistersFromStack(uint32_t *pulFaultStackAddress)
{
    volatile uint32_t r0  = pulFaultStackAddress[0];
    volatile uint32_t r1  = pulFaultStackAddress[1];
    volatile uint32_t r2  = pulFaultStackAddress[2];
    volatile uint32_t r3  = pulFaultStackAddress[3];
    volatile uint32_t r12 = pulFaultStackAddress[4];
    volatile uint32_t lr  = pulFaultStackAddress[5];
    volatile uint32_t pc  = pulFaultStackAddress[6];
    volatile uint32_t psr = pulFaultStackAddress[7];

    /* Get fault status registers */
    uint32_t cfsr  = *(volatile uint32_t*)0xE000ED28;
    uint32_t mmfar = *(volatile uint32_t*)0xE000ED34;
    uint32_t bfar  = *(volatile uint32_t*)0xE000ED38;

    char buffer[100];
    UART_printf("\n*** Hardware Fault Detected ***\n");
    
    /* Print general registers */
    snprintf(buffer, sizeof(buffer), "R0   = 0x%08lX\n", r0);  UART_printf(buffer);
    snprintf(buffer, sizeof(buffer), "R1   = 0x%08lX\n", r1);  UART_printf(buffer);
    snprintf(buffer, sizeof(buffer), "R2   = 0x%08lX\n", r2);  UART_printf(buffer);
    snprintf(buffer, sizeof(buffer), "R3   = 0x%08lX\n", r3);  UART_printf(buffer);
    snprintf(buffer, sizeof(buffer), "R12  = 0x%08lX\n", r12); UART_printf(buffer);
    snprintf(buffer, sizeof(buffer), "LR   = 0x%08lX\n", lr);  UART_printf(buffer);
    snprintf(buffer, sizeof(buffer), "PC   = 0x%08lX\n", pc);  UART_printf(buffer);
    snprintf(buffer, sizeof(buffer), "PSR  = 0x%08lX\n", psr); UART_printf(buffer);

    /* Detailed fault analysis */
    print_fault_info(cfsr, mmfar, bfar);
    
    while(1);
}

/*-----------------------------------------------------------------------------------------*/
/* Default Exception Handler                                                               */
/*-----------------------------------------------------------------------------------------*/

void Default_Handler(void)
{
    __asm volatile(
        "Infinite_Loop:\n"
        "    b Infinite_Loop\n"
    );
}

/*-----------------------------------------------------------------------------------------*/
/* Interrupt Vector Table                                                                  */
/*-----------------------------------------------------------------------------------------*/

const uint32_t* isr_vector[] __attribute__((section(".isr_vector"))) = {

    /* Core Exceptions */
    (uint32_t*)&_estack,                       /* Initial Stack Pointer */
    (uint32_t*)Reset_Handler,                  /* Reset Handler */
    (uint32_t*)Default_Handler,                /* NMI */
    (uint32_t*)HardFault_Handler,              /* Hard Fault */
    (uint32_t*)MemManage_Handler,              /* MPU Fault */
    (uint32_t*)Default_Handler,                /* Bus Fault */
    (uint32_t*)Default_Handler,                /* Usage Fault */
    0, 0, 0, 0,                                /* Reserved */
    (uint32_t*)vPortSVCHandler,                /* FreeRTOS SVC */
    (uint32_t*)Default_Handler,                /* Debug Monitor */
    0,                                         /* Reserved */
    (uint32_t*)xPortPendSVHandler,             /* FreeRTOS PendSV */
    (uint32_t*)xPortSysTickHandler,            /* FreeRTOS SysTick */
    0,0,0,0,
    0,0,0,0,
    /* Peripheral Interrupts */
    (uint32_t*)TIMER0_IRQHandler,              /* Timer 0 */
    (uint32_t*)TIMER1_IRQHandler,              /* Timer 1 */
    (uint32_t*)TIMER2_IRQHandler,              /* Timer 2 */
    
    /* Other interrupts not initialized in the Board */
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0
};

/*-----------------------------------------------------------------------------------------*/