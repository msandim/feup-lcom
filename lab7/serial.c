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
  unsigned long dll = div & (0x00FF);
  unsigned long dlm = (div >> 8);

  if (ser_set_reg(base_addr,UART_DLL,dll) || ser_set_reg(base_addr,UART_DLM,dlm))
    return 1;

  // disable DLAB
  lcr ^= UART_LCR_DLAB;

  if (ser_set_reg(base_addr,UART_LCR,lcr))
    return 1;

  return 0;
}

int ser_set_config(unsigned short base_addr, unsigned long bits, unsigned long stop,
    long parity, /* -1: none, 0: even, 1: odd */unsigned long rate)
{
  unsigned long lcr_config;

  if(ser_get_reg(base_addr,UART_LCR,&lcr_config))
  {
    printf("ser_test_set: Error reading LCR\n\n");
    return 1;
  }

  unsigned long new_lcr = 0;

  new_lcr |= (lcr_config & UART_LCR_SET_BREAK);
  new_lcr |= (lcr_config & UART_LCR_DLAB);

  //BITS
  if (bits == 5) {
    new_lcr |= UART_LCR_WORD_LENGTH_5;
  } else if (bits == 6) {
    new_lcr |= UART_LCR_WORD_LENGTH_6;
  } else if (bits == 7) {
    new_lcr |= UART_LCR_WORD_LENGTH_7;
  } else if (bits == 8) {
    new_lcr |= UART_LCR_WORD_LENGTH_8;
  } else {
    printf ("ser_test_set: Invalid number of bits %x\n", bits);
    return 1;
  }

  //STOP
  if (stop == 1) {
    new_lcr |= UART_LCR_STOP_BIT_1;
  } else if (stop == 2) {
    new_lcr |= UART_LCR_STOP_BIT_2;
  } else {
    printf ("ser_test_set: Invalid number of stop bits %x\n", stop);
    return 1;
  }

  //PARITY
  if (parity == -1) {
    new_lcr |= UART_LCR_NO_PARITY;
  } else if (parity == 0) {
    new_lcr |= UART_LCR_EVEN_PARITY;
  } else if (parity == 1) {
    new_lcr |= UART_LCR_ODD_PARITY;
  } else {
    printf ("ser_test_set: Invalid parity %x\n", parity);
    return 1;
  }

  // save new lcr
  if (ser_set_reg(base_addr,UART_LCR,new_lcr))
    return 1;

  if (ser_set_bit_rate(base_addr, rate))
    return 1;

  //printf("BASE_ADDR: %x, BITS: %x,STOP: %x,PARITY: %d, RATE: %u",base_addr,bits,stop,parity,rate);

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

int ser_send_string_poll(unsigned short base_addr, unsigned char string[])
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

int ser_receive_char_poll(unsigned short base_addr, unsigned char* char_receive)
{
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
    return 1;
  }
  else if (lsr & UART_LSR_FRAMING_ERROR)
  {
    printf("Framing error while receiving string from serial port\n");
    return 1;
  }

  while(!(lsr & UART_LSR_RBR_READY)) // repeat the cycle while THR is not empty (is not ready)
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
      return 1;
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

int ser_send_string_int(unsigned short base_addr, unsigned char string[])
{
  int ipc_status;
  message msg;

  // turn on the interrupts (only empty transmiter int)
  unsigned long ier_config, ier_config_backup;
  ser_get_reg(base_addr,UART_IER,&ier_config);
  ier_config_backup = ier_config;
  ier_config = 0; // reset all interrupts
  ier_config |= UART_IER_ENABLE_TE;
  ser_set_reg(base_addr,UART_IER,ier_config);

  // teste
  //printf("IER_CONFIG: 0x%X IER_ANTIGO: 0x%X",ier_config,ier_config_backup);

  // subscribe the serial interrupts
  int irq_set = ser_subscribe_int(base_addr);

  ser_send_char_poll(base_addr,string[0]); // send first char by polling
  printf("%c",string[0]);

  unsigned int i=1;

  while( string[i] != '\0' ) // do while we dont find the null terminator
  {
    //printf("driver receive ups");
    if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("driver_receive failed\n");
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

          ser_ih(base_addr,&string[i],0,0);

          printf("%c",string[i]);

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
  ier_config = 0; // reset interrupts
  ier_config |= UART_IER_ENABLE_RD;
  ier_config |= UART_IER_ENABLE_RLS;
  ser_set_reg(base_addr,UART_IER,ier_config);

  // teste
  //printf("IER_CONFIG: 0x%X IER_ANTIGO: 0x%X",ier_config,ier_config_backup);

  // subscribe the serial interrupts
  int irq_set = ser_subscribe_int(base_addr);

  while(char_received != '.') // do while we dont find the dot (terminator)
  {
    unsigned long IIR_content;
    ser_get_reg(base_addr,UART_IIR,&IIR_content);

    //printf("IIR_CONTENT: 0x%X",IIR_content);
    //printf("driver receive ups\n");
    if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("driver_receive failed\n");
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

          int status = ser_ih(base_addr,&char_received,0,0);

          if (status == 1)
          {
            printf("An error occurred during the reception of the strings");
            ser_unsubscribe_int();
            ser_set_reg(base_addr,UART_IER,ier_config_backup); // back to the old ier values
            return 1;
          }

          if (status == 0)
            printf("%c",char_received);

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


int ser_send_string_int_fifo(unsigned short base_addr,unsigned char string[])
{
  int ipc_status;
  message msg;

  // turn on fifo & clear transmit
  unsigned long fcr_config=0;
  fcr_config |= (UART_ENABLE_FIFO | UART_CLEAR_TFIFO);
  ser_set_reg(base_addr,UART_FCR,fcr_config);

  // turn on the interrupts (only empty transmiter int)
  unsigned long ier_config, ier_config_backup;
  ser_get_reg(base_addr,UART_IER,&ier_config);
  ier_config_backup = ier_config;
  ier_config = 0; // reset all interrupts
  ier_config |= UART_IER_ENABLE_TE;
  ser_set_reg(base_addr,UART_IER,ier_config);

  // create array of chars to transmit
  unsigned char queue[16];
  int queue_counter = 0; // array index (number of elements on the array)

  // teste
  printf("IER_CONFIG: 0x%X IER_ANTIGO: 0x%X",ier_config,ier_config_backup);

  // subscribe the serial interrupts
  int irq_set = ser_subscribe_int(base_addr);

  ser_send_char_poll(base_addr,string[0]); // send first char by polling
  printf("%c",string[0]);

  unsigned int i=1;

  while( string[i] != '\0' ) // do while we dont find the null terminator
  {
    //printf("driver receive ups");
    if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("driver_receive failed\n");
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.NOTIFY_ARG & irq_set) {

          while(queue_counter < 16 && string[i] != '\0')
          {
            queue[queue_counter] = string[i];
            printf("%c",string[i]);
            i++; // advance to next char
            queue_counter++; // advance to next queue position
          }

          ser_ih(base_addr,queue,1,queue_counter);

          queue_counter = 0; // initialize queue size

        }
        break;
      default:
        break;
      }
    } else {

    }
  }

  ser_unsubscribe_int();

  // desactivate fifo and clear transmit
  fcr_config=0;
  fcr_config |= UART_CLEAR_TFIFO;
  ser_set_reg(base_addr,UART_FCR,fcr_config);

  ser_set_reg(base_addr,UART_IER,ier_config_backup); // old ier values

  return 0;
}

int ser_receive_string_int_fifo(unsigned short base_addr,unsigned long trigger)
{
  int ipc_status;
  message msg;

  // turn on fifo & clear transmit
  unsigned long fcr_config=0;
  fcr_config |= UART_ENABLE_FIFO;
  fcr_config |= UART_CLEAR_RFIFO;
  fcr_config |= 0x80;
  ser_set_reg(base_addr,UART_FCR,fcr_config);

  // turn on the interrupts (error interrupts and receive data interrupts)
  unsigned long ier_config, ier_config_backup;
  ser_get_reg(base_addr,UART_IER,&ier_config);
  ier_config_backup = ier_config;
  ier_config = 0; // reset all interrupts
  ier_config |= UART_IER_ENABLE_RD;
  ier_config |= UART_IER_ENABLE_RLS;
  ser_set_reg(base_addr,UART_IER,ier_config);

  // create array of chars to transmit
  unsigned char queue[16];
  int queue_counter = 0; // array index (number of elements on the array)

  int terminator_not_found = 1;

  // teste
  //printf("IER_CONFIG: 0x%X IER_ANTIGO: 0x%X",ier_config,ier_config_backup);

  // subscribe the serial interrupts
  int irq_set = ser_subscribe_int(base_addr);

  while( terminator_not_found ) // do while we dont find the null terminator
  {
    //printf("driver receive ups");
    if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("driver_receive failed\n");
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.NOTIFY_ARG & irq_set) {

          int status = ser_ih(base_addr,queue,1,trigger);

          if (status == 0 || status == 2) // its all ok OR timeout and lets read!
          {
            unsigned long count=0;
            for (count; count < trigger; count++)
            {
              // do something with the chars
              printf("%c",queue[count]);

              if (queue[count] == '.')
              {
                terminator_not_found = 0; // terminate receiver
                break;
              }
            }

          }

          else if (status == 1) // -> error
          {
            printf("Error receiving char in receive_string_int_fifo()\n");
            ser_unsubscribe_int();

            // desactivate fifo and clear transmit
            fcr_config=0;
            fcr_config |= UART_CLEAR_RFIFO;
            ser_set_reg(base_addr,UART_FCR,fcr_config);

            ser_set_reg(base_addr,UART_IER,ier_config_backup); // old ier values
            return 1;
          }

        }
        break;
      default:
        break;
      }
    } else {

    }
  }

  ser_unsubscribe_int();

  // desactivate fifo and clear transmit
  fcr_config=0;
  fcr_config |= UART_CLEAR_RFIFO;
  ser_set_reg(base_addr,UART_FCR,fcr_config);

  ser_set_reg(base_addr,UART_IER,ier_config_backup); // old ier values

  return 0;
}

