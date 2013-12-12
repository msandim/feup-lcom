#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <minix/sysutil.h>
#include <string.h>

#include "vbe.h"
#include "video_gr.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab I304 is 0xF0000000
 * Better run my version of lab2 as follows:
 *     service run `pwd`/lab2 -args "mode 0x105"
 */
/*#define VRAM_PHYS_ADDR	0xF0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8*/

/* Stuff to make the VBE call */
#define LINEAR_MODEL_BIT 14
#define BIT(n) (0x1 << (n))

/* Private global variables */

static short *video_mem;		/* Process address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

unsigned int vg_get_h_res()
{
  return (unsigned int) h_res;
}

unsigned int vg_get_v_res()
{
  return (unsigned int) v_res;
}

unsigned int vg_get_bits_per_pixel()
{
  return (unsigned int) bits_per_pixel;
}

int vg_copy_buffer(short* buffer)
{
  unsigned int i;

  short* ptrVRAM = video_mem;

  /* copy all the pixels
  for (i=0; i < h_res * v_res; i++)
  {
   *ptrVRAM = *buffer;

    ptrVRAM++;
    buffer++;
  }*/

  memcpy(ptrVRAM,buffer,h_res*v_res*2); // *2 because we are copying bytes!
  return 0;
}

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

  short* ptrVRAM = video_mem;

  //short j;

  //for (j=0; j < 1000; j++)
  //{
  // fill all the pixels
  for (i=0; i < h_res * v_res; i++)
  {
    *ptrVRAM = color;
    //*ptrVRAM = j;
    ptrVRAM++;
  }
  //ptrVRAM = video_mem;
  //tickdelay(micros_to_ticks(20000));
  //}


  return 0;
}

int vg_fill_buffer(unsigned long color, short* buffer, unsigned long dim_h, unsigned long dim_v) {

  unsigned int i;

  //short j;

  //for (j=0; j < 1000; j++)
  //{
  // fill all the pixels
  for (i=0; i < dim_h * dim_v; i++)
  {
    *buffer = color;
    //*ptrVRAM = j;
    buffer++;
  }
  //ptrVRAM = video_mem;
  //tickdelay(micros_to_ticks(20000));
  //}


  return 0;
}

int vg_set_pixel_buffer(unsigned long x, unsigned long y, unsigned long color, short* buffer, unsigned long dim_h, unsigned long dim_v) {

  if (x >= dim_h || y >= dim_v)
  {
    printf("vg_set_pixel::Invalid X/Y\n");
    printf("X: %u, Y: %u V_RES: %u H_RES: %u",x,y,dim_h,dim_v);
    return 1;
  }

  //if (y == 50 && color == 0)
    //printf("Endereco inicial do buffer: %p\n",buffer);
  buffer += ((y*dim_h) + x);
  //if (y == 50 && color == 0)
    //printf("Endereco final do buffer: %p\n",buffer);

  //if (y == 50 && color == 0)
    //printf("buffer1: %x\n",*buffer);

  *buffer = color;

  //if (y == 50 && color == 0)
    //printf("buffer2: %x\n",*buffer);

  return 0;
}

long vg_get_pixel(unsigned long x, unsigned long y) {

  short* ptrVRAM = video_mem;

  if (y >= v_res || x >= h_res)
  {
    printf("vg_get_pixel::Invalid X/Y\n");
    printf("X: %u, Y: %u V_RES: %u H_RES: %u",x,y,v_res,h_res);
    return 1;
  }

  ptrVRAM += ((y*h_res) + x);

  return *ptrVRAM;
}

int vg_draw_line_buffer(unsigned long xi, unsigned long yi,
    unsigned long xf, unsigned long yf, unsigned long color, short* buffer,
    unsigned long dim_h, unsigned long dim_v) {

  /*
	int a,b,p,x,y,dx,dy;

	if (xf<xi){
		vg_draw_line(xf,yf,xi,yi,color);
	} else {

		dx = xf-xi;
		dy = yf-yi;


		if (abs(dy)>abs(dx)){
			//Calculate the A,B and initial P value
			a=2*(xf-xi);
			b=a-2*(yf-yi);
			p=a-(yf-yi);

			//Paint first pixel
			vg_set_pixel(xi,yi,color);

			//Set x as xi
			x=xi;


			if (dx <= 0){
				for (y=yi+1; y<yf; y++){
					if (p<0){
						vg_set_pixel(x,y,color);
						p=a+p;
					} else {
						x--;
						vg_set_pixel(x,y,color);
						p=b+p;
					}
				}
			} else {
				for (y=yi+1; y<yf; y++){
					if (p<0){
						vg_set_pixel(x,y,color);
						p=a+p;
					} else {
						x++;
						vg_set_pixel(x,y,color);
						p=b+p;
					}
				}
			}

		}
		else{
			//Calculate the A,B and initial P value
			a=2*(yf-yi);
			b=a-2*(xf-xi);
			p=a-(xf-xi);

			//Paint first pixel
			vg_set_pixel(xi,yi,color);

			//Set y as yi
			y=yi;

			if (dy <= 0){
				for (x=xi+1; x<xf; x++){
					if (p<0){
						y--;
						vg_set_pixel(x,y,color);
						p=a+p;
					} else {
						vg_set_pixel(x,y,color);
						p=b+p;
					}
				}
			} else {
				for (x=xi+1; x<xf; x++){
					if (p<0){
						y++;
						vg_set_pixel(x,y,color);
						p=a+p;
					} else {

						vg_set_pixel(x,y,color);
						p=b+p;
					}
				}
			}

		}
	}

   */

  if (yi >= dim_v || yf >= dim_v || xi >= dim_h || xf >= dim_h)
  {
    printf("vg_set_pixel::Invalid X/Y\n");
    return 1;
  }

  int slope;
  int dx, dy, incE, incNE, d, x, y;
  // Onde inverte a linha x1 > x2
  if (xi > xf){
    vg_draw_line_buffer(xf, yf, xi, yi, color, buffer,dim_h,dim_v);
    return;
  }
  dx = xf - xi;
  dy = yf - yi;

  if (dy < 0){
    slope = -1;
    dy = -dy;
  }
  else{
    slope = 1;
  }

  // Constante de Bresenham
  incE = 2 * dy;
  incNE = 2 * dy - 2 * dx;
  d = 2 * dy - dx;
  y = yi;
  for (x = xi; x <= xf; x++){
    vg_set_pixel_buffer(x,y,color,buffer,dim_h,dim_v);
    if (d <= 0){
      d += incE;
    }
    else{
      d += incNE;
      y += slope;
    }
  }

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
