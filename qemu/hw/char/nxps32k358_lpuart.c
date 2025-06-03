#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "hw/char/nxps32k358_lpuart.h"
#include "hw/qdev-clock.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-properties-system.h"
#include "hw/irq.h"
#include "trace.h" // tracing system of qemu
#include "chardev/char-serial.h"
#include "qapi/error.h"

#ifndef NXP_LPUART_DEBUG
#define NXP_LPUART_DEBUG 0
#endif

#define DB_PRINT_L(lvl, fmt, args...)               \
    do {                                            \
        if (NXP_LPUART_DEBUG >= lvl) {              \
            qemu_log("%s: " fmt, __func__, ##args); \
        }                                           \
    } while (0)

#define DB_PRINT(fmt, args...) DB_PRINT_L(1, fmt, ##args)
#define DB_PRINT_READ(fmt, args...) DB_PRINT_L(2, fmt, ##args)

// SECONDA REVISIONE 20 MAY
static uint32_t nxps32k358_lpuart_calculate_baud_rate(NXPS32K358LPUARTState *s) {
    uint32_t sbr;
    uint32_t osr_val_in_reg; // Valore del campo OSR letto dal registro
    uint64_t lpuart_module_clk_freq;

    // 1. Estrai SBR (Baud Rate Modulo Divisor)
    sbr = (s->baud_rate_config & LPUART_BAUD_SBR_MASK) >> LPUART_BAUD_SBR_SHIFT;

    // 2. Estrai OSR (Over Sampling Ratio)
    osr_val_in_reg = (s->baud_rate_config & LPUART_BAUD_OSR_MASK) >> LPUART_BAUD_OSR_SHIFT;
   
    lpuart_module_clk_freq = clock_get_hz(s->clk);

    // 3. Calcola e restituisci il baud rate usando la formula diretta (osr + 1)
    // ATTENZIONE: Questo non considera che se osr_val_in_reg < 3, l'oversampling effettivo è 16x.
    uint64_t divisor = (uint64_t)(osr_val_in_reg + 1) * sbr;
    if (divisor == 0) {
        return 0;
    }

    return lpuart_module_clk_freq / divisor;
}
static void nxps32k358_lpuart_update_params(NXPS32K358LPUARTState *s) {
    QEMUSerialSetParams ssp;
    ssp.speed = nxps32k358_lpuart_calculate_baud_rate(s);
    DB_PRINT("Baud rate: %d\n", ssp.speed);

    qemu_chr_fe_ioctl(&s->chr, CHR_IOCTL_SERIAL_SET_PARAMS, &ssp);
}

static void nxps32k358_lpuart_update_irq(NXPS32K358LPUARTState *s)
{
    int irq_pending = 0;

    // Controlla l'interrupt di ricezione dati
    if ((s->lpuart_cr & LPUART_CTRL_RIE) && (s->lpuart_sr & LPUART_STAT_RDRF))
    {
        irq_pending = 1;
    }

    // Controlla l'interrupt di trasmettitore pronto
    if (!irq_pending && (s->lpuart_cr & LPUART_CTRL_TIE) && (s->lpuart_sr & LPUART_STAT_TDRE))
    {
        // Spesso TIE e RIE sono mutuamente esclusivi o gestiti con priorità.
        // Se RDRF è attivo, potresti non voler segnalare TDRE subito,
        // ma questo dipende dalla logica esatta del chip.
        // Per semplicità, qui li consideriamo indipendenti se non già irq_pending.
        // In molti casi, è un OR di tutte le condizioni.
        irq_pending = 1;
    }
    // La riga sopra può essere semplificata se tutti gli interrupt possono essere attivi contemporaneamente:
    // if ((s->lpuart_cr & LPUART_CTRL_TIE) && (s->lpuart_sr & LPUART_STAT_TDRE)) {
    //     irq_pending = true;
    // }
    // Controlla l'interrupt di trasmissione completata
    if ((s->lpuart_cr & LPUART_CTRL_TIE) && (s->lpuart_sr & LPUART_STAT_TDRE))
    {
        irq_pending = 1;
    }

    // AGGIUNGI QUI I CONTROLLI PER GLI INTERRUPT DI ERRORE (OR, NF, FE, PF)
    // Esempio per Overrun (ORIE e OR sono nomi ipotetici, verifica sul manuale S32K3):
    // #define LPUART_CTRL_ORIE (1 << XYZ) // Bit di enable per interrupt Overrun
    // #define LPUART_STAT_OR   (1 << ABC) // Flag di stato Overrun
    // if ((s->lpuart_cr & LPUART_CTRL_ORIE) && (s->lpuart_sr & LPUART_STAT_OR)) {
    //     irq_pending = true;
    // }

    // AGGIUNGI QUI I CONTROLLI PER ALTRI INTERRUPT SPECIFICI (IDLE, LIN Break, ecc.)

    // Infine, imposta lo stato della linea IRQ
    qemu_set_irq(s->irq, irq_pending);
}

// Funzione chiamata quando QEMU può inviare un carattere al guest
static int nxps32k358_lpuart_can_receive(void *opaque)
{
    NXPS32K358LPUARTState *s = NXPS32K358_LPUART(opaque);

    // Controlla se c'è spazio nella FIFO di ricezione o se il ricevitore è abilitato
    if (!(s->lpuart_cr & LPUART_CTRL_RE))  return 1; // Non può ricevere
    
    return 0; // Può ricevere
}

// Funzione chiamata quando QEMU ha un carattere da inviare al guest
static void nxps32k358_lpuart_receive(void *opaque, const uint8_t *buf, int size)
{
    NXPS32K358LPUARTState *s = NXPS32K358_LPUART(opaque);

    // return when the size is 0(so no data to be sent) or when the RE is not enabled
    if (!(s->lpuart_cr & LPUART_CTRL_RE) || size == 0)
    {
        return;
    }
    s->lpuart_dr = *buf;
    s->lpuart_sr |= LPUART_STAT_RDRF; // set the status register on the flag receive data register full

    // at the end need to be done to send the Interrupt :)
    nxps32k358_lpuart_update_irq(s);
}

static void nxps32k358_lpuart_reset(DeviceState *dev)
{
    NXPS32K358LPUARTState *s = NXPS32K358_LPUART(dev);

    s->lpuart_cr = 0x00000000;        // Valore di reset dal manuale
    s->lpuart_sr = 0x00C00000;        // TDRE è solitamente 1 al reset, TBD
                                      // Altri flag (es. TC) potrebbero essere 1. Verifica!
    s->baud_rate_config = 0x0F000004; // Valore di reset dal manuale (esempio)

    nxps32k358_lpuart_update_irq(s);
}

// hwaddr is called offset because it the offset to access to a given register. In this case is possible because MemoryRegionOps è
// l'offset relativo a quella memor region definita. Sarà opaque a definire la lpuart

static uint64_t nxps32k358_lpuart_read(void *opaque, hwaddr offset, unsigned size)
{
    NXPS32K358LPUARTState *s = NXPS32K358_LPUART(opaque);
    uint64_t ret = 0;

    // qemu_log_mask(LOG_GUEST_ERROR, "LPUART read offset=0x%02x size=%u\n", (int)offset, size);

    switch (offset)
    {
    case LPUART_BAUD:
        ret = s->baud_rate_config;
        break;
    case LPUART_STAT:
        ret = s->lpuart_sr;
        // Alcuni bit di stato (come RDRF) potrebbero essere read-to-clear o auto-clearing
        // dopo la lettura del registro DATA. Implementa questa logica se necessario.
        break;
    case LPUART_CTRL:
        ret = s->lpuart_cr;
        break;
    case LPUART_DATA:
        ret = s->lpuart_dr & 0x3FF;
        s->lpuart_sr &= ~LPUART_STAT_RDRF;
        qemu_chr_fe_accept_input(&s->chr);
        // La lettura di DATA spesso cancella RDRF e l'interrupt associato
        nxps32k358_lpuart_update_irq(s);
        break;
    // Aggiungi qui i case per altri registri leggibili (VERID, PARAM, FIFO, WATER, ecc.)
    // basandoti sul S32K3XXRM.pdf
    case LPUART_VERID:
        // ret = ;
        break; // Valore esempio, metti quello reale
    case LPUART_PARAM:
        // ret = ;
        break; // Valore esempio
    case LPUART_FIFO:
        break;
    default:
        qemu_log_mask(LOG_UNIMP, "NXP S32K3 LPUART: Unimplemented read offset 0x%" HWADDR_PRIx "\n", offset);
        return 0;
    }

    return ret;
}

static void nxps32k358_lpuart_write(void *opaque, hwaddr offset, uint64_t val64, unsigned size)
{
    NXPS32K358LPUARTState *s = NXPS32K358_LPUART(opaque);
    uint32_t value = val64;
    unsigned char ch;
    // qemu_log_mask(LOG_GUEST_ERROR, "LPUART write offset=0x%02x val=0x%08x size=%u\n", (int)offset, (uint32_t)value, size);

    switch (offset)
    {
    case LPUART_BAUD:
        s->baud_rate_config = value;
        nxps32k358_lpuart_update_params(s);
        return;
    case LPUART_STAT:
        if (value <= 0x3FF)
        {
            /* I/O being synchronous, TXE is always set. In addition, it may
            only be set by hardware, so keep it set here. */
            s->lpuart_sr = value | LPUART_STAT_TDRE;
        }
        else s->lpuart_sr &= value;
        return;
    case LPUART_CTRL:
        s->lpuart_cr = value;
        nxps32k358_lpuart_update_irq(s);

        // qemu_log_mask(LOG_GUEST_ERROR, "LPUART CTRL set to 0x%08x\n", (uint32_t)value);
        return;
    case LPUART_DATA:
        if (value < 0xF000)
        {
            ch = value;
            /* XXX this blocks entire thread. Rewrite to use
             * qemu_chr_fe_write and background I/O callbacks */
            qemu_chr_fe_write_all(&s->chr, &ch, 1);
            /* XXX I/O are currently synchronous, making it impossible for
               software to observe transient states where TXE or TC aren't
               set. Unlike TXE however, which is read-only, software may
               clear TC by writing 0 to the SR register, so set it again
               on each write. */
            s->lpuart_sr |= LPUART_STAT_TDRE;
            nxps32k358_lpuart_update_irq(s);
        }
        return;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,"%s: NXP S32K3 LPUART: Unimplemented write to offset 0x%" HWADDR_PRIx " with value 0x%" PRIx32 "\n",
                      __func__, offset, value);        
    }
}


