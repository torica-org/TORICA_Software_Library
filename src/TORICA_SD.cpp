#include "TORICA_SD.h"
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

TORICA_SD::TORICA_SD(bool _retry) // for CORE
{
  retry = _retry;
  MODE = CORE;
}

TORICA_SD::TORICA_SD(int _cs_SD, bool _retry) // for LEGACY
{
  cs_SD = _cs_SD;
  retry = _retry;
}

bool TORICA_SD::begin() // for LEGACY
{
  if(MODE != LEGACY) return false;

  SERIAL_USB.print("Initializing SD card...");
#if defined(SEEED_XIAO_RP2040)
  SPI.setRX(SD_SPI_MISO);
  SPI.setCS(SD_SPI_CSn);
  SPI.setSCK(SD_SPI_SCK);
  SPI.setTX(SD_SPI_MOSI);
  if (!SD.begin(cs_SD, SPI))
#elif defined(RASPBERRY_PI_PICO)
  SPI.setRX(SD_SPI_MISO);
  SPI.setCS(SD_SPI_CSn);
  SPI.setSCK(SD_SPI_SCK);
  SPI.setTX(SD_SPI_MOSI);
  if (!SD.begin(cs_SD, SPI))
#else
  // 上記2つのマイコン以外はこのプリプロセッサが実行される
  // SPIのRX(MISO),CS(CSn),SCK,TX(MOSI)はすべて設定する必要がある
  if (!SD.begin(cs_SD))
#endif
  {
    SERIAL_USB.println("Card failed, or not present");
    SDisActive = false;
    return false;
  }
  new_file();
  dataFile = SD.open(fileName, FILE_WRITE);

  SERIAL_USB.println("card initialized.");

  SDisActive = true;
  return true;
}

bool TORICA_SD::begin(int _cs_SD) // for CORE
{
  if(MODE != CORE) return false;
  
  // SPIのRX(MISO),CS(CSn),SCK,TX(MOSI)はすべて設定する必要がある
  if (!SD.begin(cs_SD))
  {
    SERIAL_USB.println("Card failed, or not present");
    SDisActive = false;
    return false;
  }
  new_file();
  dataFile = SD.open(fileName, FILE_WRITE);

  SERIAL_USB.println("card initialized.");

  SDisActive = true;
  return true;
}

void TORICA_SD::add_str(char str[])
{
  if (SDisActive)
  {
    dataFile.write((const uint8_t*)str, strlen(str)); // バッファに書き込み
  }
}

void TORICA_SD::new_file() // ファイル名の連番`LOG00-00.CSV`を生成
{
  if (subNum == 0) // 電源投入時
  {
    while (1)
    {
      sprintf(fileName, "/LOG%02d-00.CSV", mainNum);
      if (!(SD.exists(fileName)))
      {
        subNum++;
        break;
      }
      mainNum++;
    }
  } 
  else // ファイル区切り
  {
    sprintf(fileName, "/LOG%02d-%02d.CSV", mainNum, subNum);
    subNum++;
  }
  file_time = millis();
}

void TORICA_SD::flash() // ファイルサイズを監視しつつ，SDに書き込み
{
  uint32_t SD_time = millis();

  if (retry && (!dataFile || !SDisActive))
  {
    SERIAL_USB.println("error opening file");
    end();
    SDisActive = begin();
  }

  if (SDisActive)
  {
    if (millis() - file_time > 10 * 60 * 1000) // 10分ごとにファイルを区切る
    {
      dataFile.close();
      new_file();
      dataFile = SD.open(fileName, FILE_WRITE);
    }

    if (dataFile.size() >= TORICA_SD_MAX_FILE_SIZE) // ファイルサイズでファイルを区切る
    {
      SERIAL_USB.println("TORICA_SD_MAX_FILE_SIZE");
      dataFile.close();
      new_file();
      dataFile = SD.open(fileName, FILE_WRITE);
    }

    if (dataFile)
    {
      dataFile.flush(); // SDに書き込み
      // SERIAL_USB.println("SD_buf_count,SD_total");
      // SERIAL_USB.print(",");
      uint32_t SD_total = millis() - SD_time;
      // SERIAL_USB.println(SD_total);
    }
  }
}

void TORICA_SD::end()
{
  SDisActive = false;
  SD.end();
}
