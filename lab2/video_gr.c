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
#define LINEAR_MODEL_BIT 14
#define BIT(n) (0x1 << (n))

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

void * vg_init(unsigned short mode) {

	// VERSION WITH GET MODE INFO

	vbe_mode_info_t info_vbe;

	vbe_get_mode_info(mode, &info_vbe);

	// version with hardcoded values
	//h_res = H_RES;
	//v_res = V_RES;
	//bits_per_pixel = BITS_PER_PIXEL;
	//phys_bytes flat_address = (phys_bytes) VRAM_PHYS_ADDR;

	// VERSION WITH VBE
	h_res = info_vbe.XResolution;
	v_res = info_vbe.YResolution;
	bits_per_pixel = info_vbe.BitsPerPixel;
	phys_bytes flat_address = (phys_bytes) info_vbe.PhysBasePtr;

	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */
	reg86.u.b.ah = 0x4F;    /* Prepare to make a VBE Call */
	reg86.u.b.al = 0x02;    /* Using function 2 */


	/*  Bit 14 of the BX register should be set to use
	a linear frame buffer model */
	reg86.u.w.bx = BIT(LINEAR_MODEL_BIT) | mode;

	if( sys_int86(&reg86) != OK ) {
		printf("vg_init: sys_int86() failed \n");
		return NULL;
	}

	/* Allow memory mapping */

	struct mem_range mem;
	int r;

	mem.mr_base = flat_address;
	mem.mr_limit = mem.mr_base + (h_res*v_res*bits_per_pixel);

	if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mem)))
		panic("video_gr: sys_privctl (ADD_MEM) failed: %d\n", r);

	/* Map memory */

	video_mem = vm_map_phys(SELF, (void *)mem.mr_base, h_res*v_res*bits_per_pixel);

	if(video_mem == MAP_FAILED)
		panic("video_gr couldn't map video memory");

	return video_mem;
}

int vg_fill(unsigned long color) {

	unsigned int i;

	char* ptrVRAM = video_mem;

	// fill all the pixels
	for (i=0; i < h_res * v_res; i++)
	{
		*ptrVRAM = color;
		ptrVRAM++;
	}

	return 0;
}

int vg_set_pixel(unsigned long x, unsigned long y, unsigned long color) {
	char* ptrVRAM = video_mem;

	ptrVRAM += ((y*H_RES) + x);
	*ptrVRAM = color;

	return 0;
}

long vg_get_pixel(unsigned long x, unsigned long y) {
	char* ptrVRAM = video_mem;

	ptrVRAM += ((y*H_RES) + x);

	return *ptrVRAM;
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
