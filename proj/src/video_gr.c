#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <minix/sysutil.h>
#include <string.h>

#include "vbe.h"
#include "video_gr.h"
#include "stack.h"

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

int vg_copy_buffer(unsigned short* buffer)
{
  unsigned int i;

  unsigned short* ptrVRAM = video_mem;

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

  return vg_fill_buffer(color,video_mem,h_res,v_res);
}

int vg_fill_buffer(unsigned long color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v) {

  unsigned int i;

  for (i=0; i < dim_h * dim_v; i++)
  {
    *buffer = color;
    buffer++;
  }


  return 0;
}

int vg_set_pixel_buffer(unsigned long x, unsigned long y, unsigned long color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v) {

  if (x >= dim_h || y >= dim_v)
  {
    //printf("vg_set_pixel::Invalid X/Y\n");
    //printf("X: %u, Y: %u V_RES: %u H_RES: %u",x,y,dim_h,dim_v);
    return 1;
  }

  buffer += ((y*dim_h) + x);

  *buffer = color;

  return 0;
}

long vg_get_pixel_buffer(unsigned long x, unsigned long y, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v) {


  if (y >= dim_v || x >= dim_h)
  {
    //printf("vg_get_pixel::Invalid X/Y\n");
    //printf("X: %u, Y: %u V_RES: %u H_RES: %u",x,y,dim_v,dim_h);
    return 1;
  }

  return *(buffer + ((y*dim_h) + x));
}

long vg_get_pixel(unsigned long x, unsigned long y)
{
  return vg_get_pixel_buffer(x,y,video_mem,h_res,v_res);
}

int vg_draw_line_buffer(unsigned long xi, unsigned long yi,
    unsigned long xf, unsigned long yf, unsigned long color, unsigned short* buffer,
    unsigned long dim_h, unsigned long dim_v) {


  if (yi >= dim_v || yf >= dim_v || xi >= dim_h || xf >= dim_h)
  {
    //printf("vg_set_pixel::Invalid X/Y\n");
    return 1;
  }


  int cx, cy,
  ix, iy,
  dx, dy,
  ddx= xf-xi, ddy= yf-yi;

  if (!ddx) { //vertical line special case
    if (ddy > 0) {
      cy= yi;

      do vg_set_pixel_buffer(xi,cy++,color,buffer,dim_h,dim_v);
      while (cy <= yf);

      return 0;

    } else {
      cy= yf;

      do vg_set_pixel_buffer(xi,cy++,color,buffer,dim_h,dim_v);
      while (cy <= yi);

      return 0;

    }
  }
  if (!ddy) { //horizontal line special case
    if (ddx > 0)
    {
      cx= xi;

      do vg_set_pixel_buffer(cx,yi,color,buffer,dim_h,dim_v);
      while (++cx <= xf);

      return 0;
    } else
    {
      cx= xf;

      do vg_set_pixel_buffer(cx,yi,color,buffer,dim_h,dim_v);
      while (++cx <= xi);

      return 0;
    }
  }
  if (ddy < 0) { iy= -1; ddy= -ddy; }//pointing up
  else iy= 1;
  if (ddx < 0) { ix= -1; ddx= -ddx; }//pointing left
  else ix= 1;
  dx= dy= ddx*ddy;
  cy= yi, cx= xi;
  if (ddx < ddy) { // < 45 degrees, a tall line
    do
    {
      dx-=ddy;
      do
      {
        vg_set_pixel_buffer(cx,cy,color,buffer,dim_h,dim_v);
        cy+=iy, dy-=ddx;
      } while (dy >=dx);
      cx+=ix;
    } while (dx > 0);
  } else { // >= 45 degrees, a wide line
    do
    {
      dy-=ddx;
      do
      {
        vg_set_pixel_buffer(cx,cy,color,buffer,dim_h,dim_v);
        cx+=ix, dx-=ddy;
      } while (dx >=dy);
      cy+=iy;
    } while (dy > 0);
  }

  return 0;
}

void vg_draw_object_buffer(unsigned short* object, int w, int h, int x, int y, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v) {

  int i, j, k;
  /*
	printf ("H: %u\n", h);
	printf ("W: %u\n", w);
	printf ("X: %u\n", x);
	printf ("Y: %u\n", y);
   */
  for (i = 0; i < h; i++) {
    k = 0;
    for (j = w; j > 0; j--) {
      vg_set_pixel_buffer(x+j, y+i, object[(w*h) - (i*w+k)], buffer, dim_h, dim_v);
      k++;
    }
  }
}

