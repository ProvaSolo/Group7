#include "lpspi.h"

void LPSPI_MasterInit(LPSPI_Type *base, const lpspi_master_config_t *config, uint32_t srcClock_Hz) {
    // Nota: l'abilitazione del clock per LPSPI dovrebbe essere gestita
    // a un livello più basso (es. SystemInit o specifico init della board in QEMU).

    // 1. Software Reset
    base->CR |= LPSPI_CR_RST_MASK;
    base->CR &= ~LPSPI_CR_RST_MASK; // Rilascia il reset

    // 2. Abilita il modulo LPSPI (MEN = 1)
    base->CR |= LPSPI_CR_MEN_MASK;

    // 3. Configura come Master
    base->CFGR1 |= LPSPI_CFGR1_MASTER_MASK;

    // 4. Configura Baud Rate (CCR)
    //    Questa è una configurazione semplificata. Consultare il manuale per i dettagli.
    //    SCKDIV: Clock Divider, PRESCALE: Prescaler
    //    baudRate = srcClock_Hz / (PRESCALE * (SCKDIV + 2))
    //    Scegliamo PRESCALE = 0 (divide by 1) per semplicità iniziale
    uint32_t prescale = 0; // Divisor = 2^prescale
    uint32_t sckdiv = (srcClock_Hz / (config->baudRate * (1 << prescale))) - 2;
    if (sckdiv > 255) sckdiv = 255; // Max valore per SCKDIV

    base->CCR = ( (prescale & 0x7) << 24 ) | (sckdiv & 0xFF);
    // Potrebbe essere necessario configurare anche DBT (Delay Between Transfers) e PCSSCK/SCKPCS (PCS to SCK/SCK to PCS delays)

    // 5. Configura TCR (Transmit Command Register) per CPOL, CPHA, BitsPerFrame, PCS
    uint32_t tcr = 0;
    tcr |= LPSPI_TCR_FRAMESZ(config->bitsPerFrame);
    tcr |= LPSPI_TCR_PCS(config->pcsToUse);

    if (config->cpol == kLPSPI_PolarityActiveLow) {
        tcr |= (1u << 27); // CPOL = 1
    }
    if (config->cpha == kLPSPI_ClockPhaseSecondEdge) {
        tcr |= (1u << 26); // CPHA = 1
    }
    // Non impostare PRESCALE qui nel TCR se si usa CCR per il baud rate principale.
    // Il prescaler nel TCR è per comandi specifici.

    base->TCR = tcr; // Applica la configurazione di default al TCR

    // 6. Configura FIFO (FCR) - opzionale, per ora usiamo la modalità bufferizzata
    // base->FCR = 0; // TXWATER = 0, RXWATER = 0 (FIFO disabilitate o profondità minima)

    // 7. Pulisci eventuali flag di stato
    base->SR = 0xFFFFFFFF; // Scrivi 1 per pulire i flag scrivibili
}

void LPSPI_MasterDeinit(LPSPI_Type *base) {
    // Disabilita il modulo
    base->CR &= ~LPSPI_CR_MEN_MASK;
}

uint8_t LPSPI_MasterTransferBlocking(LPSPI_Type *base, uint8_t send_byte) {
    // Assicurati che il modulo non sia occupato (opzionale, dipende dalla logica)
    // while (base->SR & LPSPI_SR_MBF_MASK);

    // Scrivi il byte da trasmettere
    // Il TCR potrebbe dover essere aggiornato per ogni trasferimento se si cambiano PCS, ecc.
    // Per un singolo byte, il TCR impostato in Init potrebbe essere sufficiente.
    // Assicurati che il TCR sia configurato per la dimensione del frame corretta (es. 8 bit).
    // base->TCR = (base->TCR & ~LPSPI_TCR_FRAMESZ(0xFFF)) | LPSPI_TCR_FRAMESZ(8); // Esempio per 8 bit

    base->TDR = (uint32_t)send_byte;

    // Attendi che la trasmissione sia completa (TDF = Transmit Data Flag)
    while (!(base->SR & LPSPI_SR_TDF_MASK));
    base->SR |= LPSPI_SR_TDF_MASK; // Pulisci il flag

    // Attendi che la ricezione sia completa (RDF = Receive Data Flag)
    while (!(base->SR & LPSPI_SR_RDF_MASK));
    uint8_t received_byte = (uint8_t)(base->RDR & 0xFF);
    base->SR |= LPSPI_SR_RDF_MASK; // Pulisci il flag (anche se RDR è read-only, il flag SR è R/W1C)

    return received_byte;
}


void LPSPI_MasterTransfer(LPSPI_Type *base, lpspi_transfer_t *transfer) {
    // Implementazione per trasferimenti più complessi (buffer multipli, non bloccante, DMA)
    // Questa è una versione bloccante semplificata per buffer:
    uint8_t dummy_rx;
    uint8_t dummy_tx = 0xFF; // Valore dummy da inviare se txData è NULL

    // Assicurati che il TCR sia configurato per la dimensione del frame corretta (es. 8 bit).
    // base->TCR = (base->TCR & ~LPSPI_TCR_FRAMESZ(0xFFF)) | LPSPI_TCR_FRAMESZ(8);

    for (size_t i = 0; i < transfer->dataSize; ++i) {
        uint8_t byte_to_send = transfer->txData ? transfer->txData[i] : dummy_tx;
        
        // Attendi che il FIFO di trasmissione abbia spazio (o TDF se FIFO disabilitate)
        while (!(base->SR & LPSPI_SR_TDF_MASK));
        base->TDR = byte_to_send;
        // base->SR |= LPSPI_SR_TDF_MASK; // TDF si pulisce automaticamente quando TDR viene scritto (se TXFIFO vuoto)
                                       // o quando il dato è mosso allo shift register.
                                       // Per sicurezza, si può pulire dopo aver verificato.

        // Attendi che il dato sia ricevuto
        while (!(base->SR & LPSPI_SR_RDF_MASK));
        uint8_t received_byte = (uint8_t)(base->RDR & 0xFF);
        
        if (transfer->rxData) {
            transfer->rxData[i] = received_byte;
        } else {
            dummy_rx = received_byte; // Leggi per svuotare RDR e pulire RDF
            (void)dummy_rx; // Evita warning unused variable
        }
        // RDF si pulisce leggendo RDR.
    }
}