int ser_init_fifo_poll(unsigned short base_addr)
{
  // turn on fifo & clear transmit
  unsigned long fcr_config=0;
  fcr_config |= UART_ENABLE_FIFO;
  fcr_config |= UART_FCR_CLEAR_BUFFERS;

  return ser_set_reg(base_addr,UART_FCR,fcr_config);
}

int ser_shut_fifo_poll(unsigned short base_addr)
{
  // desactivate fifo and clear transmit
  unsigned long fcr_config=0;
  fcr_config |= UART_FCR_CLEAR_BUFFERS;

  return ser_set_reg(base_addr,UART_FCR,fcr_config);
}

int ser_send_string_poll_fifo(unsigned short base_addr, unsigned char string[])
{
  int scount = 0, fifo_counter = 0, end_string = 0;

  do
  {
    // get lsr
    unsigned long lsr;
    ser_get_reg(base_addr,UART_LSR,&lsr);

    if (lsr & UART_LSR_THR_EMPTY) // if THR empty, lets send 16 bytes, because we are sure that the FIFO is empty
    {
      while (fifo_counter < 16)
      {
        ser_set_reg(base_addr,UART_THR,(unsigned long) string[scount]); // send char
        printf("%c",string[scount]);

        if (string[scount] == '\0') // if the '\0' was sent, end everything
        {
          end_string = 1;
          break;
        }

        fifo_counter++;
        scount++;
      }

      fifo_counter = 0;

    }

    else
    {
      printf("Nao esta vazio\n");
      tickdelay(micros_to_ticks(50000));
    }

  } while (!end_string);

  return 0;
}

