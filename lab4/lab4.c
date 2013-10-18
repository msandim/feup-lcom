#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <stdio.h>

#include "test4.h"

static int proc_args(int argc, char *argv[]);
static void print_usage(char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);

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
/*
 * int test_scan(void);
 * int test_leds(unsigned short n, unsigned short *leds);
 */

// this function prints how to use this library
static void print_usage(char *argv[]) {
	printf("\n\nUsage: :\n"
			"-   service run %s -args \"scan\" \n"
			"    . Tests reading the scancodes from the KBC using an interrupt handler \n"
			"-   service run %s -args \"leds <time (decimal)>\" \n"
			"    . Tests leds \n",
			argv[0], argv[0]);
}

// this function treats the args
static int proc_args(int argc, char *argv[]) {

	// check the function to test: if the first characters match, accept it

	if (strncmp(argv[1], "scan", strlen("scan")) == 0) {
		if( argc != 2 ) {
			printf("test4.c: wrong no of arguments for test scan \n");
			return 1;
		}

		test_scan();
		printf("\ntest4.c::test_scan()\n\n");
		return 0;

	} else if (strncmp(argv[1], "leds", strlen("leds")) == 0) {

		if( argc < 2 ) {
			printf("test4.c: wrong no of arguments for test of leds() \n");
			return 1;
		}

		// CHECK IF IT WORKS


		int i = 2;

		unsigned short arguments [argc-2];

		while (i < argc ){
			unsigned long argument;

			if ( (argument =  parse_ulong(argv[i], 10)) == ULONG_MAX || (argument > 2)){

				printf("Led number %u, %u, is not valid.\n", i-1, argument);

				return 1;
			}

			arguments[i-2] = (unsigned short)argument;
			i++;
		}

		i = 2;

		while (i < argc){
			printf ("%d\n",arguments[i-2]);
			i++;
		}

		test_leds(argc-2, arguments);

		printf("test4.c:: test_leds()\n\n");
		return 0;

	} else {
		printf("test4.c: non valid function \"%s\" to test\n", argv[1]);
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
		printf("test4.c: parse_ulong: no digits were found in %s \n", str);
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
		printf("test4.c: parse_long: no digits were found in %s \n", str);
		return LONG_MAX;
	}

	// Successful conversion

	return val;
}
