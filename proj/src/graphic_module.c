#include "graphic_module.h"

#include "video_gr.h"

void screenInit()
{
  vg_init(0x117); // init in mode 0x117
}

void screenExit()
{
  vg_exit();
}

int set_drawMode(unsigned int mouse_x, unsigned int mouse_y)
{
  // desenhar fundo
  // draw toolboxes
  // draw o desenho

  // draw o rato
  draw_mouse(mouse_x, mouse_y);

  return 0;

}

// ... draw toolbox, draw desenho

int draw_mouse(unsigned int mouse_x, unsigned int mouse_y)
{
  // desenhar quadrado de lado 3, centrado em X,Y
  // estes parametros podem ser acedidos pelo rato AQUI!

  unsigned x_draw = mouse_x, y_draw = mouse_y;

  vg_set_pixel(x_draw,y_draw,1000);

  int i = 0;

  while (i != 8)
  {
    if (x_draw < 1024 && mouse_y < 768) // se tas nas medidas
      vg_set_pixel(x_draw,y_draw,1000);

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
