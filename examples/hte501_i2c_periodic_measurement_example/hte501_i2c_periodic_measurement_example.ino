/*
Example script reading measurement values from the HTE501 sensor via I2C interface.

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

#include <hte501I2c.h>
#include <Wire.h>
hte501I2c hte(0x40);
#define REQUEST_INTERVAL_MS 100 //Time interval between measurements in ms
#define CSV_DELIMITER ','

bool measurmentReady;
float temperature, humidity, dewpoint, measurmentTime;
unsigned char identification[8];
char errorString[200];
uint8_t errorcode;

void setup()
{
  Serial.begin(9600); //Start Serial communication
  Wire.begin();       //initialize I2C peripheral (SDA..A4, SCL..A5)
  delay(1000);
  hte.getErrorString(hte.findSensor(), errorString);
  Serial.println(errorString);
  errorcode = hte.changePeriodicMeasurmentTime(5000); // in ms
  if (errorcode != 0)
  {
    hte.getErrorString(errorcode, errorString);
    Serial.println(errorString);
  }
  else
  {
    Serial.println("Changing the periodic measurement time was successful");
  }
  hte.readPeriodicMeasurmentTime(measurmentTime);
  Serial.print("periodic measurment time is: ");
  Serial.print(measurmentTime);
  Serial.println(" s");
  errorcode = hte.readIdentification(identification);
  if (errorcode != 0)
  {
    hte.getErrorString(errorcode, errorString);
    Serial.println(errorString);
  }
  else
  {
    Serial.print("Identificationnumber: ");
    for (int i = 0; i < 8; i++)
    {
      Serial.print(identification[i], HEX);
    }
  }
  Serial.println("");
  hte.startPeriodicMeasurment();
  delay(2000);
  Serial.print("temperature");
  Serial.print(CSV_DELIMITER);
  Serial.print(" relative humidity");
  Serial.print(CSV_DELIMITER);
  Serial.println(" dewpoint");
  delay(1000);
}

void loop()
{
  errorcode = hte.newMeasurmentReady(measurmentReady);
  if (errorcode != 0)
  {
  }
  else
  {
    if (measurmentReady)
    {
      errorcode = hte.getPeriodicMeasurmentTempHum(temperature, humidity);
      if (errorcode != 0)
      {
        hte.getErrorString(errorcode, errorString);
        Serial.println(errorString);
      }
      else
      {
        Serial.print(temperature);
        Serial.print(" °C");
        Serial.print(CSV_DELIMITER);
        Serial.print(humidity);
        Serial.print(" %RH");
        Serial.print(CSV_DELIMITER);
      }
      errorcode = hte.getDewpoint(dewpoint);
      if (errorcode != 0)
      {
        hte.getErrorString(errorcode, errorString);
        Serial.println(errorString);
      }
      else
      {
        Serial.print(dewpoint);
        Serial.println(" °C");
      }
    }
  }
  delay(REQUEST_INTERVAL_MS); //time until the next loop start
}