int ser_receive_string_poll_fifo(unsigned short base_addr, unsigned char string[])
{
  int scount = 0, fifo_counter = 0, end_string = 0, started_rx = 0, nothing_rx = 0;

  unsigned long rx_content = '1';

  while(rx_content != '\0')
  {
    unsigned long lsr;
    ser_get_reg(base_addr,UART_LSR,&lsr);

    // if there is any error
    if (lsr & UART_LSR_OVERRUN_ERROR)
    {
      printf("Overrun error in serial port data receiving\n");
      return 1;
    }
    else if (lsr & UART_LSR_PARITY_ERROR)
    {
      printf("Parity Error in serial port data receiving\n");
      return 1;
    }
    else if (lsr & UART_LSR_FRAMING_ERROR)
    {
      printf("Framing error while receiving string from serial port\n");
      return 1;
    }

    else if (lsr & UART_LSR_RBR_READY) // if there is something to read
    {
      ser_get_reg(base_addr,UART_RBR, &rx_content);
      //printf("%c",rx_content);

      if (!started_rx) // mark that we started receiving the string
        started_rx = 1;

      string[scount] = (unsigned char) rx_content;
      scount++;
    }

    else
    {
      if (started_rx) // if we started receiving, wait for more
      {
        printf("esperar pelo q ja comecei a receber\n");
        tickdelay(micros_to_ticks(50000));
      }

      else return 1; // se nao ha nada para receber, saio logo
    }
  }

  return 0;

}


