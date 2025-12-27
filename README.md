# TORICA_Software_Library
ソフトウェアのライブラリ
## SerialWeb
- リポジトリを分離しました．（➔[SerialWeb](https://github.com/TORICA-Org/SerialWeb)）

### (以下，https://github.com/TORICA-23th/TORICA_libからの引用です)
 ```#include <TORICA_SD.h>```のようにインクルードしてライブラリが使えるようになります．
 ![image](https://user-images.githubusercontent.com/29813058/231634508-bd923de0-9e5b-4151-83de-004a0a8e2795.png)

## TORICA_SD
SDカードに文字列を書き込むためのライブラリです．
* `add_str`で文字列をmy_torica_sd内のバッファに保存し，`flash`でSDカードに書き込みます．
* `add_str`の文字列はchar配列でヌル終端されている必要があります．
* バッファを切り替えるため，`flash`中に割り込みなどで`add_str`を呼び出してもデータは失われずに次の`flash`で書き込まれます．バッファを指定する`SD_buf_index`はvolatile宣言されています．
```cpp
#include <TORICA_SD.h>

int cs_SD = 0;
TORICA_SD my_torica_sd(cs_SD);

char SD_BUF[256];

void setup()
{
  my_torica_sd.begin();
}

void loop()
{
  for (int i = 0; i < 100; i++)
  {
    sprintf(SD_BUF, "TEST,%d,%.2f,%.2f\n", millis(), 12.34, 334);
    // my_torica_sd内のバッファに保存
    my_torica_sd.add_str(SD_BUF);
  }
  // SDに書き込み
  my_torica_sd.flash();
}
```

## TORICA_UART
```,```区切りの少数が並んだ文字列を受信してパースするライブラリです．
桁は固定されておらず，1行ごとに改行コードLF(\n)をつけて送信されることを想定し，それをパースしてfloatの配列に配置します．

* 送信側のコードはこれでOKです．
  ```cpp
  void setup() {
    Serial1.begin(115200);
  }

  void loop() {
    Serial1.print("1.234,10,-3.34\n");
    delay(100);
  }
  ```
* この文字列をパースするのが```TORICA_UART```で，受信側のコードはこれです．
  ```cpp
  #include <TORICA_UART.h>

  // コンストラクタでシリアルポートのポインタを渡す
  TORICA_UART my_torica_uart(&Serial1);

  void setup() {
    SerialUSB.begin(115200);
    Serial1.begin(115200);
  }

  void loop() {
    // readUART()でUARTのバッファを読み取り、値の個数を返す。
    int readnum = my_torica_uart.readUART();
    for (int j = 0; j < readnum; j++) {
      // 取得した値は、配列UART_dataからアクセスする。
      SerialUSB.println(my_torica_uart.UART_data[j]);
    }
  }
  ```

## TORICA_ICS
近藤科学のサーボモータの通信規格ICSを読んで角度を取得するライブラリです．
