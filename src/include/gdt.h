
#ifndef	__GDT_H__
#define	__GDT_H__

#include <types.h>


#define __KERNEL_CS32 0x08
#define __KERNEL_DS32 0x10
#define __KERNEL_CS64 0x18
#define __KERNEL_DS64 0x20


// copy of linux/include/asm-x86_64/desc_defs.h
// 8byte descriptor
struct gdt_desc {
	u32 l,h;
} __attribute__((packed));


struct gdt_desc_ptr {
	u16 size;	// 16bit table limit
	u64 address;	// 64bit linear address
} __attribute__ ((packed));


/*
 * Size of io_bitmap.
 */
#define IO_BITMAP_BITS  65536
#define IO_BITMAP_BYTES (IO_BITMAP_BITS/8)
#define IO_BITMAP_LONGS (IO_BITMAP_BYTES/sizeof(long))
#define IO_BITMAP_OFFSET offsetof(struct tss_struct,io_bitmap)
#define INVALID_IO_BITMAP_OFFSET 0x8000



struct tss_struct {
	u32 reserved1;
	u64 rsp0;	
	u64 rsp1;
	u64 rsp2;
	u64 reserved2;
	u64 ist[7];
	u32 reserved3;
	u32 reserved4;
	u16 reserved5;
	u16 io_bitmap_base;
	/*
	 * The extra 1 is there because the CPU will access an
	 * additional byte beyond the end of the IO permission
	 * bitmap. The extra byte must be all 1 bits, and must
	 * be within the limit. Thus we have:
	 *
	 * 128 bytes, the bitmap itself, for ports 0..0x3ff
	 * 8 bytes, for an extra "long" of ~0UL
	 */
	unsigned long io_bitmap[IO_BITMAP_LONGS + 1];
} __attribute__((packed)) ____cacheline_aligned;





void gdt_init(u64);
void set_gdt_desc(size_t num, u32 addr, u32 offset, flag_t type );

/* set task state segment... */
#define	set_tss_desc(num,addr)	set_gdt_desc(num,addr,235,0x89)

void load_segments(void);

#endif
