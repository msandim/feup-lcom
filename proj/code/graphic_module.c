#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>

#include "graphic_module.h"
#include "video_gr.h"
#include "user_interaction.h"

#include <stdlib.h>

static unsigned short* double_buf; // temp buffer for graphic use
static SPRITE* letters; // sprite buffer with the letters
static SPRITE cursor;

static SPRITE intro;
static SPRITE menu;
static SPRITE gallery;

int screenInit()
{
  vg_init(0x117); // init in mode 0x117

  double_buf = (unsigned short*) malloc(vg_get_h_res()*vg_get_v_res()*sizeof(unsigned short));

  if (double_buf == NULL)
  {
    printf("Error in alloc of the auxiliar buffer\n");
    return 1;
  }

  if (loadChars())
  {
    printf("Error loading chars for drawText\n");
    return 1;
  }

  if (loadCursor())
  {
    printf("Error loading cursor\n");
    return 1;
  }

  if (loadIntroGraphics())
  {
    printf("Error loading Intro Graphics\n");
    return 1;
  }

  if (loadMenuGraphics())
  {
    printf("Error loading Menu Graphics\n");
    return 1;
  }

  if (loadGalleryGraphics())
  {
    printf("Error loading Gallery Graphics\n");
    return 1;
  }

  return 0;
}

void screenExit()
{
  // free memory for double buffer
  free(double_buf);

  unsigned int i;

  // free memory for the array of sprites
  for (i=0; i < 37; i++)
    free(letters[i].pixels);

  free(letters);

  // free memory for the cursor sprite
  free(cursor.pixels);

  // free intro graphics
  freeIntroGraphics();

  // free menu graphics
  freeMenuGraphics();

  // free gallery graphics
  freeGalleryGraphics();

  vg_exit();
}

void drawBufferInVRAM()
{
  vg_copy_buffer(double_buf);
}


void set_graphicsDrawMode(unsigned short* draw_screen, Draw_screen_area draw_area, Draw_screen_area current_area, BTN* btn_array, SPRITE color_bar, unsigned short color_selected)
{
  // draw background
  vg_fill_buffer(color_selected,double_buf,vg_get_h_res(),vg_get_v_res());

  // draw toolboxes
  drawToolBar(btn_array,double_buf);

  // draw colorbox
  vg_draw_object_buffer(color_bar.pixels,color_bar.width,color_bar.height, 122, 700, double_buf,vg_get_h_res(),vg_get_v_res());

  // draw draw_screen
  drawAreaInDoubleBuffer(draw_screen, draw_area.x_ul_corner, draw_area.y_ul_corner, draw_area.h_dim, draw_area.v_dim);

  // draw frame of area selected (if different from the default one)
  if (!areasAreEqual(draw_area,current_area))
    vg_draw_frame_buffer(current_area.x_ul_corner, current_area.y_ul_corner, current_area.x_ul_corner + current_area.h_dim, current_area.y_ul_corner + current_area.v_dim, 45, double_buf, vg_get_h_res(), vg_get_v_res());

  // draw mouse
  drawMouse();

  // update buffer in VRAM
  drawBufferInVRAM();
}

void set_graphicsIntroMode()
{
  vg_draw_object_buffer(intro.pixels, intro.width, intro.height, 0, 0, double_buf, vg_get_h_res(), vg_get_v_res());

  // update buffer in VRAM
  drawBufferInVRAM();
}

void set_graphicsMenuMode()
{
  // draw menu
  //drawAreaInDoubleBuffer(menu.pixels, 0, 0, menu.width, menu.height);
  vg_draw_object_buffer(menu.pixels, menu.width, menu.height, 0, 0, double_buf, vg_get_h_res(), vg_get_v_res());

  // draw mouse
  drawMouse();

  // update buffer in VRAM
  drawBufferInVRAM();
}

void set_graphicsGalleryMode(unsigned short* file_drawing)
{
  // draw gallery
  vg_draw_object_buffer(gallery.pixels, gallery.width, gallery.height, 0, 0, double_buf, vg_get_h_res(), vg_get_v_res());

  // draw file drawing
  drawAreaInDoubleBuffer(file_drawing, 72, 31, DRAW_SCREEN_H, DRAW_SCREEN_V);

  // draw mouse
  drawMouse();

  // update buffer in VRAM
  drawBufferInVRAM();
}

