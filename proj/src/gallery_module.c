#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>

#include <string.h>

#include "graphic_module.h"

static const char* drawings_path = "/home/lcom/proj/drawings/";

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
	file_draw.width = DRAW_SCREEN_H;
	file_draw.height = DRAW_SCREEN_V;

	file_draw.pixels = (unsigned short*) malloc(DRAW_SCREEN_H * DRAW_SCREEN_V * sizeof(unsigned short));

	if (file_draw.pixels == NULL)
	{
		printf("Could not allocate file_draw SPRITE\n");
		return 1;
	}

	FILE *file;
	char *mode = "r";

	file = fopen("/home/lcom/proj/drawings/drawings",mode);

	fscanf(file,"%d", &total);

	printf("Total: %u\n",total);

	fclose(file);

	file_number = 0;

	return 0;
}

void galleryModeFree()
{
	FILE *file;
	char *mode = "w";

	file = fopen("/home/lcom/proj/drawings/drawings",mode);

	fprintf(file, "%d", total);

	printf("Total: %u\n",total);

	fclose(file);

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
		file_draw.pixels = loadDrawing(file_number, file_draw.width, file_draw.height);

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
		/*
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
		 */
		printf("NEXT\nNumber %u\nTotal %u\n",file_number + 1,total);
		file_draw.pixels = loadDrawing(++file_number,file_draw.width, file_draw.height);
	}

	// click on the left
	else if (getxMousePosition() >= 0 &&
			getxMousePosition() <= 72 &&
			getyMousePosition() >= 0 &&
			getyMousePosition() <= 767 &&
			getMouseLBstate() && file_number != 0 && !previous_LB_state)
	{
		/*
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
		 */
		printf("PREVIOUS\nNumber %u\nTotal %u\n",file_number - 1,total);
		file_draw.pixels = loadDrawing(--file_number,file_draw.width, file_draw.height);
	}

	else if (getxMousePosition() >= 170 &&
			getxMousePosition() <= 420 &&
			getyMousePosition() >= 687 &&
			getyMousePosition() <= 756 &&
			getMouseLBstate() && !previous_LB_state)
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
