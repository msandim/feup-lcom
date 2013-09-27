#include <minix/drivers.h>
#include <sys/video.h>
#include <sys/mman.h>

#include <assert.h>
#include <stdbool.h>
#include <math.h>

#include "vt_info.h"

#include "video_txt.h"

/* Private global variables */

static char *video_mem;		/* Address to which VRAM is mapped */

static unsigned scr_width;	/* Width of screen in columns */
static unsigned scr_lines;	/* Height of screen in lines */

void vt_fill(char ch, char attr) {


	// get the address of the VRAM
	char* char_address = video_mem;

	unsigned int i;

	for (i = 0; i < scr_width * scr_lines; i++)
	{
		(*char_address) = ch; // puts the wanted char
		char_address++; // incs the pointer to the next char (attributes)
		(*char_address) = attr;
		char_address++; // incs the pointer to the char (next line/column
	}

}

void vt_blank() {

	// filling the screen with black smiley faces in a black background
	vt_fill(0x01, 0x00);
}

int vt_print_char(char ch, char attr, int r, int c) {
	// NOTE: "r" and "c" start in 0 and go to scr_"height/width"-1

	if (r >= 0 && r < scr_lines && c >= 0 && c < scr_width)
	{ // if the number is valid

		char* char_address = video_mem;

		// sum the lines before the line we are putting the char on
		char_address += r * scr_width * 2;

		// sum the columns to the one where we put our char on
		char_address += c * 2;

		*char_address = ch;
		char_address ++;
		*char_address = attr;

		return 0;
	}

	else // if it's not valid, we return 1
	{
		printf("\n\nvt_print_char error: Invalid position\n\n");
		return 1;
	}

}

int vt_print_string(char *str, char attr, int r, int c) {

	unsigned int count = strlen(str);
	unsigned int charsleft = (scr_lines-r-1)*scr_width + (scr_width-c);

	if (count > charsleft){
		printf("\n\nvt_print_string error: There is not enough space for that string\n\n");
		return 1;
	}

	//Checks if the input is valid
	else if (r < scr_lines && c < scr_width && c >= 0 && r >= 0)
	{
		//Start by increasing the pointer to the starting point
		char* char_Adress = video_mem + (scr_width * r * 2) + (c*2);

		// Loop the string to print each character, only breaks if we find the
		// string terminator
		do{
			// print the char
			vt_print_char(*str, attr, r, c);
			*str ++; // points to the next element

			if (c == scr_width-1){ // change row for next element
				r++;
				c=0;
			}

			else // change column for next element
				c++;
		} while (*str != 0x00);
		return 0;

	} else {
		printf("\n\nvt_print_string error: Invalid position\n\n");
		return 1;
	}

}

int vt_print_int(int num, char attr, int r, int c) {

	unsigned int numberOfCharsNeeded;
	bool negativeNumber =  false;
	unsigned int numberOfCharsLeftInScreen = (scr_width - c) + (scr_lines - r - 1) * scr_width;
	int new_num, digit_int;
	char digit_char;
	char* char_address = video_mem;

	if (r >= 0 && r < scr_lines && c >= 0 && c < scr_width)
	{
		numberOfCharsNeeded = floor (log10 (abs (num))) + 1;

		if (num < 0)
		{
			numberOfCharsNeeded++; // because "-" occupies a space
		}

		if (numberOfCharsNeeded > numberOfCharsLeftInScreen)
		{
			printf("\n\nvt_print_int error: There is not enough space for that int\n\n");
			return 1;
		}

		//checks if the number is negative turns the flag into true and changes the sign in new_num
		if (num < 0)
		{
			negativeNumber = true;
			new_num = -num;
		}
		else new_num = num;


		//puts r and c ready to put the last element
		for (--numberOfCharsNeeded; numberOfCharsNeeded > 0; numberOfCharsNeeded--)
		{
			if (c == scr_width - 1) // if its in the last element, go to the next line
			{
				c = 0;
				r++;
			}
			else // otherwise, inc the column
				c++;
		}

		while (new_num != 0)
		{
			digit_int = new_num % 10; // extract last digit
			new_num = new_num / 10; // saves the other ones

			digit_char = digit_int + '0';
			vt_print_char(digit_char, attr, r, c);

			if (c == 0) // changes line
			{
				c = scr_width - 1;
				r--;
			}
			else c--; // only changes column
		}

		if (negativeNumber) // if it's a negative number, print "-"
			vt_print_char(0x2D, attr, r, c);

		return 0; // successful
	}
	else
	{
		printf("\n\nvt_print_int error: Invalid position\n\n");
		return 1;
	}
}

int vt_draw_frame(int width, int height, char attr, int r, int c) {

	char LEFT_UP_CORNER = 0xC9;
	char RIGHT_UP_CORNER = 0xBB;
	char LEFT_DOWN_CORNER = 0xC8;
	char RIGHT_DOWN_CORNER = 0xBC;

	char LINE_HORIZONTAL = 0xCD;
	char LINE_VERTICAL = 0xBA;

	if ((width + c) >= scr_width - 1 || (height + r) >= scr_lines - 1){
		printf("\nFrame does not fit screen\n");
		return 1;
	}

	// put the r/c in position for left-upper corner
	char* char_adress = video_mem + (scr_width * r * 2) + (c*2);
	unsigned int i, r_actual = r, c_actual = c;

	// print left-upper corner
	vt_print_char(LEFT_UP_CORNER, attr, r_actual, c_actual);
	c_actual++;

	// print upper line
	for (i = 0; i < width; i++){
		vt_print_char(LINE_HORIZONTAL, attr, r_actual, c_actual++);
	}

	// print right-upper corner
	vt_print_char(RIGHT_UP_CORNER, attr, r_actual, c_actual);

	// prepare to print left line
	r_actual = ++r;
	c_actual = c;

	// print left line
	for (i = 0; i < height; i++){
		vt_print_char(LINE_VERTICAL, attr, r_actual++, c_actual);
	}

	// print left-bottom corner
	vt_print_char(LEFT_DOWN_CORNER, attr, r_actual, c_actual++);

	// print bottom line
	for (i = 0; i < width; i++){
		vt_print_char(LINE_HORIZONTAL, attr, r_actual, c_actual++);
	}

	// print right-bottom corner
	vt_print_char(RIGHT_DOWN_CORNER, attr, r_actual--, c_actual);

	// print right line
	for (i = 0; i < height; i++){
		vt_print_char(LINE_VERTICAL, attr, r_actual--, c_actual);
	}
}

/*
 * THIS FUNCTION IS FINALIZED, do NOT touch it
 */

char *vt_init(vt_info_t *vi_p) {

	int r;
	struct mem_range mr;

	/* Allow memory mapping */

	mr.mr_base = (phys_bytes)(vi_p->vram_base);
	mr.mr_limit = mr.mr_base + vi_p->vram_size;

	if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("video_txt: sys_privctl (ADD_MEM) failed: %d\n", r);

	/* Map memory */

	video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vi_p->vram_size);

	if(video_mem == MAP_FAILED)
		panic("video_txt couldn't map video memory");

	/* Save text mode resolution */

	scr_lines = vi_p->scr_lines;
	scr_width = vi_p->scr_width;

	return video_mem;
}
