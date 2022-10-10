/*
Example script to check if the sensor HTE501 is not damaged.

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

/*
 !!!!!!!!!!!!!!!!!!!Attention!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
This programme should only be used if the conditions remain the same, otherwise incorrect results will be obtained.
*/


#include <hte501I2c.h>
#include <Wire.h>
hte501I2c hte(0x40);
#define REQUEST_INTERVAL_MS       2000           //Time interval between measurements in ms
#define CSV_DELIMITER             ','



unsigned char identification[8];
char errorString[200];
uint8_t errorcode; 
float RH_Heater_OFF = 0, T_Heater_OFF = 0, Td_Heater_OFF, RH_Heater_ON = 0 , T_Heater_ON = 0, Td_Heater_ON = 0, Td_Heater_ON_Old = 0, delta;
bool loopEnd;

float variance_calculation(float T_CC, float RH_CC)
{
  float result;
  float deltatemp , deltahumi; 
  if(T_CC >= 15 && T_CC <= 80) //Calculate the error limits
  {
    deltatemp = 0.3;  
  }
  if(T_CC < 15)
  {
    deltatemp = (0.2/55)*(15 - T_CC) + 0.3; 
  }
  if(T_CC > 85)
  {
    deltatemp = (0.2/55)*(T_CC - 85) + 0.3; 
  }

  deltahumi = (0.5/100 * RH_CC) + 2.5;

  double derivativeTemp = (pow(243.12,2)*pow(17.62,2))/(pow((log(RH_CC/100)*T_CC-243.12*17.62+log(RH_CC/100)*243.12),2));   //Calculate the error propagation
  double derivativeHum = (243.12*17.62*pow((243.12+T_CC),2))/(pow(((T_CC+243.12)*log(RH_CC/100)-243.12*17.62),2)*RH_CC);
  result = derivativeTemp * deltatemp + derivativeHum * deltahumi;
  return result;
}


void setup() 
{
  Serial.begin(9600);
  Wire.begin();
  hte.getErrorString(hte.findSensor(), errorString);
  Serial.println(errorString);
  errorcode = hte.readIdentification(identification);
  if(errorcode != 0)
  {
    hte.getErrorString(errorcode,errorString);
    Serial.println(errorString); 
  }
  else 
  {
    Serial.print("Identificationnumber: ");
    for(int i = 0; i < 8; i++)
    {
      Serial.print(identification[i] < 16 ? "0" : "");
      Serial.print(identification[i],HEX);
    }
  }
  Serial.println("");
}

void loop() 
{
  loopEnd = 0;
  delta = 0;
  hte.changeHeaterCurrent(20); // change the heater current to 20mA
  hte.heaterOn();
  errorcode = hte.singleShotTempHum(T_Heater_OFF, RH_Heater_OFF); // read the temperature, the humidity and the dewpoint and turn the heater on  
  errorcode = hte.getDewpoint(Td_Heater_OFF);
  delta = variance_calculation(T_Heater_OFF, RH_Heater_OFF); // calculate the error limit of the first dewpoint 
  delay(5000);
  while(loopEnd==0) // this loop runs until the dewpoint has reached a plateau 
  {
    errorcode = hte.singleShotTempHum(T_Heater_ON, RH_Heater_ON);
    errorcode = hte.getDewpoint(Td_Heater_ON);
    if(abs(Td_Heater_ON_Old-Td_Heater_ON) <= 0.02)
    {
      loopEnd = 1;
    }
    else
    {
      Td_Heater_ON_Old = Td_Heater_ON;
    }
    delay(1000); 
  }
  delta = delta + variance_calculation(T_Heater_ON, RH_Heater_ON); // calculate the error limit for the second dewpoint and add it to the first one 
  if(abs((Td_Heater_OFF-Td_Heater_ON))< delta) // Are the two error limits added larger then the difference of the two dewpoint measuremnts, the sensor is okay 
  {
    Serial.println("Sensor is okay");
  }
  else
  {
    Serial.println("Sensor is faulty");
  }

  hte.heaterOff(); // turn the heater off
  delay(90000); // After the sensor has been heated, it needs a cooling time, after which it can be used again. Otherwise the measurement results will be wrong.
}
