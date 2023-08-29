/**********************************************************************
**  Device: AD9912                                                   **
**  File  : AD9912.cpp                                               **
**                                                                   **
**  Created by Bo, OZ2M, http://www.rudius.net/oz2m                  **
**  Modify by JA Soler EA3HMJ 05/2014, http://qlfecv.wordpress.com   **
**                                                                   **
**  Description                                                      **
**  This file is a sample code for your reference. About AD9912      **
**  module use on Arduino with SPI.                                  **
**                                                                   **
**  This code is free software; you can redistribute it and/or       **
**  modify it under the terms of the GNU Lesser General Public       **
**  License as published by the Free Software Foundation; either     **
**  version 2.1 of the License, or (at your option)                  **
**  any later version.                                               **
**                                                                   **
**  Copyright (C) Bo, OZ2M                                           **
**                                                                   **
**********************************************************************/

#include <Arduino.h>

#include "AD9912.h"

#define PulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }

const byte ftw[6][6] =
{
    {0x0C, 0xE6, 0xF0, 0xF1, 0x6F, 0x44},  // PI4 tone0       * The relevant Frequency Tuning Words
    {0x0C, 0xE6, 0xF4, 0xE0, 0x11, 0x4D},  // PI4 tone1       * Six FTW are needed for a PI4 + CW + carrier sequence
    {0x0C, 0xE6, 0xF8, 0xCE, 0xB3, 0x57},  // PI4 tone2       * Set each FTW set to the number of bytes need
    {0x0C, 0xE6, 0xFC, 0xBD, 0x55, 0x60},  // PI4 tone3       * PI4: index 0-3. CW FSK: index 4. CW carrier: index 5
    {0x0C, 0xE6, 0xEE, 0xB7, 0x02, 0x61},  // CW -FSK tone    *
    {0x0C, 0xE6, 0xF2, 0xE8, 0xC0, 0x49}   // CW carrier tone * Example: 50,399 MHz using a 1 GHz clock
};

// Public

AD9912::AD9912(byte devSS, byte devReset, byte devUpdate)
{
    spiSS = devSS;
    deviceReset = devReset;
    deviceUpdate = devUpdate;
}

void AD9912::Initialize(void)
{
    pinMode(spiSS, OUTPUT);
    digitalWrite(spiSS, HIGH);

    pinMode(deviceReset, OUTPUT);
    digitalWrite(deviceReset, HIGH);
    delay(10);
    digitalWrite(deviceReset, LOW);

    pinMode(deviceUpdate, OUTPUT);
    digitalWrite(deviceUpdate, LOW);

    SPI.begin();                            // Setup SPI bus
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.transfer(0x10);                     // Init control word, 16 bits, see DDS data sheet p. 28 and p. 33 table 13, MSB first, SDIO used
    SPI.transfer(0x00);
}

void AD9912::SetFrequency(byte ftwNo)
{
    digitalWrite(spiSS, LOW);                // Select device

    SPI.transfer(0x61);                      // Send freq. address word, 16 bits, DDS data sheet p. 28 table 9 and p. 29 table 10,
    SPI.transfer(0xAB);                      // 011 0 bin + 01AB hex, 01AB is FTW MSB address

    for (byte i = 0; i < 6; i++)
        SPI.transfer(ftw[ftwNo][i]);

    PulseHigh(deviceUpdate);                 // Update device

    digitalWrite(spiSS, HIGH);               // Deselect device
}

// Private