int drawAreaInDoubleBuffer(unsigned short* buffer, unsigned int x_upperleft_corner, unsigned int y_upperleft_corner,unsigned int dim_h, unsigned int dim_v)
{
  // check if the area fits in the double buffer
  if (dim_h + x_upperleft_corner > vg_get_h_res() || dim_v + y_upperleft_corner > vg_get_v_res())
  {
    printf("Graphic Module: Area does not fit temp buffer\n");
    return 1;
  }

  unsigned int y_position,i; // y_position for position in the screen buffer, i for position in the "buffer"

  for (y_position = y_upperleft_corner,i=0; y_position < y_upperleft_corner + dim_v; y_position++, i++)
    memcpy(double_buf + (y_position * vg_get_h_res() + x_upperleft_corner),buffer+(i*dim_h),2*dim_h);

  return 0;
}

int areasAreEqual(Draw_screen_area area1, Draw_screen_area area2)
{
  return ((area1.h_dim == area2.h_dim) && (area1.v_dim == area2.v_dim) && (area1.x_ul_corner == area2.x_ul_corner)
      && (area1.y_ul_corner == area2.y_ul_corner));
}

void drawMouse()
{
  unsigned int mouse_x = getxMousePosition() - 5, mouse_y = getyMousePosition() - 5;
  int i, j, k;

  for (i = 0; i < 10; i++) {
    k = 1;
    for (j = 10; j > 0; j--) {
      if (cursor.pixels[(100) - (i*10+k)] != 0xFFFF) // if the cursor pixel is different from white
        // check if the pixel where the cursor stands is black, if it is, print the cursor in white version
        if (vg_get_pixel_buffer(mouse_x + j, mouse_y + i, double_buf, vg_get_h_res(), vg_get_v_res()) == 0x2104
            || vg_get_pixel_buffer(mouse_x + j, mouse_y + i, double_buf, vg_get_h_res(), vg_get_v_res()) == 0x0)
          vg_set_pixel_buffer(mouse_x + j, mouse_y + i, 0xFFFF , double_buf, vg_get_h_res(), vg_get_v_res());

        else // print the cursor in black version
          vg_set_pixel_buffer(mouse_x + j, mouse_y + i, 0x0 , double_buf, vg_get_h_res(), vg_get_v_res());
      //vg_set_pixel_buffer(x_draw,y_draw,20,double_buf,vg_get_h_res(),vg_get_v_res());
      k++;
    }
  }
}

void drawToolBar(BTN* btnArray, unsigned short* buffer) {
  unsigned int i, y = 50;

  for (i = 0; i < 8; i++) {
    if (btnArray[i].press_state == 0) {
      vg_draw_object_buffer (btnArray[i].sprite_off.pixels, btnArray[i].sprite_off.width, btnArray[i].sprite_off.height, 10, y, buffer, vg_get_h_res(), vg_get_v_res());
    } else {
      vg_draw_object_buffer (btnArray[i].sprite_on.pixels, btnArray[i].sprite_on.width, btnArray[i].sprite_on.height, 10, y, buffer, vg_get_h_res(), vg_get_v_res());
    }
    y+=55;
  }

  y+=10;

  for (i = 8; i < 11; i++) {
    if (btnArray[i].press_state == 0) {
      vg_draw_object_buffer (btnArray[i].sprite_off.pixels, btnArray[i].sprite_off.width, btnArray[i].sprite_off.height, 10, y, buffer, vg_get_h_res(), vg_get_v_res());

    } else {
      vg_draw_object_buffer (btnArray[i].sprite_on.pixels, btnArray[i].sprite_on.width, btnArray[i].sprite_on.height, 10, y, buffer, vg_get_h_res(), vg_get_v_res());
    }
    y+=55;
  }

  y+=10;

  for (i = 11; i < 12; i++) {
    if (btnArray[i].press_state == 0) {
      vg_draw_object_buffer (btnArray[i].sprite_off.pixels, btnArray[i].sprite_off.width, btnArray[i].sprite_off.height, 10, y, buffer, vg_get_h_res(), vg_get_v_res());

    } else {
      vg_draw_object_buffer (btnArray[i].sprite_on.pixels, btnArray[i].sprite_on.width, btnArray[i].sprite_on.height, 10, y, buffer, vg_get_h_res(), vg_get_v_res());
    }
    y+=55;
  }
}


void drawText(int x, int y, char* text, unsigned short color, unsigned short* buffer, unsigned int dim_h, unsigned int dim_v) {

  unsigned int i;

  for (i=0; text[i] != '\0'; i++) {

    if (text[i] == ' ') { // if its a space, skip 20 pixels
      x+=20;
    } else if ( (int) text[i] > 47 && (int) text[i] < 58) { // if its a number
      vg_draw_char_buffer(letters[(int) text[i] - 48].pixels,letters[(int) text[i] - 48].width, letters[(int) text[i] - 48].height, x, y, color,buffer,dim_h,dim_v);
      x+=20;
    } else if ( (int) text[i] > 96 && (int) text[i] < 123) { // if its a letter
      vg_draw_char_buffer(letters[(int) text[i] - 87].pixels,letters[(int) text[i] - 87].width, letters[(int) text[i] - 87].height, x, y, color,buffer,dim_h,dim_v);
      x+=20;
    } else if (text[i] == '-') { // if its the special symbol "-"
      vg_draw_char_buffer(letters[36].pixels,letters[36].width, letters[36].height, x, y + 10, color,buffer,dim_h,dim_v);
      x+=20;
    }

  }
}

