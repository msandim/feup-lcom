#include <minix/drivers.h>

#include "timer.h"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv) {

	/* Initialize service */
	sef_startup();

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

	unsigned long freq, time, timer;

	// check the function to test: if the first characters match, accept it

	if (strncmp(argv[1], "square", strlen("square")) == 0) {
		if( argc != 3 ) {
			printf("timer.c: wrong no of arguments for test of square() \n");
			return 1;
		}

		// get the arguments
		if( (freq = parse_ulong(argv[2], 10)) == ULONG_MAX )
			return 1;

		timer_test_square(freq);

		printf("timer.c:: square(%lu)\n", (unsigned) freq);
		return 0;


	} else if (strncmp(argv[1], "int", strlen("int")) == 0) {
		if( argc != 3 ) {
			printf("timer.c: wrong no of arguments for test of int() \n");
			return 1;
		}
		if( (time = parse_ulong(argv[2], 10)) == ULONG_MAX )
			return 1;

		timer_test_int(time);


		printf("timer.c:: int(%lu)\n", (unsigned) time);
		return 0;

	} else if (strncmp(argv[1], "setpixel", strlen("setpixel")) == 0) {
		if( argc != 5 ) {
			printf("timer.c: wrong no of arguments for test of config() \n");
			return 1;
		}
		if( (timer = parse_ulong(argv[2], 10)) == ULONG_MAX )
			return 1;

		timer_test_config(timer);

		printf("timer.c:: config(%lu)\n", (unsigned) timer);

	} else {
		printf("timer.c: non valid function \"%s\" to test\n", argv[1]);
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
