/*
Example script reading measurement values from the HTE501 sensor via I2C interface.

Copyright 2022 E+E Elektronik Ges.m.b.H.

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
#define REQUEST_INTERVAL_MS 2000 //Time interval between measurements in ms
#define CSV_DELIMITER ','

float temperature = 0, humidity = 0, dewpoint = 0;
unsigned char identification[8];
char errorString[200];
uint8_t errorcode;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  hte.getErrorString(hte.findSensor(), errorString);
  Serial.println(errorString);
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
      Serial.print(identification[i] < 16 ? "0" : "");
      Serial.print(identification[i], HEX);
    }
  }
  Serial.println("");
  Serial.print("temperature");
  Serial.print(CSV_DELIMITER);
  Serial.print(" relative humidity");
  Serial.print(CSV_DELIMITER);
  Serial.println(" dewpoint");
  delay(1000);
}

void loop()
{
  errorcode = hte.singleShotTempHum(temperature, humidity);
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
    Serial.print(" °C");
    Serial.println(CSV_DELIMITER);
  }
  delay(REQUEST_INTERVAL_MS);
}
