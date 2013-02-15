


//
// IO APIC setting is under constructing.
// 
// WARNING! Never use this file till it's completed.
//


#include <io_apic.h>
#include <irq_vector.h>
#include <irq_handler.h> // IRQ_TO_VECTOR

static struct io_apic *io_apic_base(void)
{
	return (void *)IO_APIC_BASE;
}

static inline unsigned int io_apic_read(unsigned int reg)
{
	struct io_apic *io_apic = io_apic_base();
	writel(reg, &io_apic->index);
	return readl(&io_apic->data);
}

static inline void io_apic_write(unsigned int reg, unsigned int value)
{
	struct io_apic  *io_apic = io_apic_base();
	writel(reg, &io_apic->index);
	writel(value, &io_apic->data);
}

/*
 * Re-write a value: to be used for read-modify-write
 * cycles where the read already set up the index register.
 */
static inline void io_apic_modify(unsigned int value)
{
	struct io_apic  *io_apic = io_apic_base();
	writel(value, &io_apic->data);
}

/*
 * Synchronize the IO-APIC and the CPU by doing
 * a dummy read from the IO-APIC
 */
static inline void io_apic_sync(void)
{
	struct io_apic  *io_apic = io_apic_base();
	readl(&io_apic->data);
}


int io_apic_enable_irq(size_t irq)
{
    u64 tbl = 0;
	ssize_t entry;
    
	entry = GET_IO_APIC_ENTRY(io_apic_read(IO_APIC_VERSION));

    if ((ssize_t)irq > entry)
        return -1;

    tbl |= SET_IO_APIC_DEST(0x0UL); // destination cpu has #0 APIC ID
    tbl &= ~IO_APIC_IRQ_MASK;  // enable INTR
    tbl |= IO_APIC_TRIG_MODE;// 1:level 0:edge
    tbl |= IO_APIC_INTPOL;// 1:active low, 0: active high

    tbl &= ~IO_APIC_DESTMOD; // 1:logical 0:physical
    tbl |= IO_APIC_DELMOD_FIX; // send INTR to destination cpu


    // Interrupt vector 0x0~0x1F is reserved for processor exceptions.
    // Therefore vector of external devices must be larger than 0x20.
    // Local APIC will occur error handling interrupt if vector 0x0~0x1F is sent.
    tbl |= IRQ_TO_VECTOR(irq);// IRQ vector
        
    io_apic_write(IO_APIC_REDTBL_BASE + irq*2, tbl);

    return CAOS_SUCCESS;
}



void io_apic_init(void)
{

    volatile int delay;
    
    // write IMCR with 0x1 to enable IO APIC
    // This sets IO APIC as Symmetric I/O Mode (introduced at MP Spcification)
    outb(0x22, 0x70); // select register
    for (delay = 0; delay != 0x100000; delay++)
        continue;
    outb(0x23, 0x1); // write command


	caos_printf("IO APIC ID=%x\n", (io_apic_read(0x0)>>24) & 0xF);
	caos_printf("IO APIC VER=%x ", GET_IO_APIC_VERSION(io_apic_read(IO_APIC_VERSION)) );
	caos_printf("IO APIC Entry=%d\n", (GET_IO_APIC_ENTRY(io_apic_read(0x1))) );

    // activating IRQs are done by device's driver with calling
   // io_apic_enable_irq
#if 0
    // moved to io_apic_enable_irq()
	for (i = 0; i <= entry; i++) {

        tbl = io_apic_read(IO_APIC_REDTBL_BASE + i*2);

        tbl |= SET_IO_APIC_DEST(0x0UL); // destination cpu has #0 APIC ID
		tbl |= IO_APIC_IRQ_MASK;  // mask all, it will be enabled when register device
		tbl |= IO_APIC_TRIG_MODE;//&= ~IO_APIC_TRIG_MODE; // edge
        tbl |= IO_APIC_INTPOL;//&= ~IO_APIC_INTPOL; // high active

        tbl &= ~IO_APIC_DESTMOD; // physical mode + APIC ID // |= IO_APIC_DESTMOD; //loginal mode + set of processors
		tbl |= IO_APIC_DELMOD_FIX; //ExtINT;	// ExtINT


        // vector values range from 0x10 to 0xEF
        // example) keyboard IRQ#1 -> cpu accepts IRQ #(1+VECTOR_BASE)
        tbl |= IRQ_TO_VECTOR(i);// + CPU_IRQ_BASE; // vector
        
		io_apic_write(IO_APIC_REDTBL_BASE + i*2, tbl);
    }
#endif
}