unsigned short* loadBMP (char const* filename, unsigned int * width, unsigned int * height) {

  // allocate memory for the file name
  char* path = (char*) malloc ( (strlen(filename) + 24) * sizeof (char));

  snprintf (path, (strlen(filename) + 24), "/home/lcom/proj/images/%s", filename);

  //printf("FILE:\n%s\n",path);

  // Open file
  FILE *bmp;
  char *mode = "rb";

  HEADER head;
  INFOHEADER info;

  bmp = fopen(path,mode);

  if (bmp == NULL) {
    printf("Cannot open file!\n");
    free (path);
    return NULL;
  }

  fread(&head.type,2,1,bmp);
  fread(&head.size,4,1,bmp);
  fread(&head.reserved1,2,1,bmp);
  fread(&head.reserved2,2,1,bmp);
  fread(&head.offset,4,1,bmp);

  fread(&info.size,4,1,bmp);
  fread(&info.width,4,1,bmp);
  fread(&info.height,4,1,bmp);
  fread(&info.planes,2,1,bmp);
  fread(&info.bits,2,1,bmp);
  fread(&info.compression,4,1,bmp);
  fread(&info.imagesize,4,1,bmp);
  fread(&info.xresolution,4,1,bmp);
  fread(&info.yresolution,4,1,bmp);
  fread(&info.ncolours,4,1,bmp);
  fread(&info.importantcolours,4,1,bmp);

  /*
	printf ("TYPE: %c\n", head.type);
	printf ("SIZE: %u\n", head.size);
	printf ("RESERVED1: %X\n", head.reserved1);
	printf ("RESERVED2: %X\n", head.reserved2);
	printf ("OFFSET: %u\n\n", head.offset);

	printf ("SIZE: %u\n", info.size);
	printf ("WIDTH: %u\n", info.width);
	printf ("HEIGHT: %u\n", info.height);
	printf ("PLANES: %u\n", info.planes);
	printf ("BITS: %u\n", info.bits);
	printf ("COMPRESSION: %u\n", info.compression);
	printf ("IMAGESIZE: %u\n", info.imagesize);
	printf ("XRESOLUTION: %u\n", info.xresolution);
	printf ("YRESOLUTION: %u\n", info.yresolution);
	printf ("NUMBER OF COLOURS: %u\n", info.ncolours);
	printf ("IMPORTANT COLOURS: %u\n\n", info.importantcolours);
   */

  // allocate memory for the buffer that has the image colors
  unsigned short* buffer = (unsigned short*) malloc (info.height * info.width * sizeof(unsigned short));

  if (buffer == NULL)
  {
    printf("Error allocating memory for bmp\n");
    free(path);
    return NULL;
  }

  //printf ("MEMORY ALLOCATED\n");
  fseek(bmp,head.offset,SEEK_SET);

  fread (buffer,2,info.height * info.width,bmp);

  *width = info.width;
  *height = info.height;

  fclose(bmp);

  free(path);

  return buffer;
}

int loadToolBar(BTN* btnArray) {

  unsigned int x,y,i;

  for (i = 1; i < 13; i++) {

    char file[15];

    snprintf (file, 15, "Button%02d_0.bmp", i);
    btnArray[i-1].sprite_off.pixels = loadBMP (file,&x,&y);
    btnArray[i-1].sprite_off.width = x;
    btnArray[i-1].sprite_off.height = y;
    snprintf (file, 15, "Button%02d_1.bmp", i);
    btnArray[i-1].sprite_on.pixels = loadBMP (file,&x,&y);
    btnArray[i-1].sprite_on.width = x;
    btnArray[i-1].sprite_on.height = y;
    btnArray[i-1].press_state = 0;

    if (btnArray[i-1].sprite_off.pixels == NULL || btnArray[i-1].sprite_on.pixels == NULL)
    {
      printf("Error in loading sprite in/off from buttons (toolbar)\n");
      return 1;
    }
  }

  return 0;
}