void vg_draw_rectangle_buffer(unsigned int x, unsigned int y, unsigned int x_dim, unsigned int y_dim, unsigned long color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v) {

  unsigned int i;

  //printf("rect (%u,%u) w=%u, h=%u, color=%u, dim_h=%u, dim_v=%u",x,y,w,h,color,dim_h,dim_v);

  for (i = 0; i < y_dim; i++)
    vg_draw_line_buffer(x,y+i,x+x_dim,y+i,color,buffer,dim_h,dim_v);
}

void vg_draw_circle_buffer(unsigned int x, unsigned int y, unsigned int radius, unsigned short color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v) {
  unsigned int v, h;

  for (v = 0; v <= radius; v++) {
    for (h = 0; h <= radius; h++) {
      if ((v*v + h*h) <= radius*radius ) {
        vg_set_pixel_buffer (x+h, y+v, color,buffer,dim_h,dim_v);
        vg_set_pixel_buffer (x+h, y-v, color,buffer,dim_h,dim_v);
        vg_set_pixel_buffer (x-h, y+v, color,buffer,dim_h,dim_v);
        vg_set_pixel_buffer (x-h, y-v, color,buffer,dim_h,dim_v);
        vg_set_pixel_buffer (x+v, y+h, color,buffer,dim_h,dim_v);
        vg_set_pixel_buffer (x+v, y-h, color,buffer,dim_h,dim_v);
        vg_set_pixel_buffer (x-v, y+h, color,buffer,dim_h,dim_v);
        vg_set_pixel_buffer (x-v, y-h, color,buffer,dim_h,dim_v);
      }
    }
  }
}

void vg_flood_fill_buffer(int x, int y, unsigned long replacement_color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v)
{

  unsigned long target_color = vg_get_pixel_buffer(x, y, buffer, dim_h, dim_v);

  if(replacement_color == target_color) return; //avoid infinite loop

  int cur_x = x, cur_y = y;

  stack* s = new_stack(16777216);

  if (s == NULL) {
    printf("Stack not created!\n");
    return;
  }

  push_stack(s, cur_x);
  push_stack(s, cur_y);

  while(!pop_stack(s,&cur_y) && !pop_stack(s,&cur_x))
  {
    vg_set_pixel_buffer(cur_x, cur_y, replacement_color, buffer, dim_h, dim_v);

    if(cur_x + 1 < dim_h && vg_get_pixel_buffer(cur_x + 1, cur_y, buffer, dim_h, dim_v) == target_color)
    {
      if(push_stack(s,cur_x + 1) || push_stack(s,cur_y)) return;
    }

    if(cur_x - 1 >= 0 && vg_get_pixel_buffer(cur_x - 1, cur_y, buffer, dim_h, dim_v) == target_color)
    {
      if(push_stack(s,cur_x - 1) || push_stack(s,cur_y)) return;
    }

    if(cur_y + 1 < dim_v && vg_get_pixel_buffer(cur_x, cur_y + 1, buffer, dim_h, dim_v) == target_color)
    {
      if(push_stack(s,cur_x) || push_stack(s,cur_y + 1)) return;
    }

    if(cur_y - 1 >= 0 && vg_get_pixel_buffer(cur_x, cur_y - 1, buffer, dim_h, dim_v) == target_color)
    {
      if(push_stack(s,cur_x) || push_stack(s,cur_y - 1)) return;
    }
  }

  delete_stack(s);

}

