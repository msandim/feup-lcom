#include <minix/drivers.h>

#include "vbe.h"
#include "video_gr.h"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv) {

	char *video_mem;
	vt_info_t vt_info;

	/* Initialize service */

	sef_startup();

	/* Get video text mode info */

	vt_info_get(&vt_info);

	/* Display video text mode info */
	vt_info_display(&vt_info);

	video_mem = vt_init(&vt_info);

	printf("lab1: VRAM mapped on virtual address %p\n", video_mem);

	if ( argc == 1 ) {
		print_usage(argv);
		return 0;
	} else {
		proc_args(argc, argv);
	}
	return 0;

}

static void print_usage(char *argv[]) {
	printf("Usage: one of the following:\n"
//			int vg_set_pixel(unsigned long x, unsigned long y, unsigned long color);
	"\t service run %s -args \"init\" \n"
	"\t service run %s -args \"fill <color>\" \n"
	"\t service run %s -args \"setpixel <line no> <col no> <color>\" \n"
	"\t service run %s -args \"getpixel <string> <hex-attr> <line no> <col no>\" \n"
	"\t service run %s -args \"int <decimal number> <hex-attr> <line no> <col no>\" \n"
	"\t service run %s -args \"frame <width> <height> <hex-attr> <line no> <col no>\" \n",
	argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {

	unsigned long ch, row, col, attr, width, height;
	char *str;
	long num;

	/* check the function to test: if the first characters match, accept it */
	if (strncmp(argv[1], "init", strlen("init")) == 0) {
		if( argc != 2 ) {
			printf("video_gr: wrong no of arguments for test of vg_init() \n");
			return 1;
		}
		printf("video_gr:: vg_init()\n"); /* Actually, it was already invoked */
		vg_init();
		return 0;
		// Done
	} else if (strncmp(argv[1], "fill", strlen("fill")) == 0) {
		if( argc != 3 ) {
			printf("video_gr: wrong no of arguments for test of vg_fill() \n");
			return 1;
		}
		if( (color = parse_ulong(argv[2], 16)) == ULONG_MAX )
			return 1;
		printf("video_gr:: vt_fill(0x%X)\n",
				(unsigned)color);
		vt_fill(color);
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
		return vt_set_pixel(col,row,color);
		// Done
	} else if (strncmp(argv[1], "getpixel", strlen("getpixel")) == 0) {
		if( argc != 6 ) {
			printf("video_gr: wrong no of arguments for test of vt_get_pixel() \n");
			return 1;
		}
		if( (col = parse_ulong(argv[2], 16)) == ULONG_MAX )
			return 1;
		if( (row = parse_ulong(argv[3], 10)) == ULONG_MAX )
			return 1;
		printf("video_gr:: vg_get_pixel(%lu, 0x%lu)\n",
				col, row);
		return vt_print_string(col, row);
		// Done
	} else if (strncmp(argv[1], "drawline", strlen("drawline")) == 0) {
		if( argc != 6 ) {
			printf("video_gr: wrong no of arguments for test of vt_draw_line() \n");
			return 1;
		}
		if( (num = parse_long(argv[2], 10)) == LONG_MAX )
			return 1;
		if( (attr = parse_ulong(argv[3], 16)) == ULONG_MAX )
			return 1;
		if( (row = parse_ulong(argv[4], 10)) == ULONG_MAX )
			return 1;
		if( (col = parse_ulong(argv[5], 10)) == ULONG_MAX )
			return 1;
		printf("video_gr:: vg_draw_line(%lu, 0x%X, %lu, %lu)\n",
				num, (unsigned)attr, row, col);
		return vt_print_int(num, attr, row, col);
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
