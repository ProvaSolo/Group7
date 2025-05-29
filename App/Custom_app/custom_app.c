/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h" // Se necessario per comunicare con i task

/* Peripheral includes */
#include "Peripherals/uart.h"
#include "Peripherals/lpspi.h"
// #include "Peripherals/IntTimer.h" // Se usi i timer
#include "Peripherals/printf-stdarg.h" // Per UART_printf

/* Task priorities */
#define mainUART_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define mainLPSPI_TASK_PRIORITY (tskIDLE_PRIORITY + 2)

// Clock di sistema (deve corrispondere a FreeRTOSConfig.h e all'emulazione)
#define SYSTEM_CLOCK_HZ configCPU_CLOCK_HZ // Definito in FreeRTOSConfig.h
#define LPSPI_SOURCE_CLOCK_HZ SYSTEM_CLOCK_HZ // O un clock specifico per LPSPI se diverso

/* Task prototypes */
void vUARTTask(void *pvParameters);
void vLPSPITask(void *pvParameters);

// Variabile globale per la UART usata da UART_printf
// È meglio inizializzarla esplicitamente.
extern LPUART_Type *g_uart_printf_base;


int main(void) {
    // Inizializzazione hardware di basso livello (es. clock, pin mux)
    // dovrebbe essere chiamata da SystemInit() in s32_startup.c o qui.
    // Per QEMU, molta di questa configurazione potrebbe essere già gestita dall'emulatore.

    // Inizializza la UART per printf e debug
    // Assumiamo LPUART12 per il debug come nel progetto di riferimento
    g_uart_printf_base = LPUART12; // Imposta la UART per UART_printf
    UART_Init(LPUART12, SYSTEM_CLOCK_HZ, 115200); // Baud rate 115200

    UART_printf("Sistema inizializzato. Benvenuto nel Progetto Group7!\r\n");
    UART_printf("CPU Clock: %lu Hz\r\n", SYSTEM_CLOCK_HZ);

    // Inizializza LPSPI0 (esempio)
    lpspi_master_config_t lpspi_config;
    lpspi_config.baudRate = 1000000; // 1 MHz
    lpspi_config.bitsPerFrame = 8;
    lpspi_config.cpol = kLPSPI_PolarityActiveHigh; // SCK idle low
    lpspi_config.cpha = kLPSPI_ClockPhaseFirstEdge; // Dati campionati sul fronte di salita
    lpspi_config.pcsToUse = 0; // Usa PCS0

    LPSPI_MasterInit(LPSPI0, &lpspi_config, LPSPI_SOURCE_CLOCK_HZ);
    UART_printf("LPSPI0 inizializzato.\r\n");

    /* Crea i task */
    xTaskCreate(vUARTTask, "UARTTask", configMINIMAL_STACK_SIZE + 100, NULL, mainUART_TASK_PRIORITY, NULL);
    xTaskCreate(vLPSPITask, "LPSPITask", configMINIMAL_STACK_SIZE + 100, NULL, mainLPSPI_TASK_PRIORITY, NULL);

    UART_printf("Scheduler FreeRTOS avviato.\r\n");
    vTaskStartScheduler();

    /* Non si dovrebbe mai arrivare qui */
    for (;;);
    return 0;
}

void vUARTTask(void *pvParameters) {
    (void)pvParameters;
    // uint8_t rx_char; // Commentato per evitare warning unused se non si usa l'eco
    UART_printf("Task UART in esecuzione.\r\n");

    for (;;) {
        UART_printf("UART Task: Invio 'U' tramite LPUART12...\r\n");
        UART_SendChar(LPUART12, 'U');
        // rx_char = UART_ReceiveChar(LPUART12); // Semplice eco bloccante
        // UART_printf("UART Task: Ricevuto '%c'\r\n", rx_char);
        vTaskDelay(pdMS_TO_TICKS(2000)); // Ogni 2 secondi
    }
}

void vLPSPITask(void *pvParameters) {
    (void)pvParameters;
    
#define LPSPI_PACKET_SIZE 4
    uint8_t tx_buffer[LPSPI_PACKET_SIZE] = {0xAA, 0x01, 0x02, 0x55}; // Esempio: Start, Cmd, Data, End
    uint8_t rx_buffer[LPSPI_PACKET_SIZE];
    lpspi_transfer_t spi_transfer;
    int i;

    UART_printf("Task LPSPI in esecuzione.\r\n");
    UART_printf("Questo task simula l'invio di un pacchetto dati tramite LPSPI0 e la lettura di una risposta.\r\n");
    UART_printf("In QEMU, senza uno slave SPI emulato che risponde attivamente, i dati ricevuti (MISO)\r\n");
    UART_printf("saranno tipicamente 0xFF per ogni byte trasmesso dal master.\r\n");

    spi_transfer.txData = tx_buffer;
    spi_transfer.rxData = rx_buffer;
    spi_transfer.dataSize = LPSPI_PACKET_SIZE;
    spi_transfer.flags = 0; // Nessun flag speciale per questo trasferimento

    for (;;) {
        UART_printf("LPSPI Task: Invio pacchetto: [");
        for(i = 0; i < LPSPI_PACKET_SIZE; ++i) {
            UART_printf("0x%02X%s", tx_buffer[i], (i == LPSPI_PACKET_SIZE - 1) ? "" : ", ");
        }
        UART_printf("]\r\n");

        // Esegui il trasferimento SPI utilizzando la funzione per buffer
        LPSPI_MasterTransfer(LPSPI0, &spi_transfer);

        UART_printf("LPSPI Task: Pacchetto ricevuto: [");
        for(i = 0; i < LPSPI_PACKET_SIZE; ++i) {
            UART_printf("0x%02X%s", rx_buffer[i], (i == LPSPI_PACKET_SIZE - 1) ? "" : ", ");
        }
        UART_printf("]\r\n");
        
        // Modifica il pacchetto per il prossimo invio (es. incrementa un contatore nel pacchetto)
        tx_buffer[1]++; // Incrementa il byte del "comando"
        if (tx_buffer[1] == 0x00) { // Esempio di rollover
            tx_buffer[2]++; // Incrementa il byte dei "dati"
        }
        // Potresti ricalcolare un checksum semplice qui se tx_buffer[3] fosse un checksum
        
        vTaskDelay(pdMS_TO_TICKS(3000)); // Ogni 3 secondi
    }
}

/* Implementazione di vApplicationMallocFailedHook se configUSE_MALLOC_FAILED_HOOK è 1 */
void vApplicationMallocFailedHook( void ) {
    taskDISABLE_INTERRUPTS();
    UART_printf("!!! MALLOC FAILED !!!\r\n");
    for( ;; );
}

/* Implementazione di vApplicationStackOverflowHook se configCHECK_FOR_STACK_OVERFLOW > 0 */
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ) {
    ( void ) pcTaskName;
    ( void ) pxTask;
    taskDISABLE_INTERRUPTS();
    UART_printf("!!! STACK OVERFLOW IN TASK: %s !!!\r\n", pcTaskName);
    for( ;; );
}

/* Implementazione di vApplicationIdleHook se configUSE_IDLE_HOOK è 1 */
void vApplicationIdleHook( void ) {
    // Metti la CPU in modalità a basso consumo se possibile
}

/* Implementazione di vApplicationTickHook se configUSE_TICK_HOOK è 1 */
void vApplicationTickHook( void ) {
    // Esegui azioni periodiche qui, se necessario
}
