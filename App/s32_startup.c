#include <stdint.h>

// Simboli definiti dal linker script
extern uint32_t _sidata;    // Indirizzo di inizio dei dati inizializzati in Flash
extern uint32_t _sdata;     // Indirizzo di inizio dei dati inizializzati in RAM
extern uint32_t _edata;     // Indirizzo di fine dei dati inizializzati in RAM
extern uint32_t _sbss;      // Indirizzo di inizio della sezione .bss
extern uint32_t _ebss;      // Indirizzo di fine della sezione .bss
extern uint32_t _estack;    // Indirizzo di fine dello stack (inizio dello stack)

// Prototipo della funzione main
extern int main(void);

// Prototipo della funzione SystemInit (opzionale, fornita da NXP o configurata dall'utente)
// Serve per inizializzare il clock di sistema e altre configurazioni hardware di basso livello.
// Se non la usi o è chiamata altrove, puoi commentarla o rimuoverla.
// extern void SystemInit(void); 

// Prototipi dei gestori di interrupt di FreeRTOS
extern void xPortSysTickHandler(void);
extern void vPortSVCHandler(void);
extern void xPortPendSVHandler(void);

// Funzione di reset handler
void Reset_Handler(void);

// Default handler per gli interrupt non gestiti
void Default_Handler(void) {
    while (1);
}

// Gestori di interrupt deboli, possono essere sovrascritti
void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler"))); // Spesso si implementa un gestore specifico
void MemManage_Handler(void) __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
// SVC_Handler è gestito da FreeRTOS (vPortSVCHandler)
void DebugMon_Handler(void) __attribute__((weak, alias("Default_Handler")));
// PendSV_Handler è gestito da FreeRTOS (xPortPendSVHandler)
// SysTick_Handler è gestito da FreeRTOS (xPortSysTickHandler)

// Aggiungi qui altri gestori di interrupt specifici per S32K3xx se necessario,
// altrimenti verranno gestiti da Default_Handler.
// Esempio:
// void LPUART0_RxTx_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));


// Tabella dei vettori di interrupt
// Nota: Il primo valore è l'indirizzo iniziale dello stack.
// Il secondo è l'indirizzo del Reset_Handler.
// Questa tabella deve essere allineata e posizionata all'inizio della Flash.
__attribute__((section(".isr_vector")))
const void * const g_pfnVectors[] = {
    (void *)&_estack,            // Initial Stack Pointer
    Reset_Handler,               // Reset Handler
    NMI_Handler,                 // NMI Handler
    HardFault_Handler,           // Hard Fault Handler
    MemManage_Handler,           // MPU Fault Handler
    BusFault_Handler,            // Bus Fault Handler
    UsageFault_Handler,          // Usage Fault Handler
    0,                           // Reserved
    0,                           // Reserved
    0,                           // Reserved
    0,                           // Reserved
    vPortSVCHandler,             // SVCall Handler (FreeRTOS)
    DebugMon_Handler,            // Debug Monitor Handler
    0,                           // Reserved
    xPortPendSVHandler,          // PendSV Handler (FreeRTOS)
    xPortSysTickHandler,         // SysTick Handler (FreeRTOS)

    // Interrupt specifici del dispositivo S32K3xx (IRQ0 in poi)
    // Questi devono corrispondere al manuale di riferimento del tuo S32K3xx.
    // È FONDAMENTALE che questa tabella sia corretta e completa per il tuo specifico MCU.
    // Per S32K344 ci sono circa 240 interrupt. Riempire con Default_Handler
    // per tutti gli interrupt non esplicitamente gestiti.
    // Questa è una versione ancora abbreviata, ma più estesa della precedente.
    // CONSULTARE IL REFERENCE MANUAL PER LA LISTA COMPLETA DEGLI IRQ.
    Default_Handler, // IRQ0
    Default_Handler, // IRQ1
    Default_Handler, // IRQ2
    Default_Handler, // IRQ3
    Default_Handler, // IRQ4
    Default_Handler, // IRQ5
    Default_Handler, // IRQ6
    Default_Handler, // IRQ7
    Default_Handler, // IRQ8
    Default_Handler, // IRQ9
    Default_Handler, // IRQ10
    Default_Handler, // IRQ11
    Default_Handler, // IRQ12
    Default_Handler, // IRQ13
    Default_Handler, // IRQ14
    Default_Handler, // IRQ15
    Default_Handler, // IRQ16
    Default_Handler, // IRQ17
    Default_Handler, // IRQ18
    Default_Handler, // IRQ19
    Default_Handler, // IRQ20
    // ... (continuare a riempire con Default_Handler fino all'ultimo IRQ supportato)
    // Esempio: se ci sono 240 IRQ, servono 240 voci qui.
    // Per brevità, ci fermiamo qui, ma una tabella completa è necessaria per la produzione.
    // Aggiungiamo altri placeholder per arrivare a un numero più ragionevole per una demo estesa.
    Default_Handler, Default_Handler, Default_Handler, Default_Handler, // IRQ 21-24
    Default_Handler, Default_Handler, Default_Handler, Default_Handler, // IRQ 25-28
    Default_Handler, Default_Handler, Default_Handler, Default_Handler, // IRQ 29-32
    // ... e così via fino all'ultimo IRQ
};


