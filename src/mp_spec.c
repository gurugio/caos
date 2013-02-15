
#include <mp_spec.h>	// 

#include <io.h>		// virt_to_phys, phys_to_virt
#include <string.h> // memcmp
#include <printf.h>
#include <processor.h> // struct cpuinfo_x86
#include <bitops.h> 	// set_bit, ffz
#include <apic.h>	// x86_cpu_to_apicid
#include <types.h> // DECLARE_BITMAP



#define DEBUG 1
#undef DEBUG


//
// copy of linux/arch/x86_64/mpparse.c
//
ssize_t smp_found_config;	// MP configuration is found?
u64 mp_lapic_addr;			// address of Local APIC of BSP
size_t disabled_cpus;		// 
size_t boot_cpu_id;			// BSP index
size_t num_processors;		// processor count
size_t mp_current_pci_id;	// pci bus index
size_t nr_ioapics;			// IO APIC count
size_t mp_irq_entries;

// 
// bitmap for system configuration
//
u64 cpu_present_map;	// each bit indicates a processor present
DECLARE_BITMAP(mp_bus_not_pci, MAX_MP_BUSSES);


// ID tables
struct intel_mp_floating *mpf_found;	// address of MP floating pointer

// APIC ID of each logical processor
u8 bios_cpu_apicid[NR_CPUS] = { [0 ... NR_CPUS-1] = 0xFF };

// IO APIC entry of MP configuration table
struct mpc_config_ioapic mp_ioapics[MAX_IO_APICS];

struct mpc_config_intsrc mp_irqs[MAX_IRQ_SOURCES];
size_t mp_bus_id_to_pci_bus[MAX_MP_BUSSES] = { [0 ... MAX_MP_BUSSES-1] = -1 };



//
// return first zero bit index that is the cpu_id
// @map: cpu present bitmap
//
static size_t first_cpu(size_t map)
{
	return ffz(map);

}

//
// set cpu-index bit in the cpu present bitmap
// @cpu_id: cpu index
// @map: address of the bitmap
static void cpu_set(size_t cpu_id, u64 *map)
{
	set_bit(cpu_id, (void *)map);
}


//
// search MP Floating Table 
// and read physical address of MP Configuration Table
// @base: base address of the search area 
// @length: length of the search area
//
ssize_t smp_scan_config(size_t base, size_t length)
{
	u32 *basep = phys_to_virt(base);
	struct intel_mp_floating *mpf;

#ifdef DEBUG
	caos_printf("scan SMP pointer table: %x~%x\n", basep, (size_t)basep+length);
#endif

	while (length > 0) {
		mpf = (struct intel_mp_floating *)basep;

		if ((*basep == SMP_MAGIC_IDENT) &&
				(mpf->mpf_length == 1) && // this should be 1
				((mpf->mpf_specification == 1) || (mpf->mpf_specification == 4))) {
			smp_found_config = 1;
			
#ifdef DEBUG
			caos_printf("Reserve SMP configuration table %x\n",
					mpf->mpf_physptr);
#endif

			mpf_found = mpf;	// mpf_found is a global variable

			return CAOS_TRUE;

		}

		basep += 4;
		length -= 16;


	}


	return CAOS_FALSE;
}


//
// read processor entries of the MP Configuration Table
// @m: base address of the entry
//
void MP_processor_info(struct mpc_config_processor *m)
{
	size_t cpu;
	char *bootup_cpu = "";


	if ((m->mpc_cpuflag & CPU_ENABLED) == 0) {
		disabled_cpus++;
		return;
	}


	// check bootstrap processor
	if (m->mpc_cpuflag & CPU_BOOTPROCESSOR) {
		bootup_cpu = " (Bootup-CPU)";
		boot_cpu_id = m->mpc_apicid;
	}


	caos_printf("Processor #%d%s is found\n", m->mpc_apicid, bootup_cpu);


	if (num_processors >= NR_CPUS) {
		caos_printf("MPCTable: NR_CPUS limit of %d reached..processor ignored\n", NR_CPUS);
		return;
	}

	num_processors++;


	// cpu_present_map is a 64bit variable
	// each bit indicates a processor present
	cpu = first_cpu(cpu_present_map);

	if (m->mpc_cpuflag & CPU_BOOTPROCESSOR) {
		//
		// This is BSP (Bootstrap Processor)!!
		// bios_cpu_apicid structure has processor list in order as
		// logial cpu numbers.
		//
		cpu = 0;
	}


	// each processor's Local APIC ID
	// I don't know difference of bios_cpu_apicid and x86_cpu_to_apicid,
	// but the linux kernel has both of them.
	bios_cpu_apicid[cpu] = m->mpc_apicid;
	x86_cpu_to_apicid[cpu] = m->mpc_apicid;

    caos_printf("cpu=%d apicid=%d\n", cpu, m->mpc_apicid);

	cpu_set(cpu, &cpu_present_map);

}



