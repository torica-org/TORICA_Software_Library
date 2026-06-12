#include "TORICA_UART.h"
#include <Arduino.h>
#include <stdlib.h>

int TORICA_UART::readUART()
{
  int UART_data_len = 0;
  while (serial->available() > 0)
  {
    buff[i_buff] = (char)serial->read();
    if (buff[i_buff] == '\n')
    {
      buff[i_buff] = '\0';
      UART_data[0] = strtof(strtok(buff, ","), NULL);
      int i_UART_data;
      for (i_UART_data = 1; true; i_UART_data++)
      {
        p = strtok(NULL, ",");
        if (p != NULL)
        {
          UART_data[i_UART_data] = strtof(p, NULL);
        }
        else
        {
          break;
        }
      }
      i_buff = 0;
      UART_data_len = i_UART_data;
    }
    else
    {
      i_buff += 1;
    }
  }
  return UART_data_len;
}
