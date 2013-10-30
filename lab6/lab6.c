#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <stdio.h>

#include "test6.h"

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

	if (strncmp(argv[1], "config", strlen("config")) == 0) {
		if( argc != 2 ) {
			printf("test6.c: wrong no of arguments for test config \n");
			return 1;
		}

		rtc_test_conf();
		printf("\ntest6.c::rtc_test_conf()\n\n");
		return 0;

	} else if (strncmp(argv[1], "date", strlen("date")) == 0) {

		if( argc != 2 ) {
			printf("test6.c: wrong no of arguments for test of date \n");
			return 1;
		}

		rtc_test_date();
		printf("test6.c:: rtc_test_date()\n\n");
		return 0;

	} else if (strncmp(argv[1], "interrupt", strlen("interrupt")) == 0) {

		if( argc != 2 ) {
			printf("test6.c: wrong no of arguments for test of interrupt \n");
			return 1;
		}


		rtc_test_int();

		printf("test6.c:: rtc_test_int()\n\n");
		return 0;

	} else {
		printf("test6.c: non valid function \"%s\" to test\n", argv[1]);
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
		printf("test6.c: parse_ulong: no digits were found in %s \n", str);
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
		printf("test6.c: parse_long: no digits were found in %s \n", str);
		return LONG_MAX;
	}

	// Successful conversion

	return val;
}