//
// read bus entries of the MP Configuration Table
// @m: base address of the entry
//
void MP_bus_info(struct mpc_config_bus *m)
{
	char str[7];

	// read bus type string identifies the type of bus
	// such as EISA, ISA, MCA, PCI, PCMCIA...
	caos_memcpy(str, m->mpc_bustype, 6);
	str[3] = 0;

#ifdef DEBUG
	caos_printf("Bus #%d is %s\n", m->mpc_busid, str);
#endif

	// only ISA and PCI bus are identified
	// the others are ignored.
	if (caos_strcmp(str, "ISA") == 0) {
		set_bit(m->mpc_busid, mp_bus_not_pci);
	} else if (caos_strcmp(str, "PCI") == 0) {
		clear_bit(m->mpc_busid, mp_bus_not_pci);
		// store PCI bus ID
		mp_bus_id_to_pci_bus[m->mpc_busid] = mp_current_pci_id;
		mp_current_pci_id++;
	} else {
		str[6] = 0;
		caos_printf("MPCTable: Unknown bustype detected: %s\n", str);
	}
	

}



//
// read IO-APIC entries of the MP Configuration Table
// @m: base address of the entry
//
void MP_ioapic_info(struct mpc_config_ioapic *m)
{

    /* search enable APIC */
	if ((m->mpc_flags & MPC_APIC_USABLE) == 0)
		return;

	caos_printf("I/O APIC #%d at 0x%x\n", 
			m->mpc_apicid, m->mpc_apicaddr);


	if (nr_ioapics >= MAX_IO_APICS || m->mpc_apicaddr == 0) {
		caos_printf("I/O APIC error detected..\n");
		return;
	}

    /* ID of ioapic can begin at non-zero number. */
	mp_ioapics[nr_ioapics] = *m;
	nr_ioapics++;


}



//
// read IO Interrupt entries of the MP Configuration Table
//
// These entries indicate which interrpt source is connected to
// each IO APIC interrupt input. There is one entry for each IO APIC
// interrupt input that is connected.
// @m: base address of the entry
//
void MP_intsrc_info(struct mpc_config_intsrc *m)
{
	mp_irqs[mp_irq_entries] = *m;

#ifdef DEBUG
    caos_printf("INT : type %d, pol %d, trig %d, bus %d,",
                m->mpc_irqtype, m->mpc_irqflag & 3, (m->mpc_irqflag >> 2) & 3, m->mpc_srcbus);
    caos_printf(" IRQ %x, APIC ID %d, APIC INT %d\n",
                m->mpc_srcbusirq, (m->mpc_dstapic & 0xF), (u8)((m->mpc_dstirq) & 0xF));
#endif

	if (++mp_irq_entries >= MAX_IRQ_SOURCES) {
		caos_printf("Max # of irq sources exceeded!\n");
		halt();
	}


}



//
// read Local Interrupt entries of the MP Configuration Table
//
// There entries tell what interrupt source is connected to each local
// interrupt input of each Local APIC.
// @m: base address of the entry
//
void MP_lintsrc_info(struct mpc_config_lintsrc *m)
{
#ifdef DEBUG
	caos_printf("LINT : type %d, pol %d, trig %d, bus %d,",
                m->mpc_irqtype, m->mpc_irqflag & 3,	(m->mpc_irqflag >> 2) & 3, m->mpc_srcbusid);
	caos_printf(" IRQ %x, APIC ID %x, APIC INT %x\n",
                m->mpc_srcbusirq, m->mpc_destapic, m->mpc_destapiclint);
#endif
    m = m; /* remove compiler warning */
}




