#include <screen.h>
#include <io.h>
#include <types.h>




struct screen_info screen_info;



/*
 * scroll screen by line-lines
 * @line: number of lines to scroll
 */
void scroll_screen(ssize_t line)
{
	ssize_t i;
	volatile u16 *src = &screen_info.video_mem[line*SCREEN_COL];

	if (line > SCREEN_ROW-1)
		return;

	// scroll X line => remove 0 ~ (X-1) lines
	// and move X ~ SCREEN_ROW lines to 0 ~ (X-1) lines
	for (i = 0; i < (SCREEN_ROW-line)*SCREEN_COL; i++)
		screen_info.video_mem[i] = src[i];

	// remain area is removed
	for (; i < SCREEN_ROW*SCREEN_COL; i++)
		screen_info.video_mem[i] = BG_BLACK;
			
}



/*
 * clear entire screen 
 */
void clear_screen(void)
{
	ssize_t i;

	// copy 4 byte per one loop
	for (i=0; i<SCREEN_ROW*SCREEN_COL; i++)
		screen_info.video_mem[i] = BG_BLACK;

	return;
}


/*
 * initialize struct screen_info, set cursor at the first point of screen.
 */
void init_screen(void)
{
		screen_info.video_mem = (volatile u16 *)VIDEO_ADDRESS;

		//
		// When C function starts, previous screen is deleted..
		screen_info.cursor_offset = 0;

		set_cursor(0);
		clear_screen();
}
		

/*
 * set cursor at A_dwNewCursorOffset-offset point
 * @A_dwNewCursorOffset: offset point from the starting point of screen
 */
void set_cursor(ssize_t A_dwNewCursorOffset)
{
		u8 dwTemp;

		// set cursor to new position
		screen_info.cursor_offset = A_dwNewCursorOffset;


		// if cursor go over screen
		if ( (screen_info.cursor_offset/SCREEN_COL) >= SCREEN_ROW) {
			scroll_screen(1);
			screen_info.cursor_offset -= SCREEN_COL;
		}

		// CRT port: 0x3d4 & 0x3d5
		dwTemp = (u8)(screen_info.cursor_offset >> 8);
		__outb((unsigned short)0x3d4, (unsigned char)0x0e);	// high byte
		__outb((unsigned short)0x3d5, (unsigned char)dwTemp);

		dwTemp = (u8)(screen_info.cursor_offset & (u8)0xff);
		__outb((unsigned short)0x3d4, 0x0f);				// low byte
		__outb((unsigned short)0x3d5, (u8)dwTemp);

}
						

