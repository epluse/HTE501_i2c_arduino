/***************************************************************************/
/* headerfile for "hte501I2c.cpp" module */
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

#ifndef hte501I2c_H
#define hte501I2c_H

#include "Arduino.h"
#include "Wire.h"

// Defines
//-----------------------------------------------------------------------------
#define CRC8_ONEWIRE_POLY 0x31
#define CRC8_ONEWIRE_START 0xFF

enum Errorcode
{
    OKAY = 0,
    ERR_CKSUM = 1,
    CON_OK = 2,
    NO_SENSOR_ON_ADDR = 3,
    NO_SENSOR = 4,
    SEC_TO_HIGH = 5,
    CUR_NOT_IN_SPEC = 6,
    MEAS_RES_WRONG = 7
};

// declaration of functions
//-----------------------------------------------------------------------------

class hte501I2c
{
public:
    hte501I2c(void);
    hte501I2c(unsigned char i2cAdress);
    uint8_t singleShotTempHum(float &temperature, float &humidity);
    uint8_t getPeriodicMeasurmentTempHum(float &temperature, float &humidity);
    uint8_t getDewpoint(float &dewpoint);
    uint8_t findSensor(void);
    uint8_t changePeriodicMeasurmentTime(uint32_t millisec);
    void readPeriodicMeasurmentTime(float &periodicMeasurmentTime);
    uint8_t changeHeaterCurrent(int mA);
    void readHeaterCurrent(int &heaterCurrent);
    uint8_t changeMeasurmentResolution(int measResTemp, int measResHum);
    void readMeasurmentResolution(int &measResTemp, int &measResHum);
    void startPeriodicMeasurment(void);
    void endPeriodicMeasurment(void);
    void heaterOn(void);
    void heaterOff(void);
    uint8_t readIdentification(unsigned char identification[]);
    uint8_t newMeasurmentReady(bool &measurement);
    uint8_t constantHeaterOnOff(bool &conHeaterOnOff);
    void clearStatusregister1(void);
    void reset(void);
    unsigned char address = 0x40;
    void wireWrite(unsigned char buf[], int to, bool stopmessage);
    void wireRead(unsigned char buf[], uint8_t to);
    unsigned char calcCrc8(unsigned char buf[], unsigned char from, unsigned char to);
    void getErrorString(uint8_t Status, char errorString[]);
};

#endif