#include <minix/drivers.h>

#include "vbe.h"
#include "video_gr.h"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv) {

	char *video_mem;

	unsigned long color;

	/* Initialize service */

	sef_startup();

	video_mem = vg_init(0x105);
	vg_exit();

	//vg_fill(0x34);

	//sleep(3);

	if ( argc == 1 ) {
		printf("\nVRAM virtual address at %p\n\n", video_mem);
		print_usage(argv);
		return 0;
	} else {
		proc_args(argc, argv);
		vg_exit();
	}
	return 0;

}

static void print_usage(char *argv[]) {
	printf("Usage: one of the following:\n"
			//			int vg_set_pixel(unsigned long x, unsigned long y, unsigned long color);
			"\t service run %s -args \"init\ <mode>\" \n"
			"\t service run %s -args \"fill <color>\" \n"
			"\t service run %s -args \"setpixel <line no> <col no> <color>\" \n"
			"\t service run %s -args \"getpixel <string> <hex-attr> <line no> <col no>\" \n"
			"\t service run %s -args \"int <decimal number> <hex-attr> <line no> <col no>\" \n"
			"\t service run %s -args \"frame <width> <height> <hex-attr> <line no> <col no>\" \n",
			argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {

	unsigned long mode, x1, x2, y1, y2, color, row, col, attr, width, height;
	char *str;
	long num;

	/* check the function to test: if the first characters match, accept it */
	if (strncmp(argv[1], "init", strlen("init")) == 0) {
		if( argc != 3 ) {
			printf("video_gr: wrong no of arguments for test of vg_init() \n");
			return 1;
		}

		if( (mode = parse_ulong(argv[2], 16)) == ULONG_MAX )
			return 1;

		printf("video_gr:: vg_init(%ul)\n", mode);
		sleep(1);

		vg_init(mode);
		return 0;
		// Done
	} else if (strncmp(argv[1], "fill", strlen("fill")) == 0) {
		if( argc != 3 ) {
			printf("video_gr: wrong no of arguments for test of vg_fill() \n");
			return 1;
		}
		if( (color = parse_ulong(argv[2], 16)) == ULONG_MAX )
			return 1;

		printf("video_gr:: vt_fill(0x%lu)\n", (unsigned)color);

		vg_fill(color);
		return 0;
		// Done
	} else if (strncmp(argv[1], "setpixel", strlen("setpixel")) == 0) {
		if( argc != 6 ) {
			printf("video_gr: wrong no of arguments for test of vg_set_pixel() \n");
			return 1;
		}
		if( (row = parse_ulong(argv[2], 16)) == ULONG_MAX )
			return 1;
		if( (col = parse_ulong(argv[3], 16)) == ULONG_MAX )
			return 1;
		if( (color = parse_ulong(argv[4], 16)) == ULONG_MAX )
			return 1;
		printf("video_gr:: vg_set_pixel(0x%lu, 0x%lu, %X)\n",
				row, col, color);
		return vg_set_pixel(col,row,color);
		// Done
	} else if (strncmp(argv[1], "getpixel", strlen("getpixel")) == 0) {
		if( argc != 6 ) {
			printf("video_gr: wrong no of arguments for test of vg_get_pixel() \n");
			return 1;
		}
		if( (col = parse_ulong(argv[2], 16)) == ULONG_MAX )
			return 1;
		if( (row = parse_ulong(argv[3], 10)) == ULONG_MAX )
			return 1;
		printf("video_gr:: vg_get_pixel(%lu, 0x%lu)\n",
				col, row);
		return vg_get_pixel(col, row);
		// Done
	} else if (strncmp(argv[1], "drawline", strlen("drawline")) == 0) {
		if( argc != 6 ) {
			printf("video_gr: wrong no of arguments for test of vg_draw_line() \n");
			return 1;
		}
		if( (x1 = parse_long(argv[2], 16)) == LONG_MAX )
			return 1;
		if( (y1 = parse_ulong(argv[3], 16)) == ULONG_MAX )
			return 1;
		if( (x2 = parse_ulong(argv[4], 16)) == ULONG_MAX )
			return 1;
		if( (y2 = parse_ulong(argv[5], 16)) == ULONG_MAX )
			return 1;
		if( (color = parse_ulong(argv[6], 16)) == ULONG_MAX )
			return 1;
		printf("video_gr:: vg_draw_line(%lu, %lu, %lu, %lu, %lu)\n",
				x1, y1, x2, y2, color);
		return vg_draw_line(x1,y1,x2,y2,color);
	} else {
		printf("video_gr: non valid function \"%s\" to test\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base) {
	char *endptr;
	unsigned long val;

	val = strtoul(str, &endptr, base);

	if ((errno == ERANGE && val == ULONG_MAX )
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		return ULONG_MAX;

	}
	if (endptr == str) {
		printf("video_gr: parse_ulong: no digits were found in %s \n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}

static long parse_long(char *str, int base) {
	char *endptr;
	unsigned long val;

	val = strtol(str, &endptr, base);

	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		return LONG_MAX;
	}

	if (endptr == str) {
		printf("video_gr: parse_long: no digits were found in %s \n", str);
		return LONG_MAX;
	}

	/* Successful conversion */
	return val;
}
