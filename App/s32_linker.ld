/* filepath: /Users/pierfrancescovicari/Desktop/Polito/Magistrale/CAOS/progetto/Group7-main/s32_linker.ld */
/* Linker script per NXP S32K3X8EVB (es. S32K358/S32K388) con FreeRTOS */

/* Definisci l'entry point */
ENTRY(Reset_Handler)

/* Definisci le dimensioni dello stack e dell'heap (se non gestito dinamicamente da FreeRTOS heap_x) */
/* Lo stack principale (MSP) è usato solo fino all'avvio dello scheduler di FreeRTOS. */
/* Ogni task di FreeRTOS avrà il suo stack. */
_Min_Stack_Size = 0x400 ; /* Dimensione minima dello stack (MSP), es. 1KB */
/* _Min_Heap_Size = 0x200 ; */ /* Non usato se si usa heap_x.c di FreeRTOS che gestisce l'heap in .bss o un array statico */

/* Definizioni della memoria */
/* Questi valori DEVONO corrispondere al tuo specifico MCU S32K3X8 e alla sua mappa di memoria. */
/* Controlla il manuale di riferimento del tuo dispositivo (es. S32K358, S32K388). */
/* Flash: S32K358 ha fino a 6MB, S32K388 fino a 8MB di Code Flash. L'indirizzo di boot è 0x00400000. */
/* RAM: S32K358/S32K388 hanno diverse SRAM. */
/*      SRAM_A0 (System RAM 0): 0x20400000, size 512KB (0x80000) */
/*      SRAM_A1 (System RAM 1): 0x20480000, size 256KB (S32K358) or 512KB (S32K388) */
/*      SRAM_B0 (TCM per CM7_0): 0x30000000, size 128KB (0x20000) */
/*      Qui usiamo SRAM_A0 per i dati generali, stack e heap. */

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00400000, LENGTH = 6144K /* Esempio per S32K358 (6MB Code Flash). Adatta a 8192K per S32K388 se necessario. */
  RAM (xrw)  : ORIGIN = 0x20400000, LENGTH = 512K  /* Esempio per SRAM_A0 (System RAM 0) */
  /* Se vuoi usare la TCM (es. SRAM_B0 per CM7_0): */
  /* RAM_TCM (xrw) : ORIGIN = 0x30000000, LENGTH = 128K */
}

/* Simboli globali per lo stack */
/* _estack punta alla fine della regione RAM principale definita. */
_estack = ORIGIN(RAM) + LENGTH(RAM); /* Lo stack cresce verso il basso, quindi _estack è la fine della RAM */

SECTIONS
{
    /* La sezione .isr_vector deve essere all'inizio della Flash */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector)) /* Mantieni la tabella dei vettori all'inizio */
        . = ALIGN(4);
    } >FLASH

    /* Codice eseguibile */
    .text :
    {
        . = ALIGN(4);
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        *(.rodata)         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
        *(.glue_7)         /* glue arm to thumb */
        *(.glue_7t)        /* glue thumb to arm */
        *(.eh_frame)

        KEEP (*(.init))
        KEEP (*(.fini))

        . = ALIGN(4);
        _etext = .;        /* Simbolo per la fine della sezione codice */
    } >FLASH

    /* Dati inizializzati */
    /* _sidata è l'indirizzo di load (in Flash) */
    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;        /* Inizio della sezione .data in RAM */
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */
        . = ALIGN(4);
        _edata = .;        /* Fine della sezione .data in RAM */
    } >RAM AT> FLASH       /* Carica da FLASH, esegui da RAM (System RAM in questo caso) */


    /* Sezione .bss (dati non inizializzati) */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;         /* Inizio della sezione .bss */
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;         /* Fine della sezione .bss */
    } >RAM /* Posiziona .bss in System RAM */

    /* Sezione per l'heap, se usata da malloc o da FreeRTOS heap_x.c */
    /* Se heap_x.c usa un array statico, questa sezione potrebbe non essere necessaria */
    /* o potrebbe essere usata per posizionare quell'array. */
    /* Lo stack MSP è allocato qui. */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        PROVIDE ( end = . ); /* Simbolo 'end' usato da sbrk (se presente) */
        PROVIDE ( _end = . );
        . = . + _Min_Stack_Size; /* Spazio per lo stack MSP (usato prima dell'avvio di FreeRTOS) */
        /* . = . + _Min_Heap_Size; */ /* Spazio per l'heap se necessario */
        . = ALIGN(8);
    } >RAM /* Posiziona lo stack MSP in System RAM */

    /* Rimuovi sezioni non necessarie per ridurre le dimensioni */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    /* Informazioni di debug DWARF */
    .ARM.attributes 0 : { *(.ARM.attributes) }
}