/*********************************************************************
**  Device: AD9833                                                   **
**  File  : AD9833.cpp                                               **
**                                                                   **
**  Created by Bo, OZ2M, http://www.rudius.net/oz2m                  **
**                                                                   **
**  Description                                                      **
**  This file is a sample code for your reference. About AD9833      **
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

#include "AD9833.h"

const byte ftw[6][4] =
{
    {0x49, 0x21, 0x65, 0xE8},  // PI4 tone0       * The relevant Frequency Tuning Words
    {0x49, 0x21, 0x6F, 0xBD},  // PI4 tone1       * Six FTW are needed for a PI4 + CW + carrier sequence
    {0x49, 0x21, 0x79, 0x91},  // PI4 tone2       * Set each FTW set to the number of bytes need
    {0x49, 0x22, 0x43, 0x66},  // PI4 tone3       * PI4: index 0-3. CW FSK: index 4. CW carrier: index 5
    {0x49, 0x21, 0x60, 0x56},  // CW -FSK tone    *
    {0x49, 0x21, 0x6A, 0xD2}   // CW carrier tone * Example: 3,567 MHz using a 25 MHz clock
};

// Public

AD9833::AD9833(byte devSCK, byte devUpdate)
{
    spiSCK = devSCK;
    deviceUpdate = devUpdate;
}

void AD9833::Initialize(void)
{
    pinMode(deviceUpdate, OUTPUT);
    digitalWrite(deviceUpdate, HIGH);

    SPI.begin();                            // Setup SPI bus
    SPI.setDataMode(SPI_MODE2);
    SPI.setBitOrder(MSBFIRST);

    SPI.transfer(0x10);                     // Control word MSB, reset
    SPI.transfer(0x00);                     // Control word LSB, sine wave
}

void AD9833::SetFrequency(byte ftwNo)
{
    digitalWrite(deviceUpdate, LOW);

    SPI.transfer(0x20);                     // Send control word MSB
    SPI.transfer(0x00);                     // Send control word LSB

    SPI.transfer(ftw[ftwNo][2]);            // Send each byte starting with LSB
    SPI.transfer(ftw[ftwNo][3]);
    digitalWrite(deviceUpdate, HIGH);       // Update device

    digitalWrite(deviceUpdate, LOW);
    SPI.transfer(ftw[ftwNo][0]);
    SPI.transfer(ftw[ftwNo][1]);

    digitalWrite(deviceUpdate, HIGH);       // Update device
}

// Private

