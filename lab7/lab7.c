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
			"-   service run %s -args \"read_config <base address>\" \n"
			"    . Tests reading the configuration of the serial Port\n"
			"-   service run %s -args \"set_config <base address>\" \n"
			"    . Tests setting the configuration of the serial Port\n"
			"-   service run %s -args \"poll <base address> <0(receive)/1(transmit)> <number of bits> <number of stop bits> <parity> <rate> <number of strings to transmit> <string1> <string2> ...\" \n"
			"    . Tests polling communication through serial Port\n",
			argv[0], argv[0], argv[0]);
}

// this function treats the args
static int proc_args(int argc, char *argv[]) {

	// check the function to test: if the first characters match, accept it

	unsigned short address;

	if (strncmp(argv[1], "read_config", strlen("read_config")) == 0) {
		if( argc != 3 ) {
			printf("test7.c: wrong no of arguments to test read_config \n");
			return 1;
		}

		if( (address = parse_ulong(argv[2], 16)) == ULONG_MAX )
			return 1;

		int ser_test_conf(address);

		printf("\ntest7.c::read_config(base_address)\n\n");
		return 0;

	} else if (strncmp(argv[1], "set_config", strlen("set_config")) == 0) {

		if( argc != 3 ) {
			printf("test7.c: wrong no of arguments to test set_config \n");
			return 1;
		}

		if( (address = parse_ulong(argv[2], 16)) == ULONG_MAX )
			return 1;

		int ser_test_set(address);

		printf("\ntest7.c::set_config(base_address)\n\n");
		return 0;

	} else if (strncmp(argv[1], "poll", strlen("poll")) == 0) {

		if( argc != 3 ) {
			printf("test7.c: wrong no of arguments for test of poll \n");
			return 1;
		}

		unsigned short strings [argc-2];

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