void vg_draw_brush_buffer(int xi, int yi, int xf, int yf, unsigned short color, unsigned int thickness, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v) {

  int cx, cy,
  ix, iy,
  dx, dy,
  ddx= xf-xi, ddy= yf-yi;

  if (!ddx) { //vertical line special case
    if (ddy > 0) {
      cy= yi;

      do vg_draw_circle_buffer(xi,cy++,thickness,color,buffer,dim_h,dim_v);
      while (cy <= yf);

      return;

    } else {
      cy= yf;

      do vg_draw_circle_buffer(xi,cy++,thickness,color,buffer,dim_h,dim_v);
      while (cy <= yi);

      return;
    }
  }
  if (!ddy) { //horizontal line special case
    if (ddx > 0) {
      cx= xi;

      do vg_draw_circle_buffer(cx,yi,thickness,color,buffer,dim_h,dim_v);
      while (++cx <= xf);

      return;
    } else {
      cx= xf;

      do vg_draw_circle_buffer(cx,yi,thickness,color,buffer,dim_h,dim_v);
      while (++cx <= xi);

      return;
    }
  }

  if (ddy < 0) { iy= -1; ddy= -ddy; }//pointing up
  else iy= 1;

  if (ddx < 0) { ix= -1; ddx= -ddx; }//pointing left
  else ix= 1;

  dx= dy= ddx*ddy;
  cy= yi, cx= xi;

  if (ddx < ddy) { // < 45 degrees, a tall line
    do
    {
      dx-=ddy;
      do
      {
        vg_draw_circle_buffer(cx,cy,thickness,color,buffer,dim_h,dim_v);
        cy+=iy, dy-=ddx;
      } while (dy >=dx);
      cx+=ix;
    } while (dx > 0);
  } else { // >= 45 degrees, a wide line
    do
    {
      dy-=ddx;
      do
      {
        vg_draw_circle_buffer(cx,cy,thickness,color,buffer,dim_h,dim_v);
        cx+=ix, dx-=ddy;
      } while (dx >=dy);
      cy+=iy;
    } while (dy > 0);
  }
}

void vg_draw_char_buffer(unsigned short* object, int w, int h, int x, int y, unsigned short color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v) {

  int i, j, k;

  for (i = 0; i < h; i++) {
    k = 1;
    for (j = w ; j > 0; j--) {
      if (object[(w*h) - (i*w+k)] != 0xFFFF)
        vg_set_pixel_buffer(x+j, y+i, color, buffer, dim_h, dim_v);
      k++;
    }
  }
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

unsigned short* _vg_fill_buf;
unsigned int _vg_fill_w, _vg_fill_h;
unsigned short _vg_fill_c, _vg_fill_o;

void vg_fill_ra(unsigned int x, unsigned int y,
    unsigned short color,
    unsigned short* buffer, unsigned int w, unsigned int h){
  if( buffer[x+y*w] == color ) return;
  _vg_fill_buf = buffer;
  _vg_fill_w = w;
  _vg_fill_h = h;
  _vg_fill_c = color;
  _vg_fill_o = buffer[x+y*w];
  buffer[x+y*w] = color;
  //vg_set_pixel_buffer(x,y,_vg_fill_c,_vg_fill_buf,_vg_fill_w,_vg_fill_h);
  if(
      x > 0 &&
      _vg_fill_o == buffer[x-1+y*w]
  ) _vg_fill( x-1, y, 0 );
  if(
      y > 0 &&
      _vg_fill_o == buffer[x+(y-1)*w]
  ) _vg_fill( x, y-1, 1 );
  if(
      x < w &&
      _vg_fill_o == buffer[x+1+y*w]
  ) _vg_fill( x+1, y, 2 );
  if(
      y < h &&
      _vg_fill_o == buffer[x+(y+1)*w]
  ) _vg_fill( x, y+1, 3 );
}

void _vg_fill(unsigned int x, unsigned int y, unsigned char dir){ //dir 0 = going left, 1 = up, 2 = right, 3 = down
  _vg_fill_buf[x+y*_vg_fill_w] = _vg_fill_c;
  //vg_set_pixel_buffer(x,y,_vg_fill_c,_vg_fill_buf,_vg_fill_w,_vg_fill_h);
  if(
      dir != 2 &&
      x > 0 &&
      _vg_fill_o == _vg_fill_buf[x-1+y*_vg_fill_w]
  ) _vg_fill( x-1, y, 0 );
  if(
      dir != 3 &&
      y > 0 &&
      _vg_fill_o == _vg_fill_buf[x+(y-1)*_vg_fill_w]
  ) _vg_fill( x, y-1, 1 );
  if(
      dir != 0 &&
      x < _vg_fill_w &&
      _vg_fill_o == _vg_fill_buf[x+1+y*_vg_fill_w]
  ) _vg_fill( x+1, y, 2 );
  if(
      dir != 1 &&
      y < _vg_fill_h &&
      _vg_fill_o == _vg_fill_buf[x+(y+1)*_vg_fill_w]
  ) _vg_fill( x, y+1, 3 );
}