int ser_ih(unsigned short base_addr, unsigned char* char_send_receive, int fifo, int size_fifo)
{
  // get IIR and analyze it
  unsigned long IIR_content;
  ser_get_reg(base_addr,UART_IIR,&IIR_content);

  //printf("IIR_CONTENT_IH: 0x%X\n",IIR_content);

  if(!(IIR_content & UART_INT_PEND)) // int is pending?
  {
    switch(IIR_content & UART_INT_SEQUENCE) // check sequence
    {
    case UART_INT_TE: // transmitter empty
      //printf("Transmiter Empty\n");
      if (fifo)
      {
        while(size_fifo--)
        {
          ser_set_reg(base_addr,UART_THR,(unsigned long) *char_send_receive); // send char
          char_send_receive++;
        }
      }
      else
        ser_set_reg(base_addr,UART_THR,(unsigned long) *char_send_receive); // send char
      return 0;
      break;

    case UART_INT_RD: // received data available
    {
      //printf("Received Data Available\n");
      if (fifo)
      {
        int counter_till_trigger = 0;
        while(counter_till_trigger < size_fifo)
        {
          ser_get_reg(base_addr,UART_RBR,(unsigned long*) char_send_receive); // receive char
          //printf("RECEBI %c\n",*char_send_receive);
          counter_till_trigger++;
          char_send_receive++;
        }
      }
      else
        ser_get_reg(base_addr,UART_RBR,(unsigned long*) char_send_receive); // receive char
      return 0;
      break;
    }

    case UART_INT_LS: // line status (error)
      //printf("Error during reception occurred\n");
      return 1;
      break;

    case UART_INT_TIMEOUT: // timeout (trigger)
    {
      //printf("Timeout interrupt\n");
      unsigned long lsr_contents; // To see the receiver data flag and transfer while we can to the fifo
      ser_get_reg(base_addr,UART_LSR,&lsr_contents);

      while(lsr_contents & UART_LSR_RBR_READY) // receive while we have chars to receive
      {
        ser_get_reg(base_addr,UART_RBR,(unsigned long*) char_send_receive); // receive char
        ser_get_reg(base_addr,UART_LSR,&lsr_contents); // get reg to analyze again
        char_send_receive++;
      }
      return 2;
      break;
    }

    default:
      printf("Non-relevant interrupt occurred.\n"); return 3; break;
    }
  }

  printf("An interrupt outside the UART occured.\n"); return 3;
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