int loadColorBar(SPRITE* color_bar) {
  int x, y;
  char* file = "ColorBar.bmp";

  color_bar->pixels = loadBMP(file,&x,&y);
  color_bar->width = x;
  color_bar->height = y;

  if (color_bar->pixels == NULL)
  {
    printf("Error while allocating memory to load the color bar\n");
    return 1;
  }

  printf ("ColorBar loaded\n");
  return 0;
}

int loadCursor()
{
  SPRITE sprite_cursor;
  int x,y;

  sprite_cursor.width = x;
  sprite_cursor.height = y;
  sprite_cursor.pixels = loadBMP("Cursor.bmp",&x,&y);

  if (sprite_cursor.pixels == NULL)
  {
    printf("Error while allocating memory to load cursor\n");
    return 1;
  }

  cursor = sprite_cursor;

  printf ("Cursor loaded\n");
  return 0;
}

int loadChars() {

  SPRITE* sprite_buffer = (SPRITE*) malloc(37 * sizeof(SPRITE)); // allocating memory for char buffer

  unsigned int i , x , y;

  for (i = 0; i < 10; i++) {
    char file[15];

    snprintf(file, 15, "/letters/%u.bmp", i);
    sprite_buffer[i].pixels = loadBMP (file,&x,&y);

    if (sprite_buffer[i].pixels == NULL)
    {
      printf("Error loading char in loadChars()\n");
      return 1;
    }

    sprite_buffer[i].width = x;
    sprite_buffer[i].height = y;

    //printf("Loaded %u\n",i);
  }

  for (i = 10; i < 36; i++) {
    char file[15];

    snprintf(file, 15, "/letters/%c.bmp", (char) i+87);
    sprite_buffer[i].pixels = loadBMP (file,&x,&y);

    if (sprite_buffer[i].pixels == NULL)
    {
      printf("Error loading char in loadChars()\n");
      return 1;
    }

    sprite_buffer[i].width = x;
    sprite_buffer[i].height = y;
    //printf("Loaded %c\n",(char) i+87);
  }

  char file[15];

  snprintf(file, 15, "/letters/%c.bmp", '-');
  sprite_buffer[36].pixels = loadBMP (file,&x,&y);

  if (sprite_buffer[36].pixels == NULL)
  {
    printf("Error loading char in loadChars()\n");
    return 1;
  }

  sprite_buffer[36].width = x;
  sprite_buffer[36].height = y;

  letters = sprite_buffer;

  return 0;
}

int loadIntroGraphics()
{
  unsigned int x, y;
  intro.pixels = loadBMP ("intro.bmp",&x,&y);

  if (intro.pixels == NULL)
  {
    printf("Error loading intro BMP\n");
    return 1;
  }

  intro.width = x;
  intro.height = y;

  return 0;
}

void freeIntroGraphics()
{
  free(intro.pixels);
}

int loadMenuGraphics() {

  unsigned int x, y;
  menu.pixels = loadBMP ("menu.bmp",&x,&y);

  if (menu.pixels == NULL)
  {
    printf("Error loading menu BMP\n");
    return 1;
  }

  menu.width = x;
  menu.height = y;

  return 0;
}

void freeMenuGraphics() {
  free(menu.pixels);
}

int loadGalleryGraphics()
{
  unsigned int x, y;
  gallery.pixels = loadBMP ("gallery.bmp",&x,&y);

  if (gallery.pixels == NULL)
  {
    printf("Error loading gallery BMP\n");
    return 1;
  }

  gallery.width = x;
  gallery.height = y;

  return 0;
}

void freeGalleryGraphics()
{
  free(gallery.pixels);
}

int loadDrawing(int number, unsigned short* draw_screen) {

  // allocate memory for the file name
  char path[100];

  snprintf (path, 100, "/home/lcom/proj/drawings/drawing%03d",number);

  // Open file
  FILE *bmp;
  char *mode = "rb";

  bmp = fopen(path,mode);

  if (bmp == NULL) {
    printf("Cannot open file!\n");
    return 1;
  }

  fread (draw_screen,2, DRAW_SCREEN_H * DRAW_SCREEN_V,bmp);

  fclose(bmp);

  return 0;
}

void saveDrawing(int number, unsigned short* buffer)
{
  // allocate memory for the file name
  char path[100];

  snprintf (path, 100, "/home/lcom/proj/drawings/drawing%03d",number);

  printf ("Path: %s\n",path);

  // Open file
  FILE *bmp;
  char *mode = "wb";

  bmp = fopen(path,mode);

  if (bmp == NULL) {
    printf("Cannot open file!\n");
    return;
  }

  fwrite(buffer, DRAW_SCREEN_H* DRAW_SCREEN_V * sizeof(unsigned short),1,bmp);
  printf("GRAVOU!!!\n");

  fclose(bmp);

  printf("Closed\n");
  printf("Return\n");

  return;
}
