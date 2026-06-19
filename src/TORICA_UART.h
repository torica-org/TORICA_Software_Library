#pragma once // インクルードガード

#include <Arduino.h>

class TORICA_UART {
  public:
    TORICA_UART(Stream *_serial) {
      serial = _serial;
    }

    int readUART();
    bool listenUART();
    int parseBuffer(const char* input_buf);
    float UART_data[16];
    char buff[8196]; // 公開用バッファ

  private:
    int i_buff = 0;
    char _buff[8196]; // 内部用バッファ
    char *p;
    Stream *serial;
};
