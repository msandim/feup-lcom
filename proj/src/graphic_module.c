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


int set_graphicsDrawMode(short* tela, BTN* btn_array, SPRITE color_bar)
{


	// desenhar fundo
	vg_fill_buffer(65535,double_buf,vg_get_h_res(),vg_get_v_res());
	vg_draw_line_buffer(500,200,500,399,0,double_buf,vg_get_h_res(),vg_get_v_res());
	vg_draw_line_buffer(500,399,699,399,0,double_buf,vg_get_h_res(),vg_get_v_res());
	vg_draw_line_buffer(699,399,699,200,0,double_buf,vg_get_h_res(),vg_get_v_res());
	vg_draw_line_buffer(699,200,500,200,0,double_buf,vg_get_h_res(),vg_get_v_res());


	vg_draw_line_buffer(0,0,500,200,30,double_buf,vg_get_h_res(),vg_get_v_res());
	// draw toolboxes
	drawToolBar(btn_array,double_buf);
	vg_draw_object_buffer(color_bar.pixels,color_bar.width,color_bar.height, 122, 700,double_buf,vg_get_h_res(),vg_get_v_res());
	// draw a tela
	drawAreaInDoubleBuffer(tela, 500, 200, 200, 200);

	// draw o rato
	draw_mouse();

	drawBufferInVRAM();

	return 0;
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

unsigned short* loadBMP (char const* filename, unsigned int * width, unsigned int * height) {

	char* path = (char*) malloc ( (strlen(filename) + 24) * sizeof (char));

	snprintf (path, (strlen(filename) + 24), "/home/lcom/proj/images/%s", filename);

	printf("FILE:\n%s\n",path);


	FILE *bmp;
	char *mode = "rb";

	HEADER* head = (HEADER*) malloc (sizeof(HEADER));
	INFOHEADER* info = (INFOHEADER*) malloc (sizeof(INFOHEADER));

	bmp = fopen(path,mode);

	if (bmp == NULL) {
		printf("Cannot open file!\n");
		return NULL;
	}

	fread(&head->type,2,1,bmp);
	fread(&head->size,4,1,bmp);
	fread(&head->reserved1,2,1,bmp);
	fread(&head->reserved2,2,1,bmp);
	fread(&head->offset,4,1,bmp);

	fread(&info->size,4,1,bmp);
	fread(&info->width,4,1,bmp);
	fread(&info->height,4,1,bmp);
	fread(&info->planes,2,1,bmp);
	fread(&info->bits,2,1,bmp);
	fread(&info->compression,4,1,bmp);
	fread(&info->imagesize,4,1,bmp);
	fread(&info->xresolution,4,1,bmp);
	fread(&info->yresolution,4,1,bmp);
	fread(&info->ncolours,4,1,bmp);
	fread(&info->importantcolours,4,1,bmp);

	/*
	printf ("TYPE: %c\n", head->type);
	printf ("SIZE: %u\n", head->size);
	printf ("RESERVED1: %X\n", head->reserved1);
	printf ("RESERVED2: %X\n", head->reserved2);
	printf ("OFFSET: %u\n\n", head->offset);

	printf ("SIZE: %u\n", info->size);
	printf ("WIDTH: %u\n", info->width);
	printf ("HEIGHT: %u\n", info->height);
	printf ("PLANES: %u\n", info->planes);
	printf ("BITS: %u\n", info->bits);
	printf ("COMPRESSION: %u\n", info->compression);
	printf ("IMAGESIZE: %u\n", info->imagesize);
	printf ("XRESOLUTION: %u\n", info->xresolution);
	printf ("YRESOLUTION: %u\n", info->yresolution);
	printf ("NUMBER OF COLOURS: %u\n", info->ncolours);
	printf ("IMPORTANT COLOURS: %u\n\n", info->importantcolours);
	 */
	unsigned short* buffer = (unsigned short*) malloc (info->height * info->width * sizeof(unsigned short));
	printf ("MEMORY ALLOCATED\n");
	fseek(bmp,head->offset,SEEK_SET);

	fread (buffer,2,info->height*info->width,bmp);


	*width = info->width;
	*height = info->height;

	fclose (bmp);

	return buffer;
}

void loadToolBar(BTN* btnArray) {

	unsigned int x,y,i;

	for (i = 1; i < 12; i++) {
		char* file = (char*) malloc (15*sizeof(char));
		snprintf (file, 15, "Button%02d_0.bmp", i);
		btnArray[i-1].sprite_off.pixels = loadBMP (file,&x,&y);
		btnArray[i-1].sprite_off.width = x;
		btnArray[i-1].sprite_off.height = y;
		snprintf (file, 15, "Button%02d_1.bmp", i);
		btnArray[i-1].sprite_on.pixels = loadBMP (file,&x,&y);
		btnArray[i-1].sprite_on.width = x;
		btnArray[i-1].sprite_on.height = y;
		btnArray[i-1].press_state = 0;
	}
}

void drawToolBar(BTN* btnArray, short* buffer) {
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

void loadColorBar(SPRITE* color_bar) {
	int x, y;
	char* file = "ColorBar.bmp";

	color_bar->pixels = loadBMP(file,&x,&y);
	color_bar->width = x;
	color_bar->height = y;

	printf ("ColorBar loaded\n");
}
