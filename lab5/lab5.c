#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <stdio.h>

#include "test5.h"

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
 * int test_packet();
 * int test_asynch(unsigned short duration);
 * int test_config(void);
 */

// this function prints how to use this library
static void print_usage(char *argv[]) {
	printf("\n\nUsage:\n"
			"-   service run %s -args \"packet\" \n"
			"    . Tests reading the command byte from mouse \n"
			"-   service run %s -args \"asynch <time (secs)>\" \n"
			"    . Tests measuring time from timer 0\n"
			"-   service run %s -args \"config\" \n"
			"    . Shows mouse configuration \n",
			argv[0], argv[0], argv[0]);
}

// this function treats the args
static int proc_args(int argc, char *argv[]) {

	// check the function to test: if the first characters match, accept it

	if (strncmp(argv[1], "packet", strlen("packet")) == 0) {
		if( argc != 2 ) {
			printf("test5.c: wrong no of arguments for test packet \n");
			return 1;
		}

		test_packet();
		printf("\ntest5.c::test_packet()\n\n");
		return 0;

	} else if (strncmp(argv[1], "asynch", strlen("asynch")) == 0) {

		if( argc != 3 ) {
			printf("test5.c: wrong no of arguments for test of asynch \n");
			return 1;
		}

		unsigned short duration;

		if( (duration = parse_ulong(argv[2], 10)) == ULONG_MAX )
			return 1;

		test_asynch(duration);

		printf("test5.c:: test_asynch()\n\n");
		return 0;

	} else if (strncmp(argv[1], "config", strlen("config")) == 0) {

		if( argc != 2 ) {
			printf("test5.c: wrong no of arguments for test of config \n");
			return 1;
		}


		test_config();

		printf("test5.c:: test_config()\n\n");
		return 0;

	} else {
		printf("test5.c: non valid function \"%s\" to test\n", argv[1]);
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
		printf("test5.c: parse_ulong: no digits were found in %s \n", str);
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
		printf("test5.c: parse_long: no digits were found in %s \n", str);
		return LONG_MAX;
	}

	// Successful conversion

	return val;
}
