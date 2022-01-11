/***************************************************************************/
/* sourcefile for "hte501I2c.h" module */
/***************************************************************************/
/*
Read functions for measurement values of the HTE501 Sensor via I2C interface.

Copyright 2021 E+E Elektronik Ges.m.b.H.

Disclaimer:
This application example is non-binding and does not claim to be complete with regard
to configuration and equipment as well as all eventualities. The application example
is intended to provide assistance with the HTE501 sensor module design-in and is provided "as is".
You yourself are responsible for the proper operation of the products described.
This application example does not release you from the obligation to handle the product safely
during application, installation, operation and maintenance. By using this application example,
you acknowledge that we cannot be held liable for any damage beyond the liability regulations
described.

We reserve the right to make changes to this application example at any time without notice.
In case of discrepancies between the suggestions in this application example and other E+E
publications, such as catalogues, the content of the other documentation takes precedence.
We assume no liability for the information contained in this document.
*/

// Includes
//-----------------------------------------------------------------------------
#include "hte501I2c.h"
#include <Arduino.h>
#include "Wire.h"

hte501I2c::hte501I2c(void)
{
}

hte501I2c::hte501I2c(unsigned char i2cAdress)
{
  address = i2cAdress;
}

uint8_t hte501I2c::singleShotTempHum(float &temperature, float &humidity)
{
  unsigned char i2cResponse[6];
  unsigned char Command[] = {0x2C, 0x1B};
  wireWrite(Command, 1, false);
  wireRead(i2cResponse, 6);
  if (i2cResponse[2] == calcCrc8(i2cResponse, 0, 1) && i2cResponse[5] == calcCrc8(i2cResponse, 3, 4))
  {
    temperature = ((float)(i2cResponse[0]) * 256 + i2cResponse[1]) / 100;
    humidity = ((float)(i2cResponse[3]) * 256 + i2cResponse[4]) / 100;
    return 0;
  }
  else
  {
    return 1;
  }
}

uint8_t hte501I2c::getPeriodicMeasurmentTempHum(float &temperature, float &humidity)
{
  unsigned char i2cResponse[6];
  unsigned char Command[] = {0xE0, 0x00};
  wireWrite(Command, 1, false);
  wireRead(i2cResponse, 6);
  if (i2cResponse[2] == calcCrc8(i2cResponse, 0, 1) && i2cResponse[5] == calcCrc8(i2cResponse, 3, 4))
  {
    temperature = ((float)(i2cResponse[0]) * 256 + i2cResponse[1]) / 100;
    humidity = ((float)(i2cResponse[3]) * 256 + i2cResponse[4]) / 100;
    return 0;
  }
  else
  {
    return 1;
  }
}

uint8_t hte501I2c::getDewpoint(float &dewpoint)
{
  unsigned char i2cResponse[3];
  unsigned char Command[] = {0xE0, 0x16};
  wireWrite(Command, 1, false);
  wireRead(i2cResponse, 3);
  if (i2cResponse[2] == calcCrc8(i2cResponse, 0, 1))
  {
    dewpoint = ((float)(i2cResponse[0]) * 256 + i2cResponse[1]) / 100;
    return 0;
  }
  else
  {
    return 1;
  }
}

uint8_t hte501I2c::findSensor(void)
{
  int error;
  int i2cAddresseFound = 0;
  Wire.beginTransmission(address);
  error = Wire.endTransmission();
  if (error != 0)
  {
    for (int i = 64; i <= 71; i++)
    {
      Wire.beginTransmission(i);
      error = Wire.endTransmission();
      if (error == 0)
      {
        i2cAddresseFound++;
        return 3;
      }
    }
    if (i2cAddresseFound == 0)
    {
      return 4;
    }
  }
  else
  {
    return 2;
  }
}

