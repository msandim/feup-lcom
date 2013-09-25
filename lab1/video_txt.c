#include <minix/drivers.h>
#include <sys/video.h>
#include <sys/mman.h>

#include <assert.h>

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
		return 1;

}

int vt_print_string(char *str, char attr, int r, int c) {

/* Completing .... */

//Checks if the input is valid
        if ( r > scr_lines || c > scr_width || c < 0 || r < 0) {
                return 1;
        }
//Start by increasing the pointer to the starting point
        char* char_Adress = video_mem + (scr_width * r * 2) + (c*2);

//Loop the string printing each character
        bool flag == true;

        do{
                if (*str == 0x00){
                        break;
                }

                c ++;

                if (c == scr_width-1){
                        r++;
                        c=0;
                }

                if (r>scr_lines){
                        return 1;                }
                *char_Adress = *str;
                char_Adress ++;
                *char_Adress = attr;
                char_Adress ++;

        } while (flag);

        return 0;


}

int vt_print_int(int num, char attr, int r, int c) {

	/*char digit;
	char* char_address = video_mem;

	// sum the lines before the line we are putting the int on
	char_address += r * scr_width * 2;

	// sum the columns to the one where we put our int on
	char_address += c * 2;



	do
	{
		*char_address = string_with_int[i]; // gets the char to VRAM
		char_address++;
		*char_address = attr; // sets the attribute
		char_address++;

		i++; // incs counter
	} while (i < int_length);*/
}


int vt_draw_frame(int width, int height, char attr, int r, int c) {

  /* To complete ... */

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
