#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "serial.h"

static int hook_id;

int ser_get_reg(unsigned short base_addr, unsigned char relative_addr, unsigned long* result)
{
  if (sys_inb(base_addr + relative_addr, result) != OK)
  {
    printf("serial.c: Error getting register in 0x%x",base_addr+relative_addr);
    return 1;
  }
  else
    return 0;
}

int ser_set_reg(unsigned short base_addr, unsigned char relative_addr, unsigned long value)
{
  if (sys_outb(base_addr + relative_addr, value) != OK)
  {
    printf("serial.c: Error writting in register in 0x%x",base_addr+relative_addr);
    return 1;
  }
  else
    return 0;
}

int ser_get_bit_rate(unsigned short base_addr,unsigned long* bit_rate)
{
  // get LCR
  unsigned long lcr;

  if (ser_get_reg(base_addr,UART_LCR,&lcr))
    return 1;

  // set DLAB in LCR
  lcr |= UART_LCR_DLAB;

  // save new LCR
  if (ser_set_reg(base_addr,UART_LCR,lcr))
    return 1;

  // read DLL and DLM
  unsigned long dll,dlm;
  if (ser_get_reg(base_addr,UART_DLL,&dll) || ser_get_reg(base_addr,UART_DLM,&dlm))
    return 1;

  // disable DLAB and save
  lcr ^= UART_LCR_DLAB;
  if (ser_set_reg(base_addr,UART_LCR,lcr))
    return 1;

  // return bit_rate by parameter
  *bit_rate = BITRATE_CONSTANT/((dlm << 8) | dll);

  return 0;
}

int ser_set_bit_rate(unsigned short base_addr,unsigned long bit_rate)
{
  // get LCR
  unsigned long lcr;

  if (ser_get_reg(base_addr,UART_LCR,&lcr))
    return 1;

  // set DLAB in LCR
  lcr |= UART_LCR_DLAB;

  // save new LCR
  if (ser_set_reg(base_addr,UART_LCR,lcr))
    return 1;

  // set DLL and DLM
  unsigned long div = BITRATE_CONSTANT/bit_rate;
  unsigned long dll = div & (0x0F);
  unsigned long dlm = (div >> 8);

  if (ser_set_reg(base_addr,UART_DLL,dll) || ser_set_reg(base_addr,UART_DLM,dlm))
    return 1;

  // disable DLAB
  lcr ^= UART_LCR_DLAB;

  if (ser_set_reg(base_addr,UART_LCR,lcr))
    return 1;

  return 0;
}

void ser_show_lcr(unsigned long lcr)
{
  printf("LCR REGISTER: 0x%x\n\n", lcr);

  printf("Number of bits per char: ");

  if ((lcr & UART_LCR_WORD_LENGTH) == UART_LCR_WORD_LENGTH_5)
    printf("5\n");
  else if ((lcr & UART_LCR_WORD_LENGTH) == UART_LCR_WORD_LENGTH_6)
    printf("6\n");
  else if ((lcr & UART_LCR_WORD_LENGTH) == UART_LCR_WORD_LENGTH_7)
    printf("7\n");
  else
    printf("8\n");

  printf("Number of stop bits: ");

  if ((lcr & UART_LCR_STOP_BIT) == UART_LCR_STOP_BIT_1)
    printf("1\n");
  else
    printf("2\n");

  printf("Parity Control: ");

  if ((lcr & UART_LCR_PARITY_BIT_3) == 0x0) // if parity is "X X 0"
    printf("no parity\n");
  else if ((lcr & UART_LCR_PARITY) == UART_LCR_ODD_PARITY)
    printf("odd parity\n");
  else if ((lcr & UART_LCR_PARITY) == UART_LCR_EVEN_PARITY)
    printf("even parity\n");
  else if ((lcr & UART_LCR_PARITY) == UART_LCR_1_PARITY)
    printf("always 1\n");
  else
    printf("always 0\n");

  printf("DLAB: ");

  if ((lcr & UART_LCR_DLAB) == UART_LCR_DLAB_DATA)
    printf("0 (Acess Registers RBR e THR)\n");
  else
    printf("1 (Divisor Latch Access)\n");

  printf("\n\n");
}

