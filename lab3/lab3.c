#include <minix/drivers.h>

#include "timer.h"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv) {

	/* Initialize service */

	sef_startup();

	/* For future use of lm_malloc */
	lm_init();

	if ( argc == 1 ) {
		print_usage(argv);
		return 0;
	} else {
		proc_args(argc, argv);
	}
	return 0;
}


// this function prints how to use this library
static void print_usage(char *argv[]) {
	printf("\n\nUsage: :\n"
			"-   service run %s -args \"square <freq>\" \n"
			"    . Tests programming timer in square wave mode \n"
			"-   service run %s -args \"int <time>\" \n"
			"    . Tests Timer 0 interrupt handling \n"
			"-   service run %s -args \"config <timer>\" \n"
			"    . Tests display of timer config \n\n",
			argv[0], argv[0], argv[0]);
}

// this function treats the args
static int proc_args(int argc, char *argv[]) {

	unsigned long freq, x1, x2, y1, y2, color, row, col, width, height, colorPixel;
	char *str;
	long num;

	// check the function to test: if the first characters match, accept it

	if (strncmp(argv[1], "square", strlen("square")) == 0) {
		if( argc != 3 ) {
			printf("timer.c: wrong no of arguments for test of square() \n");
			return 1;
		}

		// get the arguments
		if( (freq = parse_ulong(argv[2], 16)) == ULONG_MAX )
			return 1;

		printf("timer.c:: square(0x%X)\n", (unsigned) freq);
		return 0;


	} else if (strncmp(argv[1], "fill", strlen("fill")) == 0) {
		if( argc != 3 ) {
			printf("video_gr: wrong no of arguments for test of vg_fill() \n");
			return 1;
		}
		if( (color = parse_ulong(argv[2], 16)) == ULONG_MAX )
			return 1;

		vg_init(0x105);

		vg_fill(color);

		sleep(3);

		vg_exit();

		printf("video_gr:: vt_fill(0x%X)\n", (unsigned) color);
		return 0;

	} else if (strncmp(argv[1], "setpixel", strlen("setpixel")) == 0) {
		if( argc != 5 ) {
			printf("video_gr: wrong no of arguments for test of vg_set_pixel() \n");
			return 1;
		}
		if( (x1 = parse_ulong(argv[2], 10)) == ULONG_MAX )
			return 1;
		if( (y1 = parse_ulong(argv[3], 10)) == ULONG_MAX )
			return 1;
		if( (color = parse_ulong(argv[4], 16)) == ULONG_MAX )
			return 1;

		vg_init(0x105);

		vg_set_pixel(x1,y1,color);

		sleep(3);

		vg_exit();

		printf("video_gr:: vg_set_pixel(0x%lu, 0x%lu, %X)\n",
				x1, y1, color);


	} else if (strncmp(argv[1], "getpixel", strlen("getpixel")) == 0) {
		if( argc != 4 ) {
			printf("video_gr: wrong no of arguments for test of vg_get_pixel() \n");
			return 1;
		}
		if( (x1 = parse_ulong(argv[2], 10)) == ULONG_MAX )
			return 1;
		if( (y1 = parse_ulong(argv[3], 10)) == ULONG_MAX )
			return 1;

		vg_init(0x105);

		colorPixel = (unsigned) vg_get_pixel(x1,y1);

		vg_exit();

		printf("Pixel with color code: 0x%lu\n\n", colorPixel);

		printf("video_gr:: vg_get_pixel(%lu, %lu)\n",
				x1, y1);

	} else if (strncmp(argv[1], "drawline", strlen("drawline")) == 0) {
        if( argc != 7 ) {
                printf("video_gr: wrong no of arguments for test of vg_draw_line() \n");
                return 1;
        }
        if( (x1 = parse_long(argv[2], 10)) == LONG_MAX )
                return 1;
        if( (y1 = parse_ulong(argv[3], 10)) == ULONG_MAX )
                return 1;
        if( (x2 = parse_ulong(argv[4], 10)) == ULONG_MAX )
                return 1;
        if( (y2 = parse_ulong(argv[5], 10)) == ULONG_MAX )
                return 1;
        if( (color = parse_ulong(argv[6], 16)) == ULONG_MAX )
                return 1;

        vg_init(0x105);

        vg_draw_line(x1,y1,x2,y2,color);

        sleep(3);

        vg_exit();

        printf("video_gr:: vg_draw_line(%lu, %lu, %lu, %lu, 0x%X)\n",
                        x1, y1, x2, y2, color);
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
		printf("video_txt: parse_ulong: no digits were found in %s \n", str);
		return ULONG_MAX;
	}



	// Successful conversion

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

	// Successful conversion

	return val;
}
