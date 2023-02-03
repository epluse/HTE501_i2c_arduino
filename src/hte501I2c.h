/***************************************************************************/
/* headerfile for "hte501I2c.cpp" module */
/***************************************************************************/
/*
Read functions for measurement values of the HTE501 Sensor via I2C interface.

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

#ifndef hte501I2c_H
#define hte501I2c_H

#include "Arduino.h"
#include "Wire.h"

// Defines
//-----------------------------------------------------------------------------
#define CRC8_ONEWIRE_POLY                           0x31
#define CRC8_ONEWIRE_START                          0xFF
#define HTE501_COMMAND_READ_SINGLE_SHOT             0x2C1B
#define HTE501_COMMAND_READ_PERIODIC_MEASUREMENT    0xE000
#define HTE501_COMMAND_READ_DEW_POINT               0xE016
#define HTE501_COMMAND_READ_WRITE_SENSOR_SETTINGS   0x72A7
#define HTE501_COMMAND_CLEAR_REGISTER_1             0x3041
#define HTE501_COMMAND_READ_REGISTER_1              0xF32D
#define HTE501_COMMAND_READ_REGISTER_2              0xF352
#define HTE501_COMMAND_START_PERIODIC_MEASUREMENT   0x201E
#define HTE501_COMMAND_END_PERIODIC_MEASUREMENT     0x3093
#define HTE501_COMMAND_SOFT_RESET                   0x30A2
#define HTE501_COMMAND_HEATER_ON                    0x306D
#define HTE501_COMMAND_HEATER_OFF                   0x3066
#define HTE501_COMMAND_READ_IDENTIFICATION          0x7029
#define HTE501_REGISTER_PERIODIC_MEASUREMENT_TIME   0x10
#define HTE501_REGISTER_HEATER_CURRENT              0x08
#define HTE501_REGISTER_MEASUREMENT_RESOLUTION      0x0F

// declaration of functions
//-----------------------------------------------------------------------------

class hte501I2c
{
public:
    hte501I2c(void);
    hte501I2c(unsigned char i2cAdress);
    uint8_t singleShotTempHum(float &temperature, float &humidity);
    uint8_t getPeriodicMeasurementTempHum(float &temperature, float &humidity);
    uint8_t getDewpoint(float &dewpoint);
    uint8_t findSensor(void);
    uint8_t changePeriodicMeasurementTime(uint32_t millisec);
    void readPeriodicMeasurementTime(float &periodicMeasurementTime);
    uint8_t changeHeaterCurrent(int mA);
    void readHeaterCurrent(int &heaterCurrent);
    uint8_t changeMeasurementResolution(int measResTemp, int measResHum);
    void readMeasurementResolution(int &measResTemp, int &measResHum);
    void startPeriodicMeasurement(void);
    void endPeriodicMeasurement(void);
    void heaterOn(void);
    void heaterOff(void);
    uint8_t readIdentification(unsigned char identification[]);
    uint8_t newMeasurementReady(bool &measurement);
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