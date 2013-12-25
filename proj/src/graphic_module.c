#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>

#include "graphic_module.h"
#include "video_gr.h"
#include "user_interaction.h"
#include <stdlib.h>

static unsigned short* double_buf; // temp buffer for graphic use

int screenInit()
{
	vg_init(0x117); // init in mode 0x117

	double_buf = (unsigned short*) malloc(vg_get_h_res()*vg_get_v_res()*sizeof(unsigned short));

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


int set_graphicsDrawMode(unsigned short* draw_screen, draw_screen_area draw_area, BTN* btn_array, SPRITE color_bar, unsigned short color_selected)
{
  // desenhar fundo
  vg_fill_buffer(color_selected,double_buf,vg_get_h_res(),vg_get_v_res());

	// draw toolboxes
	drawToolBar(btn_array,double_buf);

	// draw colorbox
	vg_draw_object_buffer(color_bar.pixels,color_bar.width,color_bar.height, 122, 700,double_buf,vg_get_h_res(),vg_get_v_res());

	// draw draw_screen
	drawAreaInDoubleBuffer(tela, 122, 30, 880, 650);
	print_text(500,200,"hello - world", 0xF900, chars, double_buf,vg_get_h_res(),vg_get_v_res());
	// draw mouse
	draw_mouse();

	return 0;
}

int drawAreaInDoubleBuffer(unsigned short* buffer, unsigned int x_upperleft_corner, unsigned int y_upperleft_corner,unsigned int dim_h, unsigned int dim_v)
{
	// check if the area fits in the double buffer
	if (dim_h + x_upperleft_corner >= vg_get_h_res() || dim_v + y_upperleft_corner >= vg_get_v_res())
	{
		printf("Graphic Module: Area does not fit temp buffer\n");
		return 1;
	}

	unsigned int y_position,i; // y_position for position in the screen buffer, i for possition in the "buffer"

	for (y_position = y_upperleft_corner,i=0; y_position < y_upperleft_corner + dim_v; y_position++, i++)
		memcpy(double_buf + (y_position * vg_get_h_res() + x_upperleft_corner),buffer+(i*dim_h),2*dim_h);

	return 0;
}
int draw_mouse()
{
	// desenhar quadrado de lado 3, centrado em X,Y
	// estes parametros podem ser acedidos pelo rato AQUI!

	unsigned int mouse_x = getxMousePosition(), mouse_y = getyMousePosition();
	unsigned x_draw = mouse_x, y_draw = mouse_y;

	vg_set_pixel_buffer(x_draw,y_draw,20,double_buf,vg_get_h_res(),vg_get_v_res());

	int i = 0;

	while (i != 8)
	{
		if (x_draw < vg_get_h_res() && y_draw < vg_get_v_res()) // se tas nas medidas
			vg_set_pixel_buffer(x_draw,y_draw,20,double_buf,vg_get_h_res(),vg_get_v_res());

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

	for (i = 1; i < 12; i++) {

		char* file = (char*) malloc (15*sizeof(char)); // malloc for button string

		if (file == NULL)
		{
			printf("Could not allocate memory for loading the toolbar\n");
			return 1;
		}

		snprintf (file, 15, "Button%02d_0.bmp", i);
		btnArray[i-1].sprite_off.pixels = loadBMP (file,&x,&y);
		btnArray[i-1].sprite_off.width = x;
		btnArray[i-1].sprite_off.height = y;
		snprintf (file, 15, "Button%02d_1.bmp", i);
		btnArray[i-1].sprite_on.pixels = loadBMP (file,&x,&y);
		btnArray[i-1].sprite_on.width = x;
		btnArray[i-1].sprite_on.height = y;
		btnArray[i-1].press_state = 0;

		free(file); // free the memory

		if (btnArray[i-1].sprite_off.pixels == NULL || btnArray[i-1].sprite_on.pixels == NULL)
		{
			printf("Error in loading sprite in/off from buttons (toolbar)\n");
			return 1;
		}
	}

	return 0;
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
	for (i = 8; i < 10; i++) {
		if (btnArray[i].press_state == 0) {
			vg_draw_object_buffer (btnArray[i].sprite_off.pixels, btnArray[i].sprite_off.width, btnArray[i].sprite_off.height, 10, y, buffer, vg_get_h_res(), vg_get_v_res());

		} else {
			vg_draw_object_buffer (btnArray[i].sprite_on.pixels, btnArray[i].sprite_on.width, btnArray[i].sprite_on.height, 10, y, buffer, vg_get_h_res(), vg_get_v_res());
		}
		y+=55;
	}
	y+=10;
	for (i = 10; i < 11; i++) {
		if (btnArray[i].press_state == 0) {
			vg_draw_object_buffer (btnArray[i].sprite_off.pixels, btnArray[i].sprite_off.width, btnArray[i].sprite_off.height, 10, y, buffer, vg_get_h_res(), vg_get_v_res());

		} else {
			vg_draw_object_buffer (btnArray[i].sprite_on.pixels, btnArray[i].sprite_on.width, btnArray[i].sprite_on.height, 10, y, buffer, vg_get_h_res(), vg_get_v_res());
		}
		y+=55;
	}
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

int loadChars(SPRITE* chars) {
	unsigned int i , x , y;

	for (i = 0; i < 10; i++) {
		char* file = (char*) malloc (15*sizeof(char)); // malloc for button string

		if (file == NULL)
		{
			printf("Could not allocate memory for loading the toolbar\n");
			return 1;
		}

		snprintf (file, 15, "%u.bmp", i);
		chars[i].pixels = loadBMP (file,&x,&y);
		chars[i].width = x;
		chars[i].height = y;

		printf("Loaded %u\n",i);
	}

	for (i = 10; i < 36; i++) {
		char* file = (char*) malloc (15*sizeof(char)); // malloc for button string

		if (file == NULL)
		{
			printf("Could not allocate memory for loading the toolbar\n");
			return 1;
		}

		snprintf (file, 15, "%c.bmp", (char) i+87);
		chars[i].pixels = loadBMP (file,&x,&y);
		chars[i].width = x;
		chars[i].height = y;
		printf("Loaded %c\n",(char) i+87);
	}
	char* file = (char*) malloc (15*sizeof(char)); // malloc for button string

	if (file == NULL)
	{
		printf("Could not allocate memory for loading the toolbar\n");
		return 1;
	}
	snprintf (file, 15, "%c.bmp", '-');
	chars[36].pixels = loadBMP (file,&x,&y);
	chars[36].width = x;
	chars[36].height = y;

	return 0;
}

void print_text (int x, int y, char* text, unsigned short color, SPRITE* letters, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v) {

	unsigned int i;

	for (i=0; text[i] != '\0'; i++) {
		if (text[i] == ' ') {
			x+=20;
		} else if ( (int) text[i] > 47 && (int) text[i] < 58) {
			vg_draw_char_buffer(letters[(int) text[i] - 48].pixels,letters[(int) text[i] - 48].width, letters[(int) text[i] - 48].height, x, y, color,buffer,dim_h,dim_v);
			x+=20;
		} else if ( (int) text[i] > 96 && (int) text[i] < 123) {
			vg_draw_char_buffer(letters[(int) text[i] - 87].pixels,letters[(int) text[i] - 87].width, letters[(int) text[i] - 87].height, x, y, color,buffer,dim_h,dim_v);
			x+=20;
		} else if (text[i] == '-') {
			vg_draw_char_buffer(letters[36].pixels,letters[36].width, letters[36].height, x, y + 10, color,buffer,dim_h,dim_v);
			x+=20;
		}

	}
}
