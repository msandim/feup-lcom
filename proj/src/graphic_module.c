#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>

#include "graphic_module.h"
#include "video_gr.h"
#include "mouse.h"
#include <stdlib.h>

static short* double_buf; // temp buffer for graphic use

int screenInit()
{
  vg_init(0x117); // init in mode 0x117

  double_buf = (short*) malloc(vg_get_h_res()*vg_get_v_res()*sizeof(short));

  if (double_buf == NULL)
  {
    printf("Error in alloc of the auxiliar buffer\n");
    return 1;
  }
  else
  {
    printf("OK\n");
    return 0;
  }
}

void screenExit()
{
  free(double_buf);
  vg_exit();
}

int set_graphicsDrawMode()
{
  // desenhar fundo
  vg_fill_buffer(65535,double_buf);
  // draw toolboxes
  // draw o desenho

  // draw o rato
  draw_mouse(mouse_getxposition(),mouse_getyposition());

  vg_copy_buffer(double_buf);

  return 0;
}

// ... draw toolbox, draw desenho

int draw_mouse(unsigned int mouse_x, unsigned int mouse_y)
{
  // desenhar quadrado de lado 3, centrado em X,Y
  // estes parametros podem ser acedidos pelo rato AQUI!

  unsigned x_draw = mouse_x, y_draw = mouse_y;

  vg_set_pixel_buffer(x_draw,y_draw,0,double_buf);

  int i = 0;

  while (i != 8)
  {
    if (x_draw < vg_get_h_res() && y_draw < vg_get_v_res()) // se tas nas medidas
      vg_set_pixel_buffer(x_draw,y_draw,0,double_buf);

    switch(i) {
    case 0: y_draw = --mouse_y; break; // same column
    case 1: y_draw = ++mouse_y; break;
    case 2: x_draw = ++mouse_x; y_draw = --mouse_y; break; // -> column
    case 3: y_draw = mouse_y; break;
    case 4: y_draw = ++mouse_y; break;
    case 5: x_draw = --mouse_x; y_draw = --mouse_y; break; // <- column
    case 6: y_draw = mouse_y; break;
    case 7: y_draw = ++mouse_y; break;
    }

    i++;
  }

  return 0;
}
