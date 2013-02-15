#ifndef __MPCORE_H__
#define __MPCORE_H__

#include <types.h>
#include <processor.h>


//
//
// copy of linux/include/asm-x86_64/mpspec.h
//

/*
 * Structure definitions for SMP machines following the
 * Intel Multiprocessing Specification 1.1 and 1.4.
 */

/*
 * This tag identifies where the SMP configuration
 * information is. 
 */
 
#define SMP_MAGIC_IDENT	(('_'<<24)|('P'<<16)|('M'<<8)|'_')

/*
 * A maximum of 255 APICs with the current APIC ID architecture,
 * but Caos supports only 2 APICs.
 */
#define MAX_APICS 2 /* 255 */

#define MAX_IO_APICS 2 /* 128 */


struct intel_mp_floating
{
	char mpf_signature[4];		/* "_MP_" 			*/
	unsigned int mpf_physptr;	/* Configuration table address	*/
	unsigned char mpf_length;	/* Our length (paragraphs)	*/
	unsigned char mpf_specification;/* Specification version	*/
	unsigned char mpf_checksum;	/* Checksum (makes sum 0)	*/
	unsigned char mpf_feature1;	/* Standard or configuration ? 	*/
	unsigned char mpf_feature2;	/* Bit7 set for IMCR|PIC	*/
	unsigned char mpf_feature3;	/* Unused (0)			*/
	unsigned char mpf_feature4;	/* Unused (0)			*/
	unsigned char mpf_feature5;	/* Unused (0)			*/
} __attribute__ ((packed));

struct mp_config_table
{
	char mpc_signature[4];		// string "PCMP"
#define MPC_SIGNATURE "PCMP"
	unsigned short mpc_length;	/* Size of table */
	char  mpc_spec;			/* 0x01 or 0x04 */
	char  mpc_checksum;		// checksum of the table
	char  mpc_oem[8];		// OEM ID: system hardware manufacturer
	char  mpc_productid[12];	// Product familly ID
	unsigned int mpc_oemptr;	/* 0 if not present, optional */
	unsigned short mpc_oemsize;	/* 0 if not present, optional */
	unsigned short mpc_oemcount;	// entry count
	unsigned int mpc_lapic;	/* Local APIC address */
	unsigned int reserved;
} __attribute__ ((packed));

/* Followed by entries */

#define	MP_PROCESSOR	0
#define	MP_BUS		1
#define	MP_IOAPIC	2
#define	MP_INTSRC	3
#define	MP_LINTSRC	4

struct mpc_config_processor
{
	unsigned char mpc_type;		// A value of 0 identifies a processor entry
	unsigned char mpc_apicid;	/* Local APIC ID number */
	unsigned char mpc_apicver;	/* Local APIC's versions */
	unsigned char mpc_cpuflag;
#define CPU_ENABLED		1	/* Processor is available */
#define CPU_BOOTPROCESSOR	2	/* Processor is the BP */
	unsigned int mpc_cpufeature;		
#define CPU_STEPPING_MASK 0x0F
#define CPU_MODEL_MASK	0xF0
#define CPU_FAMILY_MASK	0xF00
	unsigned int mpc_featureflag;	/* CPUID feature value */
	unsigned int mpc_reserved[2];
} __attribute__ ((packed));

struct mpc_config_bus
{
	unsigned char mpc_type;
	unsigned char mpc_busid;
	unsigned char mpc_bustype[6];
} __attribute__ ((packed));

/* List of Bus Type string values, Intel MP Spec. */
#define BUSTYPE_EISA	"EISA"
#define BUSTYPE_ISA	"ISA"
#define BUSTYPE_INTERN	"INTERN"	/* Internal BUS */
#define BUSTYPE_MCA	"MCA"
#define BUSTYPE_VL	"VL"		/* Local bus */
#define BUSTYPE_PCI	"PCI"
#define BUSTYPE_PCMCIA	"PCMCIA"
#define BUSTYPE_CBUS	"CBUS"
#define BUSTYPE_CBUSII	"CBUSII"
#define BUSTYPE_FUTURE	"FUTURE"
#define BUSTYPE_MBI	"MBI"
#define BUSTYPE_MBII	"MBII"
#define BUSTYPE_MPI	"MPI"
#define BUSTYPE_MPSA	"MPSA"
#define BUSTYPE_NUBUS	"NUBUS"
#define BUSTYPE_TC	"TC"
#define BUSTYPE_VME	"VME"
#define BUSTYPE_XPRESS	"XPRESS"

