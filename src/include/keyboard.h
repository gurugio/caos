/********************************************************************
 * DESCRIPTION :
 * FILE NAME : keyboard.h
 *******************************************************************/

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__


#include <types.h>






#define MK_SHIFT (1<<0)
#define MK_CTRL (1<<1)
#define MK_ALT (1<<2)
#define MK_CAPSLOCK (1<<3)
#define MK_SCROLLLOCK (1<<4)
#define MK_NUMLOCK (1<<5)



#define SCAN_NUMLOCK 0x45
#define SCAN_CAPSLOCK 0x1d
#define SCAN_SCROLLLOCK 0x46
//#define SCAN_RIGHTSHIFT 0x36 // prefix 0xe0
#define SCAN_LEFTSHIFT 0x2a
#define SCAN_LEFTALT 0x38
#define SCAN_LEFTCTRL 0x3a
//#define SCAN_RIGHTALT SCAN_LEFTALT // prefix 0xe0

#define SCAN_ENTER 0x1c
#define SCAN_BACKSPACE 0x0e
#define SCAN_TAB 0x0f
#define SCAN_KOR 0x72

#define SCAN_ESC 0x1

#define SCAN_F1 0x3b
#define SCAN_F2 0x3c
#define SCAN_F3 0x3d
#define SCAN_F4 0x3e
#define SCAN_F5 0x3f
#define SCAN_F6 0x40
#define SCAN_F7 0x41
#define SCAN_F8 0x42
#define SCAN_F9 0x43
#define SCAN_F10 0x44
#define SCAN_F11 0x57
#define SCAN_F12 0x58

//
// Blow are need prefix 0xe0
#define SCAN_INSERT 0x52
#define SCAN_HOME 0x47
#define SCAN_PAGEUP 0x49
#define SCAN_DELETE 0x53
#define SCAN_END 0x4f
#define SCAN_PAGEDOWN 0x51
#define SCAN_UP 0x48
#define SCAN_DOWN 0x50
#define SCAN_LEFT 0x4b
#define SCAN_RIGHT 0x4d

#define SCAN_DIVIDE 0x35	// prefix 0xe0
#define SCAN_MULTIPLY 0x37
#define SCAN_SUBTRACT 0x4a
#define SCAN_ADD 0x4e
#define SCAN_SMALLENTER 0x1c // prefix 0xe0
#define SCAN_POINT 0x53
#define SCAN_PAD_0 0x52
#define SCAN_PAD_1 0x4f
#define SCAN_PAD_2 0x50
#define SCAN_PAD_3 0x51
#define SCAN_PAD_4 0x4b
#define SCAN_PAD_5 0x4c
#define SCAN_PAD_6 0x4d
#define SCAN_PAD_7 0x47
#define SCAN_PAD_8 0x48
#define SCAN_PAD_9 0xc9



struct key_tag_t {
	ssize_t status;
	u8 code;
};

ssize_t keyboard_init(void);

void keyboard_isr(size_t);

#endif