uint8_t hte501I2c::changePeriodicMeasurmentTime(uint32_t millisec)
{
  unsigned char sendBytes[2];
  if (3276751 > millisec)
  {
    int value = millisec / 50;
    sendBytes[1] = value / 255;
    sendBytes[0] = value % 256;
    unsigned char crc8[] = {0x10, sendBytes[0], sendBytes[1]};
    unsigned char Command[] = {0x72, 0xA7, 0x10, sendBytes[0], sendBytes[1], calcCrc8(crc8, 0, 2)};
    wireWrite(Command, 5, true);
    return 0;
  }
  else
  {
    return 5;
  }
}

void hte501I2c::readPeriodicMeasurmentTime(float &periodicMeasurmentTime)
{
  unsigned char i2cResponse[3];
  unsigned char Command[] = {0x72, 0xA7, 0x10};
  wireWrite(Command, 2, false);
  wireRead(i2cResponse, 3);
  float value = i2cResponse[1] * 256 + i2cResponse[0];
  periodicMeasurmentTime = value * 0.05;
}

uint8_t hte501I2c::changeHeaterCurrent(int mA) //5mA - 80mA
{
  unsigned char sendByte = 0x00;
  if (5 <= mA <= 80)
  {
    int value = (mA / 5) - 1;
    sendByte = value;
    sendByte = (sendByte << 3) + 7; // +7 because of Heater off
    unsigned char crc8[] = {0x08, sendByte};
    unsigned char Command[] = {0x72, 0xA7, 0x08, sendByte, calcCrc8(crc8, 0, 1)};
    wireWrite(Command, 4, true);
    return 0;
  }
  else
  {
    return 6;
  }
}

void hte501I2c::readHeaterCurrent(int &heaterCurrent)
{
  unsigned char i2cResponse[2];
  unsigned char Command[] = {0x72, 0xA7, 0x08};
  wireWrite(Command, 2, false);
  wireRead(i2cResponse, 1);
  i2cResponse[0] = i2cResponse[0] << 1;
  i2cResponse[0] = i2cResponse[0] >> 4;
  heaterCurrent = (i2cResponse[0] + 1) * 5;
}

uint8_t hte501I2c::changeMeasurmentResolution(int measResTemp, int measResHum) //8 - 13 Bit
{

  if (8 <= measResTemp <= 13 && 8 <= measResHum <= 14)
  {
    unsigned char sendByte = ((measResTemp - 8) << 3) + (measResHum - 8);
    unsigned char crc8[] = {0x0F, sendByte};
    unsigned char Command[] = {0x72, 0xA7, 0x0F, sendByte, calcCrc8(crc8, 0, 1)};
    wireWrite(Command, 4, true);
    return 0;
  }
  else
  {
    return 7;
  }
}

void hte501I2c::readMeasurmentResolution(int &measResTemp, int &measResHum)
{
  unsigned char i2cResponse[2];
  unsigned char Command[] = {0x72, 0xA7, 0x0F};
  wireWrite(Command, 2, false);
  wireRead(i2cResponse, 1);
  i2cResponse[1] = i2cResponse[0];
  i2cResponse[0] = i2cResponse[0] << 2;
  i2cResponse[0] = i2cResponse[0] >> 5;
  measResHum = i2cResponse[0] + 8;
  i2cResponse[1] = i2cResponse[1] << 5;
  i2cResponse[1] = i2cResponse[1] >> 5;
  measResTemp = i2cResponse[1] + 8;
}

void hte501I2c::startPeriodicMeasurment(void)
{
  unsigned char Command[] = {0x20, 0x1E};
  wireWrite(Command, 1, true);
}

void hte501I2c::endPeriodicMeasurment(void)
{
  unsigned char Command[] = {0x30, 0x93};
  wireWrite(Command, 1, true);
}

void hte501I2c::heaterOn(void)
{
  unsigned char Command[] = {0x30, 0x6D};
  wireWrite(Command, 1, true);
}

void hte501I2c::heaterOff(void)
{
  unsigned char Command[] = {0x30, 0x66};
  wireWrite(Command, 1, true);
}

uint8_t hte501I2c::readIdentification(unsigned char identification[])
{
  unsigned char i2cResponse[9];
  unsigned char Command[] = {0x70, 0x29};
  wireWrite(Command, 1, false);
  wireRead(i2cResponse, 9);
  if (i2cResponse[8] == calcCrc8(i2cResponse, 0, 7))
  {
    for (int i = 0; i < 8; i++)
    {
      identification[i] = i2cResponse[i];
    }
    return 0;
  }
  else
  {
    return 1;
  }
}