//
// read information of the MP Configuration Table
// such as number of processor core, IOAPIC map, PCI interrupts
// @mpc: linear address of the MP Configuration Table
//
int smp_read_mpc(struct mp_config_table *mpc)
{
    // count processing data
	ssize_t count = sizeof(*mpc);
	
	// pointer to entry type stored in MP configuration table entry
	// The first byte of entry indicates entry type.
	unsigned char *mpt = ((unsigned char *)mpc) + count;

	// read signature "PCMP"
	if (caos_memcmp(mpc->mpc_signature, MPC_SIGNATURE, 4) != 0) {
		caos_printf("MPCTable: Bad signature[%c%c%c%c]\n",
				mpc->mpc_signature[0],
				mpc->mpc_signature[1],
				mpc->mpc_signature[2],
				mpc->mpc_signature[3]);
		return CAOS_FALSE;
	}

	// MP specification version 1.1 or 1.4
	if (mpc->mpc_spec != 0x01 && mpc->mpc_spec != 0x04) {
		caos_printf("MPCTable: bad table version (%d)\n", mpc->mpc_spec);
		return CAOS_FALSE;
	}

	// Local APIC address must exist
	if (mpc->mpc_lapic == 0) {
		caos_printf("MPCTable: null local APIC address\n");
		return CAOS_FALSE;
	}

	// Local APIC
	mp_lapic_addr = mpc->mpc_lapic;

	
#ifdef DEBUG
	do {
		char str[16];
		caos_memcpy(str, mpc->mpc_oem, 8);
		str[8] = 0;

		caos_memcpy(str, mpc->mpc_productid, 12);
		str[12] = 0;

		caos_printf("MPCTable: Product ID: %s\n", str);
		caos_printf("MPCTable: OEM ID: %s ", str);
		caos_printf("MPCTable: Local APIC at: 0x%x\n", mpc->mpc_lapic);
        caos_printf("MPCTable: Base table length: %d\n", mpc->mpc_length);
	} while (0);
#endif



	//
	// read entries in the table
	//
	mpt = ((unsigned char *)mpc) + count; /* start point of tables */
	while (count < mpc->mpc_length) {
		switch (*mpt) { // read type information, 
			// one entry per processor
			case MP_PROCESSOR:
			{
				struct mpc_config_processor *m = (struct mpc_config_processor *)mpt;

				MP_processor_info(m);
				mpt += sizeof(*m);
				count += sizeof(*m);
				break;
			}
			// one entry per bus
			case MP_BUS:
			{
				struct mpc_config_bus *m = (struct mpc_config_bus *)mpt;

				MP_bus_info(m);
				mpt += sizeof(*m);
				count += sizeof(*m);
				break;
			}
			// one entry per IO APIC
			case MP_IOAPIC:
			{
				struct mpc_config_ioapic *m = (struct mpc_config_ioapic *)mpt;

				MP_ioapic_info(m);
				mpt += sizeof(*m);
				count += sizeof(*m);
				break;
			}
			// one entry per bus interrupt source
			case MP_INTSRC:
			{
				struct mpc_config_intsrc *m = (struct mpc_config_intsrc *)mpt;

				MP_intsrc_info(m);
				mpt += sizeof(*m);
				count += sizeof(*m);
				break;
			}
			// one entry per system interrupt source
			case MP_LINTSRC:
			{
				struct mpc_config_lintsrc *m = (struct mpc_config_lintsrc *)mpt;

				MP_lintsrc_info(m);
				mpt += sizeof(*m);
				count += sizeof(*m);
				break;
			}
		}
	}


#ifdef DEBUG
	caos_printf("MPCTable: success to read tables!\n");
#endif

	return CAOS_TRUE;

}





//
// init SMP facility
//
// Read MP floating pointer and then read MP configuration table,
// the number of processor and IO APIC address, system interrupt signals
// are identified.
//
void mp_spec_init(void)
{
	struct intel_mp_floating *mpf;

	//
	// MP floating pointer structure is in one of followings
	//
	// 1) the fist 1K area
	// 2) 639K ~ 640K
	// 3) 0xF0000 ~ 0xFFFFF
	//
	if (smp_scan_config(0x0, 0x400) ||
			smp_scan_config(639*0x400, 0x400) ||
			smp_scan_config(0xf0000, 0x10000)) {
		mpf = mpf_found;
#ifdef DEBUG
		caos_printf("MP floating pointer structure is found at 0x%x.\n", mpf);
#endif
	} else {
		caos_printf("No MP floating pointer structure is found\n");
		halt();
		return;
	}
		



	caos_printf("Intel Multiprocessor Specification v1.%d\n", mpf->mpf_specification);


	
	// init cpu info
	disabled_cpus = 0;
	boot_cpu_id = 0;
	num_processors = 0;

	cpu_present_map = 0;
	mp_current_pci_id = 0;
	nr_ioapics = 0;
	mp_irq_entries = 0;

	caos_memset(mp_ioapics, 0, sizeof(mp_ioapics));
	caos_memset(mp_irqs, 0, sizeof(mp_irqs));




	//
	// read MP configuration tables
	//

	if (mpf->mpf_feature1 == 0 && mpf->mpf_physptr != 0) {
#ifdef DEBUG
		caos_printf("read MP configure at 0x%x\n", mpf->mpf_physptr);
#endif

		if (smp_read_mpc(phys_to_virt(mpf->mpf_physptr)) == 0) {
			caos_printf("BIOS bus, MP table error detected!!!\n");
			halt();
		}
	} else if (mpf->mpf_feature1 != 0) {
		caos_printf("Default MP configurationg processing is not implemented yet!!!\n");
		halt();
	} else {
		caos_printf("SMP structure ERROR\n");
		halt();
	}

}


