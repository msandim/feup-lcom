#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>

#include "graphic_module.h"
#include "video_gr.h"
#include "mouse.h"
#include <stdlib.h>

//short* vg_temp_buf;

void screenInit()
{
  vg_init(0x117); // init in mode 0x117
  //short * vg_temp_buf = malloc(1024*768 * sizeof(short));

  /*if (vg_temp_buf == NULL)
    printf("UPSS");
  else
    printf("OK");*/
}

void screenExit()
{
  vg_exit();
  //free(vg_temp_buf);
}

int set_drawMode()
{
  // desenhar fundo
  vg_fill(65535);
  // draw toolboxes
  // draw o desenho

  // draw o rato
  draw_mouse(mouse_getxposition(),mouse_getyposition());

  //vg_set_buffer(vg_temp_buf);

  return 0;

}

// ... draw toolbox, draw desenho

int draw_mouse(unsigned int mouse_x, unsigned int mouse_y)
{
  // desenhar quadrado de lado 3, centrado em X,Y
  // estes parametros podem ser acedidos pelo rato AQUI!

  unsigned x_draw = mouse_x, y_draw = mouse_y;

  vg_set_pixel(x_draw,y_draw,0);

  int i = 0;

  while (i != 8)
  {
    if (x_draw < vg_get_h_res() && y_draw < vg_get_v_res()) // se tas nas medidas
      vg_set_pixel(x_draw,y_draw,0);

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
