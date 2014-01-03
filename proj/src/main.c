#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include "central_module.h"

static int proc_args(int argc, char *argv[]);
static void print_usage(char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);

int main(int argc, char **argv)
{
  endpoint_t ep;
  char name[256];
  int priv_f;

  // Initialize service
  sef_startup();

  // Fetch our endpoint
  sys_whoami(&ep, name, 256, &priv_f);

  //Enable IO-sensitive operations for ourselves
  sys_enable_iop(ep);

  // For future use of lm_malloc
  lm_init();

  programInit();

  printf("Paint Minix Style @ Joao Neto e Miguel Sandim | LCOM 2013/2014\n");

  return 0;
}