void hte501I2c::reset(void)
{
  unsigned char Command[] = {0x30, 0xA2};
  wireWrite(Command, 1, true);
}

uint8_t hte501I2c::newMeasurmentReady(bool &measurement)
{
  unsigned char i2cResponse[3];
  unsigned char Command[] = {0xF3, 0x52};
  wireWrite(Command, 1, false);
  wireRead(i2cResponse, 3);
  if (i2cResponse[2] == calcCrc8(i2cResponse, 0, 1))
  {
    measurement = i2cResponse[0] >> 7;
    return 0;
  }
  else
  {
    return 1;
  }
}

uint8_t hte501I2c::constantHeaterOnOff(bool &conHeaterOnOff)
{
  unsigned char i2cResponse[3];
  unsigned char Command[] = {0xF3, 0x2D};
  wireWrite(Command, 1, false);
  wireRead(i2cResponse, 3);
  if (i2cResponse[2] == calcCrc8(i2cResponse, 0, 1))
  {
    i2cResponse[0] = i2cResponse[0] << 2;
    conHeaterOnOff = i2cResponse[0] >> 7;
    return 0;
  }
  else
  {
    return 1;
  }
}

void hte501I2c::clearStatusregister1(void)
{
  unsigned char Command[] = {0x30, 0x41};
  wireWrite(Command, 1, true);
}

void hte501I2c::wireWrite(unsigned char buf[], int to, bool stopmessage)
{
  Wire.beginTransmission(address);
  for (int i = 0; i <= to; i++)
  {
    Wire.write(buf[i]);
  }
  Wire.endTransmission(stopmessage);
}

void hte501I2c::wireRead(unsigned char buf[], uint8_t to)
{
  int i = 0;
  Wire.requestFrom(address, to);
  while (Wire.available())
  {
    buf[i++] = Wire.read();
  }
}

unsigned char hte501I2c::calcCrc8(unsigned char buf[], unsigned char from, unsigned char to)
{
  unsigned char crcVal = CRC8_ONEWIRE_START;
  unsigned char i = 0;
  unsigned char j = 0;
  for (i = from; i <= to; i++)
  {
    int curVal = buf[i];
    for (j = 0; j < 8; j++)
    {
      if (((crcVal ^ curVal) & 0x80) != 0) //If MSBs are not equal
      {
        crcVal = ((crcVal << 1) ^ CRC8_ONEWIRE_POLY);
      }
      else
      {
        crcVal = (crcVal << 1);
      }
      curVal = curVal << 1;
    }
  }
  return crcVal;
}

void hte501I2c::getErrorString(uint8_t Status, char errorString[])
{
  memset(errorString, '\0', sizeof(errorString));
  switch (Status)
  {
  case OKAY:
    strcpy(errorString, "Success");
    break;
  case ERR_CKSUM:
    strcpy(errorString, "Checksum error");
    break;
  case CON_OK:
    strcpy(errorString, "A connection has been established with the sensor at the specified i2c address.");
    break;
  case NO_SENSOR_ON_ADDR:
    strcpy(errorString, "Found no sensor on your specified i2c address but on another that could be possible your sensor. Please check that the connection is error-free and that the correct i2c address is specified in the code.");
    break;
  case NO_SENSOR:
    strcpy(errorString, "No sensor was found in the area where the HTE501 should have its i2c address, please check if you connected the sensor correctly!");
    break;
  case SEC_TO_HIGH:
    strcpy(errorString, "The Milliseconds are not in the specification");
    break;
  case CUR_NOT_IN_SPEC:
    strcpy(errorString, "The milli Ampere are not in the specification 5-80 mA");
    break;
  case MEAS_RES_WRONG:
    strcpy(errorString, "The Measurment Resolution are not in the specification 8-13 Bit");
    break;
  default:
    strcpy(errorString, "unknown error");
    break;
  }
}