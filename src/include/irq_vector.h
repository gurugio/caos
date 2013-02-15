#ifndef __IRQ_H__
#define __IRQ_H__




//
// IRQ numbers and vector numbers of devices
//






// external devices
#define DEVICE_IRQ_BASE 0x0
#define  EXTERNAL_TIMER (DEVICE_IRQ_BASE + 0x0)
#define  KEYBOARD_IRQ (DEVICE_IRQ_BASE + 0x1)


// CPU internal
#define CPU_IRQ_BASE 0x10
#define BSP_TIMER_IRQ (CPU_IRQ_BASE+0x0)
#define LINT0_IRQ (CPU_IRQ_BASE+0x1)
#define LINT1_IRQ (CPU_IRQ_BASE+0x2)
#define SPURIOUS_IRQ (CPU_IRQ_BASE+0x3)


#endif
