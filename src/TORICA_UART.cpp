#include "TORICA_UART.h"
#include <Arduino.h>
#include <stdlib.h>

int TORICA_UART::readUART()
{
  int UART_data_len = 0;
  while (serial->available() > 0)
  {
    _buff[i_buff] = (char)serial->read();
    if (_buff[i_buff] == '\n')
    {
      _buff[i_buff] = '\0';

      char* token = strtok(_buff, ",");
      if (token == NULL)
      {
        i_buff = 0;
        continue; // データが空ならスキップして次の受信を待つ
      }
      UART_data[0] = strtof(token, NULL);

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


bool TORICA_UART::listenUART()
{
  bool has_signal = false;

  while (serial->available() > 0)
  {
    _buff[i_buff] = (char)serial->read();

    if (_buff[i_buff] == '\n')
    {
      _buff[i_buff] = '\0'; 

      // 1行完成すると公開用buffに内容をコピー
      memcpy(buff, _buff, i_buff + 1);
      i_buff = 0; // 次の行のためにリセット

      return true;
    }
    else
    {
      i_buff += 1;
    }
  }

  return has_signal;
}


int TORICA_UART::parseBuffer(const char* input_buf)
{
  if (input_buf == NULL || input_buf[0] == '\0') return 0;

  // 安全のため作業用バッファにコピー
  char work_buf[1024];
  strncpy(work_buf, input_buf, sizeof(work_buf));
  work_buf[sizeof(work_buf) - 1] = '\0';
  
  // 1個目の要素を取り出す
  char* token = strtok(work_buf, ",");
  if (token == NULL) return 0;
  
  UART_data[0] = strtof(token, NULL);
  
  int i_UART_data;
  for (i_UART_data = 1; true; i_UART_data++)
  {
    token = strtok(NULL, ",");
    if (token != NULL)
    {
      UART_data[i_UART_data] = strtof(token, NULL);
    }
    else
    {
      break;
    }
  }
  
  return i_UART_data; // 取り出せたデータ数を返す
}