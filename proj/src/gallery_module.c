#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>

#include <string.h>

#include "graphic_module.h"

static const char* drawings_path = "/home/lcom/proj/drawings/";

static int file_number;
static SPRITE file_draw;

SPRITE getFileDraw()
{
  return file_draw;
}

int galleryModeLoad()
{
  file_draw.width = DRAW_SCREEN_H;
  file_draw.height = DRAW_SCREEN_V;

  file_draw.pixels = (unsigned short*) malloc(DRAW_SCREEN_H * DRAW_SCREEN_V * sizeof(unsigned short));

  if (file_draw.pixels == NULL)
  {
    printf("Could not allocate file_draw SPRITE\n");
    return 1;
  }

  return 0;
}

void galleryModeFree()
{
  free(file_draw.pixels);
}

void galleryModeInit()
{
  // Open file
  FILE *file;
  char *mode = "rb";

  char* filename = "drawing001";

  //char* path = "/home/lcom/proj/drawings/drawing001";

  char path[100];

  strncpy(path, drawings_path, sizeof(path));
  strcat(path,filename);
  printf("path apos filename: %s\n",path);

  file = fopen(path,mode);

  // check if the file exists, if exists, file number = 1 and copy the contents to the sprite
  if (file == NULL)
  {
    file_number = 0;

    vg_fill_buffer(0xFFFF,file_draw.pixels, file_draw.width, file_draw.height);

    drawText(100, 100, "- no file was found -", 0, file_draw.pixels, file_draw.width, file_draw.height);
  }
  else
  {
    file_number = 1;

    // copiar conteudo para a sprite - NETO


    printf("abriu o ficheiro inicial!\n");

    // copia fingida para teste (meter tudo branco)
    vg_fill_buffer(0xFFFF,file_draw.pixels, file_draw.width, file_draw.height);

    fclose(file);
  }
}

int mouseGalleryEvent()
{
  static int previous_LB_state = 0;

  // se clickarmos na direita
  if (getxMousePosition() >= 100 &&
      getxMousePosition() <= 100 + 50 - 1 &&
      getyMousePosition() >= 670 &&
      getyMousePosition() <= 670 + 100 - 1 &&
      getMouseLBstate() && file_number != 0 && !previous_LB_state)
  {
    // Open file
    FILE *file;
    char *mode = "rb";

    char filename[50];
    snprintf(filename, 50, "drawing%03d", file_number+1);

    char path[100];

    strncpy(path, drawings_path, sizeof(path));
    strcat(path,filename);

    file = fopen(path,mode);

    // check if the file exists, if exists, advance the file_number and copy the contents
    if (file != NULL)
    {
      file_number++;

      // PRINT DE TESTE
      printf("avancou para file numero: %d\n",file_number);

      // copiar conteudo do file para a sprite - NETO

      fclose(file);
    }
  }

  else if (getxMousePosition() >= 200 &&
      getxMousePosition() <= 200 + 50 - 1 &&
      getyMousePosition() >= 670 &&
      getyMousePosition() <= 670 + 50 - 1 &&
      getMouseLBstate() && file_number != 0 && !previous_LB_state)
  {
    if (file_number > 1)
    {
      file_number--;

      // PRINT DE TESTE
      printf("diminuiu para file numero: %d\n",file_number);

      // Open file
      FILE *file;
      char *mode = "rb";
      char path[100];

      snprintf (path, 100, "/home/lcom/proj/drawings/drawing%03d",file_number);

      file = fopen(path,mode);

      // copiar conteudo do file para a sprite - NETO

      fclose(file);
    }
  }

  else if (getxMousePosition() >= 300 &&
      getxMousePosition() <= 300 + 50 - 1 &&
      getyMousePosition() >= 670 &&
      getyMousePosition() <= 670 + 100 - 1 &&
      getMouseLBstate() && file_number != 0 && !previous_LB_state)
    return 1; // editar este ficheiro

  else if (getxMousePosition() >= 400 &&
      getxMousePosition() <= 400 + 50 - 1 &&
      getyMousePosition() >= 670 &&
      getyMousePosition() <= 670 + 100 - 1 &&
      getMouseLBstate() && !previous_LB_state)
    return -1; // sair

  previous_LB_state = getMouseLBstate();

  return 0;
}
