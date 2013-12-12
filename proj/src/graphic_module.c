#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>

#include "graphic_module.h"
#include "video_gr.h"
#include "user_interaction.h"
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

void drawBufferInVRAM()
{
  vg_copy_buffer(double_buf);
}

int drawAreaInDoubleBuffer(short* buffer, unsigned int x_upperleft_corner, unsigned int y_upperleft_corner,unsigned int dim_h, unsigned int dim_v)
{
  // check if the area fits in the double buffer
  if (dim_h + x_upperleft_corner >= vg_get_h_res() || dim_v + y_upperleft_corner >= vg_get_v_res())
  {
    printf("Graphic Module: Area does not fit temp buffer\n");
    return 1;
  }

  unsigned int y_position;

  for (y_position = y_upperleft_corner; y_position <= dim_v + y_upperleft_corner; y_position++)
    memcpy(double_buf + (y_position * vg_get_h_res() + x_upperleft_corner),buffer,2*dim_h);

  return 0;
}

int set_graphicsDrawMode(short* tela)
{
  // desenhar fundo
  vg_fill_buffer(65535,double_buf,vg_get_h_res(),vg_get_v_res());
  vg_draw_line_buffer(500,200,500,399,0,double_buf,vg_get_h_res(),vg_get_v_res());
  vg_draw_line_buffer(500,399,699,399,0,double_buf,vg_get_h_res(),vg_get_v_res());
  vg_draw_line_buffer(699,399,699,200,0,double_buf,vg_get_h_res(),vg_get_v_res());
  vg_draw_line_buffer(699,200,500,200,0,double_buf,vg_get_h_res(),vg_get_v_res());


  vg_draw_line_buffer(0,0,500,200,30,double_buf,vg_get_h_res(),vg_get_v_res());
  // draw toolboxes

  // draw a tela
  drawAreaInDoubleBuffer(tela, 500, 200, 200, 200);

  // draw o rato
  draw_mouse();

  drawBufferInVRAM();

  return 0;
}

// ... draw toolbox, draw desenho

int draw_mouse()
{
  // desenhar quadrado de lado 3, centrado em X,Y
  // estes parametros podem ser acedidos pelo rato AQUI!

  unsigned int mouse_x = getxMousePosition(), mouse_y = getyMousePosition();
  unsigned x_draw = mouse_x, y_draw = mouse_y;

  vg_set_pixel_buffer(x_draw,y_draw,0,double_buf,vg_get_h_res(),vg_get_v_res());

  int i = 0;

  while (i != 8)
  {
    if (x_draw < vg_get_h_res() && y_draw < vg_get_v_res()) // se tas nas medidas
      vg_set_pixel_buffer(x_draw,y_draw,40,double_buf,vg_get_h_res(),vg_get_v_res());

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
