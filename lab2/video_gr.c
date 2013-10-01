#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab I304 is 0xF0000000
 * Better run my version of lab2 as follows:
 *     service run `pwd`/lab2 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xF0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

/* Stuff to make the VBE call */
#define BIT(n) (0x1 << (n))
#define LINEAR_MODEL_BIT 14

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

void * vg_init(unsigned short mode) {

	// version with hardcoded values
	h_res = H_RES;
	v_res = V_RES;
	bits_per_pixel = BITS_PER_PIXEL;

	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */
	reg86.u.b.ah = 0x4F;    /* Prepare to make a VBE Call */
	reg86.u.b.al = 0x02;    /* Using function 2 */


	/*  Bit 14 of the BX register should be set to use
	a linear frame buffer model */
	reg86.u.w.bx = BIT(LINEAR_MODEL_BIT) | mode; /* try to use 015 */
	if( sys_int86(&reg86) != OK ) {
		printf("vg_init: sys_int86() failed \n");
		return NULL;
	}

	/* Map memory */

	struct mem_range mr;
	//video_mem = vm_map_phys(SELF, (void *)mr.mr_base, 1024*768);
	video_mem = (char*) VRAM_PHYS_ADDR;

	return video_mem;
}

int vg_fill(unsigned long color) {
	return 0;
}

int vg_set_pixel(unsigned long x, unsigned long y, unsigned long color) {
	return 0;
}

long vg_get_pixel(unsigned long x, unsigned long y) {
	return 0;
}

int vg_draw_line(unsigned long xi, unsigned long yi, 
		unsigned long xf, unsigned long yf, unsigned long color) {
	return 0;
}

int vg_exit() {
	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */

	reg86.u.b.ah = 0x00;    /* Set Video Mode function */
	reg86.u.b.al = 0x03;    /* 80x25 text mode*/

	if( sys_int86(&reg86) != OK ) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}