void Reset_Handler(void) {
    uint32_t *pSrc, *pDest;

    // 1. Copia la sezione .data da Flash a RAM
    pSrc = &_sidata;
    pDest = &_sdata;
    while (pDest < &_edata) {
        *pDest++ = *pSrc++;
    }

    // 2. Inizializza la sezione .bss a zero
    pDest = &_sbss;
    while (pDest < &_ebss) {
        *pDest++ = 0;
    }

    // 3. (Opzionale) Chiama SystemInit per configurare il clock di sistema
    //    e altre inizializzazioni di basso livello.
    //    SystemInit è tipicamente fornita da NXP (es. in system_S32K3xx.c).
    //    Se non la usi o è già gestita, puoi ometterla.
    // #if defined (__USE_CMSIS) || defined (USE_SYSTEM_INIT) // Condizionale se usi CMSIS o una define custom
    // SystemInit();
    // #endif

    // 4. Chiama la funzione main dell'applicazione
    main();

    // 5. Se main ritorna (non dovrebbe mai accadere in un'applicazione embedded)
    while (1);
}

// Nota: Per un HardFault_Handler più utile, potresti voler stampare
// lo stato dei registri o salvare informazioni di debug.
// Esempio (molto basilare, richiede implementazione di printf/UART già funzionante):
/*
void HardFault_Handler_C(unsigned int *hardfault_args) {
    // unsigned int stacked_r0 = ((unsigned long) hardfault_args[0]);
    // unsigned int stacked_r1 = ((unsigned long) hardfault_args[1]);
    // unsigned int stacked_r2 = ((unsigned long) hardfault_args[2]);
    // unsigned int stacked_r3 = ((unsigned long) hardfault_args[3]);
    // unsigned int stacked_r12 = ((unsigned long) hardfault_args[4]);
    // unsigned int stacked_lr = ((unsigned long) hardfault_args[5]);
    // unsigned int stacked_pc = ((unsigned long) hardfault_args[6]);
    // unsigned int stacked_psr = ((unsigned long) hardfault_args[7]);

    // UART_printf("HardFault!\n");
    // UART_printf("R0 = %x, R1 = %x, R2 = %x, R3 = %x\n", stacked_r0, stacked_r1, stacked_r2, stacked_r3);
    // UART_printf("R12 = %x, LR = %x, PC = %x, PSR = %x\n", stacked_r12, stacked_lr, stacked_pc, stacked_psr);
    // UART_printf("BFAR = %x\n", (*((volatile unsigned long *)(0xE000ED38))));
    // UART_printf("CFSR = %x\n", (*((volatile unsigned long *)(0xE000ED28))));
    // UART_printf("HFSR = %x\n", (*((volatile unsigned long *)(0xE000ED2C))));
    // UART_printf("DFSR = %x\n", (*((volatile unsigned long *)(0xE000ED30))));
    // UART_printf("AFSR = %x\n", (*((volatile unsigned long *)(0xE000ED3C))));

    while(1);
}

__attribute__((naked))
void HardFault_Handler(void) {
    __asm volatile (
        " movs r0, #4       \n"
        " mov r1, lr        \n"
        " tst r0, r1        \n" // Check if Thread mode (LR bit 2)
        " beq _MSP          \n" // Branch to _MSP if 0 (MSP was used)
        " mrs r0, psp       \n" // PSP was used, so r0 = psp
        " b _GetRegs        \n" // Branch to _GetRegs
        "_MSP:              \n"
        " mrs r0, msp       \n" // MSP was used, so r0 = msp
        "_GetRegs:          \n"
        " ldr r1, [r0, #24] \n" // Get stacked PC
        " ldr r2, =HardFault_Handler_C \n"
        " bx r2             \n"
        : // No output
        : // No input
        : "r0", "r1", "r2" // Clobbered registers
    );
}
*/