struct mpc_config_ioapic
{
	unsigned char mpc_type;		// must be 2
	unsigned char mpc_apicid;	// ID if this IO APIC
	unsigned char mpc_apicver;	// version
	unsigned char mpc_flags;	// usable flag
#define MPC_APIC_USABLE		0x01
	unsigned int mpc_apicaddr;	// base address of this IO APIC
} __attribute__ ((packed));

struct mpc_config_intsrc
{
	unsigned char mpc_type;		// must be 3
	unsigned char mpc_irqtype;	// interrupt type value
	unsigned short mpc_irqflag;	// int polarity and trigger mode
	/* bits[1:0]: polarity
	 * 00 = conforms to specifications of bus
	 * 01 = active high
	 * 10 = reserved
	 * 11 = active low
	 * bits[3:2]: trigger mode
	 * 00 = conforms to specifications of bus
	 * 01 = edge
	 * 10 = reserved
	 * 11 = level
	 */
	unsigned char mpc_srcbus;	// bus id which the int comes from
	unsigned char mpc_srcbusirq;	// vector onto source bus signals
	unsigned char mpc_dstapic;		// IO APIC id the signal connected
	unsigned char mpc_dstirq;		// INTINn pin of IO APIC the signal connected
} __attribute__ ((packed));

enum mp_irq_source_types {
	mp_INT = 0,// vectored interrupt, vector is supplied by IO APIC
	mp_NMI = 1,// nonmaskable int
	mp_SMI = 2,// system management int
	mp_ExtINT = 3// vectored int, vector is supplied by externam PIC
};

#define MP_IRQDIR_DEFAULT	0
#define MP_IRQDIR_HIGH		1
#define MP_IRQDIR_LOW		3


struct mpc_config_lintsrc
{
	unsigned char mpc_type;		// entry type 4
	unsigned char mpc_irqtype;	// same value with IO interrupt entry
	unsigned short mpc_irqflag;	// same with IO interrupt entry
	unsigned char mpc_srcbusid;	// bus ID the interrupt came from
	unsigned char mpc_srcbusirq; // vector from the bus
	unsigned char mpc_destapic;		// Local APIC ID the signal connected
#define MP_APIC_ALL	0xFF
	unsigned char mpc_destapiclint;	// LINTINn pin the signal connected
} __attribute__ ((packed));

/*
 *	Default configurations
 *
 *	1	2 CPU ISA 82489DX
 *	2	2 CPU EISA 82489DX neither IRQ 0 timer nor IRQ 13 DMA chaining
 *	3	2 CPU EISA 82489DX
 *	4	2 CPU MCA 82489DX
 *	5	2 CPU ISA+PCI
 *	6	2 CPU EISA+PCI
 *	7	2 CPU MCA+PCI
 */

#define MAX_MP_BUSSES 256

#define MAX_IRQ_SOURCES (MAX_MP_BUSSES*4)



void mp_spec_init(void);


extern ssize_t smp_found_config;	// MP configuration is found?
extern u64 mp_lapic_addr;			// address of Local APIC of BSP
extern size_t disabled_cpus;		// 
extern size_t boot_cpu_id;			// BSP index
extern size_t num_processors;		// processor count
extern size_t mp_current_pci_id;	// pci bus index
extern size_t nr_ioapics;			// IO APIC count
extern size_t mp_irq_entries;

// 
// bitmap for system configuration
//
extern u64 cpu_present_map;	// each bit indicates a processor present
extern DECLARE_BITMAP(mp_bus_not_pci, MAX_MP_BUSSES);
// ID tables
extern struct intel_mp_floating *mpf_found;	// address of MP floating pointer
// APIC ID of each logical processor
extern u8 bios_cpu_apicid[NR_CPUS];
// IO APIC entry of MP configuration table
extern struct mpc_config_ioapic mp_ioapics[MAX_IO_APICS];
extern struct mpc_config_intsrc mp_irqs[MAX_IRQ_SOURCES];
extern size_t mp_bus_id_to_pci_bus[MAX_MP_BUSSES];


#endif
