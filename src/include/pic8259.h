#ifndef __PIC8259_H__
#define __PIC8259_H__




void pic8259_init(void);
void enable_pic8259(size_t);
void disable_pic8259(size_t);
void ack_pic8259(void);


#endif
