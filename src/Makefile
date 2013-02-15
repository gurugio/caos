# How to compile 64bit kernel refers to http://wiki.osdev.org/Creating_a_64-bit_kernel
# and the other options refer gcc manual & http://www.osx86.org


include ../config.mk


#
# local definitions
#

# linear address where kernel exists
KERNEL_VIRTUAL_ADDR=0xFFFF800000100000
KERNEL_STACK_ADDR=0xFFFF800000200000

# physical memory size (MIN 16MB ~ MAX 512MB)
MEMORY_SIZE=128


#
# compile option
#

INCLUDE=./include
CFLAGS=-Wall -Wextra -Winline -I$(INCLUDE) -fno-stack-protector -O2 -nostdinc -nostdlib -m64 -ffreestanding -nostdlib -mcmodel=large -DKERNEL_VIRTUAL_ADDR=$(KERNEL_VIRTUAL_ADDR) -DKERNEL_STACK_ADDR=$(KERNEL_STACK_ADDR) -c




#
# link option
#

LFLAGS=-m elf_x86_64 -nodefaultlibs -nostdlib -Map link_map.txt --defsym KERNEL_VMA=$(KERNEL_VIRTUAL_ADDR) --defsym KERNEL_STACK_START=$(KERNEL_STACK_ADDR)


#
# assemble option
# 

NASMFLAG=-f elf64


#
# bootloader
#

BOOT_SRCS = bootsect.asm setup.asm


#
# kernel
#

C_SRCS = main.c screen.c printf.c string.c setup.c page.c memory.c gdt.c idt.c except_handler.c page_alloc.c slab.c except_handler.c mp_spec.c apic.c irq_handler.c io_apic.c pic8259.c smp.c keyboard.c percpu.c spinlock.c process.c
ASM_SRCS = except_entry.asm irq_entry.asm
C_OBJS = $(C_SRCS:.c=.o)
ASM_OBJS = $(ASM_SRCS:.asm=.o)



#
# binary image
#

BOOT_IMG = boot.img
KERNEL_IMG = kernel.img
FLOPPY_IMG = floppy.img
BOCHS_IMG = bochs.img


#
# system.map
#
MAP=system.map

#
# BUILD
#

all:$(FLOPPY_IMG)
	objdump -d kernel.elf > disasm.txt
	cp floppy.img system.map disasm.txt ../image


$(FLOPPY_IMG):$(KERNEL_IMG) $(MAP) $(BOOT_IMG)
	cat $(BOOT_IMG) $(KERNEL_IMG) setup.img > $@


$(KERNEL_IMG):$(C_OBJS) $(ASM_OBJS)
	$(LD64) $(LFLAGS) -o kernel.elf -T ld-script.ld  $^
	$(OBJCOPY64) -j .text -j .rodata -j .data -O binary kernel.elf $@


%.o : %.c
	$(GCC64) $(CFLAGS) $<


%.o : %.asm
	$(NASM) -f elf64 -o $@ $<


$(MAP):kernel.elf
	$(NM) kernel.elf > system.map


$(BOOT_IMG):$(BOOT_SRCS) $(MAP) $(KERNEL_IMG)
	../utils/gen_section ./system.map ./kernel_section.inc
	KERNEL_SIZE=`ls -l kernel.img | awk '{ print $$5 }'`; \
	KERNEL_SIZE=`expr \( $$KERNEL_SIZE + 512 \) / 512`;	\
	echo "Kernel size(sectors) = " $$KERNEL_SIZE;	\
	$(NASM) -DKERNEL_SIZE=$$KERNEL_SIZE -o bootsect.img bootsect.asm;\
	$(NASM) -DKSIZE=$$KERNEL_SIZE -DMSIZE=$(MEMORY_SIZE) -o setup.img setup.asm
	cat bootsect.img setup.img > $@


BOCHS:$(FLOPPY_IMG)
	$(DD) if=floppy.img of=bochs.img bs=1440K conv=sync
	echo "Floppy image for 1.44MB dist is created..bochs.img"
	cp bochs.img ../image


clean:
	rm -f *.o *.img *.elf *.txt *.map cscope.* tags *.out