void ser_show_ier(unsigned long ier)
{
  printf("IER REGISTER: 0x%x\n\n",ier);

  printf("Received Data Interrupts: ");

  if (ier & UART_IER_ENABLE_RD)
    printf("ON\n");
  else
    printf("OFF\n");

  printf("Received Transmitter Holding Register Empty Interrupt: ");

  if (ier & UART_IER_ENABLE_TE)
    printf("ON\n");
  else
    printf("OFF\n");

  printf("Receiver Line Status Interrupt: ");

  if (ier & UART_IER_ENABLE_RLS)
    printf("ON\n");
  else
    printf("OFF\n");
}

int ser_send_string_poll(unsigned short base_addr, char string[])
{
  unsigned int char_count=0;

  // continue sending chars till we find the null terminator
  while (string[char_count] != '\0')
  {
    ser_send_char_poll(base_addr,string[char_count]);
    printf("%c",string[char_count]);
    char_count++;
  }

  return 0;
}

void ser_send_char_poll(unsigned short base_addr,unsigned char char_send)
{
  unsigned long lsr;
  ser_get_reg(base_addr,UART_LSR,&lsr);

  while(!(lsr & UART_LSR_THR_EMPTY)) // repeat the cycle while THR is not empty
  {
    tickdelay(micros_to_ticks(DELAY_POLL)); // lets wait till we check again
    ser_get_reg(base_addr,UART_LSR,&lsr); // get the reg again and then analyze it
  }

  // now that it's empty, we can send the char
  ser_set_reg(base_addr,UART_THR, char_send);
}

int ser_receive_string_poll(unsigned short base_addr)
{
  int char_counter = 0;
  unsigned char current_char;

  do {

    if (ser_receive_char_poll (base_addr, &current_char))
      return 1;

    printf("%c",current_char);

  } while (current_char != '.');

  return 0;
}

int ser_receive_char_poll (unsigned short base_addr, unsigned char* char_receive) {

  unsigned long lsr;
  ser_get_reg(base_addr,UART_LSR,&lsr);

  if (lsr & UART_LSR_OVERRUN_ERROR)
  {
    printf("Overrun error in serial port data receiving\n");
    return 1;
  }
  else if (lsr & UART_LSR_PARITY_ERROR)
  {
    printf("Parity Error in serial port data receiving\n");
  }
  else if (lsr & UART_LSR_FRAMING_ERROR)
  {
    printf("Framing error while receiving string from serial port\n");
    return 1;
  }

  while(!(lsr & UART_LSR_RBR_READY)) // repeat the cycle while THR is not empty
  {
    tickdelay(micros_to_ticks(DELAY_POLL)); // lets wait till we check again
    ser_get_reg(base_addr,UART_LSR,&lsr); // get the reg again and then analyze it

    if (lsr & UART_LSR_OVERRUN_ERROR)
    {
      printf("Overrun error in serial port data receiving\n");
      return 1;
    }
    else if (lsr & UART_LSR_PARITY_ERROR)
    {
      printf("Parity Error in serial port data receiving\n");
    }
    else if (lsr & UART_LSR_FRAMING_ERROR)
    {
      printf("Framing error while receiving string from serial port\n");
      return 1;
    }
  }

  unsigned long RBR_content;
  ser_get_reg(base_addr,UART_RBR, &RBR_content);
  *char_receive = (unsigned char) RBR_content;

  return 0;
}

