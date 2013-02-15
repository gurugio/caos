#ifndef __SCREEN_H__

#define __SCREEN_H__

#include <types.h>
#include <pgtable.h>

#define VIDEO_ADDRESS (PAGE_OFFSET+0xb8000UL)
#define SCREEN_ROW 25
#define SCREEN_COL 80
#define ONECHAR_SIZE 2
//#define ONELINE_SIZE (SCREEN_COL * ONECHAR_SIZE)

#define BG_BLACK ((unsigned short)0x0700)


void init_screen(void);
void set_cursor(ssize_t);

void scroll_screen(ssize_t);
void clear_screen(void);


struct screen_info {
	size_t cursor_offset;
	volatile u16 *video_mem;

};

extern struct screen_info screen_info;



static inline void set_screen(size_t offset, u8 c)
{
	
	screen_info.video_mem[offset] = (BG_BLACK | c);

}





#endif
