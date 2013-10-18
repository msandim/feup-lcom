#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

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
    if( argc != 2 ) {
      printf("test4.c: wrong no of arguments for test of int() \n");
      return 1;
    }

    // CHECK IF IT WORKS
    int j = 0;
    int length = 0;
    printf ("Enter length of array you wish to pass to function:\n");
    scanf("%u ", &length);

    int leds[length];

    while (j < length){
      printf("Enter element number %d in array: ");
      scanf("%u ", &leds[j]);
      j++;
    }

    unsigned short leds2[6] = {0,1,2,1,2,0};
    test_leds(6,leds2);
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