int ser_send_string_int(unsigned short base_addr, char string[])
{
  int ipc_status;
  message msg;

  ser_send_char_poll(base_addr,string[0]); // send first char by polling

  // turn on the interrupts (only empty transmiter int)
  unsigned long ier_config, ier_config_backup;
  ser_get_reg(base_addr,UART_IER,&ier_config);
  ier_config_backup = ier_config;
  ier_config &= UART_IER_ENABLE_TE;
  ser_set_reg(base_addr,UART_IER,ier_config);


  // subscribe the serial interrupts
  int irq_set = ser_subscribe_int(base_addr);

  unsigned int i=1;

  while( string[i] != '\0' ) // do while we dont find the dot (terminator)
  {
    if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("driver_receive failed\n");
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

          unsigned char ex;
          int status = ser_ih(base_addr,&ex);

          if (status == 1)
          {
            printf("An error occurred during the reception of the strings");
            return 1;
          }

          i++; // advance to the next char

        }
        break;
      default:
        break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }

  ser_unsubscribe_int();

  ser_set_reg(base_addr,UART_IER,ier_config_backup); // old ier values

  return 0;
}

int ser_receive_string_int(unsigned short base_addr)
{
  int ipc_status;
  message msg;
  unsigned char char_received;

  // turn on the interrupts (only received interrupts and error)
  unsigned long ier_config, ier_config_backup;
  ser_get_reg(base_addr,UART_IER,&ier_config);
  ier_config_backup = ier_config;
  ier_config &= (UART_IER_ENABLE_RD | UART_IER_ENABLE_RLS);
  ser_set_reg(base_addr,UART_IER,ier_config);

  // subscribe the serial interrupts
  int irq_set = ser_subscribe_int(base_addr);

  while( char_received != '.' ) // do while we dont find the dot (terminator)
  {
    if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("driver_receive failed\n");
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

          int status = ser_ih(base_addr,&char_received);

          if (status == 1)
          {
            printf("An error occurred during the reception of the strings");
            ser_set_reg(base_addr,UART_IER,ier_config_backup); // back to the old ier values
            return 1;
          }

          if (status == 0)
            printf("Char received: %c\n",char_received);
        }
        break;
      default:
        break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }

  ser_unsubscribe_int();

  ser_set_reg(base_addr,UART_IER,ier_config_backup); // old ier values

  return 0;
}

int ser_ih(unsigned short base_addr, unsigned char* char_send_receive)
{
  // get IIR and analyze it
  unsigned long IIR_content;
  ser_get_reg(base_addr,UART_IIR,&IIR_content);

  if(IIR_content & UART_INT_PEND) // int is pending?
  {
    switch(IIR_content & UART_INT_SEQUENCE) // check sequence
    {
    case UART_INT_TE: // transmiter empty
      printf("Transmiter Empty\n");
      ser_set_reg(base_addr,UART_THR,(unsigned long) *char_send_receive); // send char
      return 0;
      break;

    case UART_INT_RD: // received data available
      printf("Received Data Available\n");
      ser_get_reg(base_addr,UART_RBR,(unsigned long*) char_send_receive); // receive char
      return 0;
      break;

    case UART_INT_LS: // line status (error)
      printf("Error during reception occurred\n");
      return 1;
      break;

    default:
      printf("Non-relevant interruption occurred.\n"); return 2; break;
    }
  }

  printf("Not an int occurred!\n"); return 2;
}

int ser_subscribe_int(unsigned short base_addr)
{
  // set hook_id with the bit to put to one
  if (base_addr == SER_PORT_COM1)
    hook_id = SER_COM1_IRQ;
  else
    hook_id = SER_COM2_IRQ;

  // create a bit_mask with the int active
  int bit_mask = BIT(hook_id);

  // use both policies
  int policy = IRQ_REENABLE | IRQ_EXCLUSIVE;

  // hook_id will be changed with a different ID, but no problem
  // because we have the bit_mask already :-) Note: hook_id has the IRQ line
  if (sys_irqsetpolicy(hook_id,policy,&hook_id) != OK ||
      sys_irqenable(&hook_id) != OK)
    return -1;

  return bit_mask;
}

int ser_unsubscribe_int()
{
  if (sys_irqdisable(&hook_id) != OK || sys_irqrmpolicy(&hook_id) != OK)
    return 1;
  else
    return 0;
}