// FINAL FUNCTION FOR EACH VIRTUALIZED DEVICE
static const MemoryRegionOps nxps32k358_lpuart_ops = {
    .read = nxps32k358_lpuart_read,
    .write = nxps32k358_lpuart_write,
    .endianness = DEVICE_NATIVE_ENDIAN, // Verifica l'endianness delle periferiche S32K3
};

static const Property nxps32k358_lpuart_properties[] = {
    DEFINE_PROP_CHR("chardev", NXPS32K358LPUARTState, chr),
};

static void nxps32k358_lpuart_init(Object *obj)
{
    NXPS32K358LPUARTState *s = NXPS32K358_LPUART(obj);

        // Inizializza la linea IRQ
    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    // Inizializza la MemoryRegion per i registri della LPUART
    // La dimensione (es. 0x1000 o 4KB) deve coprire tutti i registri LPUART
    memory_region_init_io(&s->iomem, obj, &nxps32k358_lpuart_ops, s,
                          "nxps32k358-lpuart", 0x4000); // Dimensione esempio


    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);
    s->clk = qdev_init_clock_in(DEVICE(s), "clk", NULL, s, 0);



    // Inizializza il backend per i caratteri
    //qdev_prop_set_chr(DEVICE(obj), "chardev", qemu_chr_fe_get_driver(&s->chr)); // Permette di specificare -serial ...
                                                                                // o un altro chardev
}

