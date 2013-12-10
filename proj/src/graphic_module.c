#include "graphic_module.h"

#include "video_gr.h"
#include <stdio.h>
void screenInit()
{
	vg_init(0x117); // init in mode 0x117
}

void screenExit()
{
	vg_exit();
}

int set_drawMode(unsigned int mouse_x, unsigned int mouse_y, BTN* btn_array)
{
	// desenhar fundo
	// draw toolboxes

	drawToolBar(btn_array);

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

void drawToolBar(BTN* btnArray) {
	unsigned int i;

	for (i = 0; i < 11; i++) {
		if (btnArray[i].press_state == 0) {
			vg_draw_object (btnArray[i].sprite_off.pixels, btnArray[i].sprite_off.width, btnArray[i].sprite_off.height, 10, 50+i*55);
		}
	}
}
