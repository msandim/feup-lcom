#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>

#include <string.h>

#include "graphic_module.h"

static int file_number, total;
static SPRITE file_draw;

SPRITE getFileDraw()
{
  return file_draw;
}

int getTotal()
{
  return total;
}

int getFileNumber()
{
  return file_number;
}

void setNumber (int number) {
  file_number = number;
}

void incTotal () {
  total++;
}

int galleryModeLoad()
{
  // allocate memory for the file in memory (visualization at the gallery)
  file_draw.width = DRAW_SCREEN_H;
  file_draw.height = DRAW_SCREEN_V;
  file_draw.pixels = (unsigned short*) malloc(DRAW_SCREEN_H * DRAW_SCREEN_V * sizeof(unsigned short));

  if (file_draw.pixels == NULL)
  {
    printf("Could not allocate file_draw SPRITE\n");
    return 1;
  }

  // Open file with the number of drawings info
  FILE *file;
  char *mode = "r";
  file = fopen("/home/lcom/proj/drawings/drawings",mode);

  if (file != NULL)
  {
    fscanf(file,"%d", &total);
    printf("Total: %u\n",total);
    fclose(file);
  }
  else
  {
    total = 0;
    printf("Total: %u\n",total);
  }

  file_number = 0;

  return 0;
}

void galleryModeFree()
{
  FILE *file;
  char *mode = "w";
  file = fopen("/home/lcom/proj/drawings/drawings",mode);

  if (file != NULL)
  {
    fprintf(file, "%d", total);

    printf("Total: %u\n",total);

    fclose(file);
  }

  // free the memory to store current file in gallery
  free(file_draw.pixels);
}

void galleryModeInit()
{

  file_number = 0;

  // check if the file exists, if exists, file number = 1 and copy the contents to the sprite
  if (total == 0)
  {
    vg_fill_buffer(0xFFFF,file_draw.pixels, file_draw.width, file_draw.height);

    drawText(100, 100, "- no file was found -", 0, file_draw.pixels, file_draw.width, file_draw.height);
  } else
    loadDrawing(file_number, file_draw.pixels);

}

void keyboardGalleryEvent()
{
  // se clicarmos na direita
  if (getKeyboardPressState() && getKeyboard2ByteCode() && getKeyboardLastKey() == 0x4D
      && file_number < (total-1))
  {
    printf("NEXT\nNumber %u\nTotal %u\n",file_number + 1,total);
    loadDrawing(++file_number,file_draw.pixels);
  }

  // click on the left
  else if (getKeyboardPressState() && getKeyboard2ByteCode() && getKeyboardLastKey() == 0x4B
      && file_number != 0)
  {
    printf("PREVIOUS\nNumber %u\nTotal %u\n",file_number - 1,total);
    loadDrawing(--file_number,file_draw.pixels);
  }
}

int mouseGalleryEvent()
{
  static int previous_LB_state = 0;

  // se clicarmos na direita
  if (getxMousePosition() >= 952 &&
      getxMousePosition() <= 1023 &&
      getyMousePosition() >= 0 &&
      getyMousePosition() <= 767 &&
      getMouseLBstate()  && !previous_LB_state
      && file_number < (total-1)
  )
  {
    printf("NEXT\nNumber %u\nTotal %u\n",file_number + 1,total);
    loadDrawing(++file_number,file_draw.pixels);
  }

  // click on the left
  else if (getxMousePosition() >= 0 &&
      getxMousePosition() <= 72 &&
      getyMousePosition() >= 0 &&
      getyMousePosition() <= 767 &&
      getMouseLBstate() && file_number != 0 && !previous_LB_state)
  {
    printf("PREVIOUS\nNumber %u\nTotal %u\n",file_number - 1,total);
    loadDrawing(--file_number,file_draw.pixels);
  }

  else if (getxMousePosition() >= 170 &&
      getxMousePosition() <= 420 &&
      getyMousePosition() >= 687 &&
      getyMousePosition() <= 756 &&
      getMouseLBstate() && !previous_LB_state && total != 0)
    return 1; // editar este ficheiro

  else if (getxMousePosition() >= 600 &&
      getxMousePosition() <= 850 &&
      getyMousePosition() >= 687 &&
      getyMousePosition() <= 756 &&
      getMouseLBstate() && !previous_LB_state) {
    return -1; // sair
  }

  previous_LB_state = getMouseLBstate();

  return 0;
}