static void nxps32k358_lpuart_realize(DeviceState *dev, Error **errp)
{
    NXPS32K358LPUARTState *s = NXPS32K358_LPUART(dev);
    if (!clock_has_source(s->clk)) {
        error_setg(errp, "LPUART clock must be wired up by SoC code");
        return;
    }
    // Connetti le funzioni di callback per la ricezione dei caratteri
    // dall'host QEMU al dispositivo emulato.
    qemu_chr_fe_set_handlers(&s->chr, nxps32k358_lpuart_can_receive,
                             nxps32k358_lpuart_receive, NULL, NULL, s, NULL, true);

    // Qui non serve connettere clock perché il clock viene passato dal SoC
    // al momento della connessione in nxps32k358_soc_realize
    // qdev_connect_clock_in(dev, "clk", some_clock_source);
}

static void nxps32k358_lpuart_class_init(ObjectClass *klass, const void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_legacy_reset(dc, nxps32k358_lpuart_reset);
    device_class_set_props(dc, nxps32k358_lpuart_properties);
    dc->realize = nxps32k358_lpuart_realize;
}

static const TypeInfo nxps32k358_lpuart_info = {
    .name = TYPE_NXPS32K358_LPUART,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(NXPS32K358LPUARTState),
    .instance_init = nxps32k358_lpuart_init,
    .class_init = nxps32k358_lpuart_class_init,
};

static void nxps32k358_lpuart_register_types(void)
{
    type_register_static(&nxps32k358_lpuart_info);
}

type_init(nxps32k358_lpuart_register_types);
