/**********************************************************************
**  Device: AD9913                                                   **
**  File  : AD9913.cpp                                               **
**                                                                   **
**  Created by Bo, OZ2M, http://www.rudius.net/oz2m                  **
**                                                                   **
**  Description                                                      **
**  This file is a sample code for your reference. About AD9913      **
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

#include "AD9913.h"

#define PulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }

const byte ftw[6][4] =
{
    {0x47, 0xFC, 0x6E, 0x2A},  // PI4 tone0       * The relevant Frequency Tuning Words
    {0x47, 0xFC, 0x7D, 0xE5},  // PI4 tone1       * Six FTW are needed for a PI4 + CW + carrier sequence
    {0x47, 0xFC, 0x8D, 0x9F},  // PI4 tone2       * Set each FTW set to the number of bytes need
    {0x47, 0xFC, 0x9D, 0x5A},  // PI4 tone3       * PI4: index 0-3. CW FSK: index 4. CW carrier: index 5
    {0x47, 0xFC, 0x65, 0x40},  // CW -FSK tone    *
    {0x47, 0xFC, 0x76, 0x07}   // CW carrier tone * Example: 70,299 MHz using a 250 MHz clock
};

// Public

AD9913::AD9913(byte devSS, byte devReset, byte devUpdate)
{
    spiSS = devSS;
    deviceReset = devReset;
    deviceUpdate = devUpdate;
}

void AD9913::Initialize(void)
{
    pinMode(spiSS, OUTPUT);
    digitalWrite(spiSS, HIGH);
    pinMode(deviceReset, OUTPUT);
    digitalWrite(deviceReset, LOW);
    pinMode(deviceUpdate, OUTPUT);
    digitalWrite(deviceUpdate, LOW);

    digitalWrite(deviceReset, HIGH);         // Reset DDS
    delay(1);
    digitalWrite(deviceReset, LOW);
    delay(1);

    SPI.begin();                             // Setup SPI bus
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
}

void AD9913::SetFrequency(byte ftwNo)
{
    digitalWrite(spiSS, LOW);                // Select device

    SPI.transfer(0x03);                      // FTW register

    for (byte i = 0; i < 4; i++)
        SPI.transfer(ftw[ftwNo][i]);
    PulseHigh(deviceUpdate);                 // Update device

    digitalWrite(spiSS, HIGH);               // Deselect device
}

// Private
