/*********************************************************************
**  Device: AD9851                                                   **
**  File  : AD9851.cpp                                               **
**                                                                   **
**  Created by Bo, OZ2M, http://www.rudius.net/oz2m                  **
**                                                                   **
**  Description                                                      **
**  This file is a sample code for your reference. About AD9851      **
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

#include "AD9851.h"

#define PulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }

const byte ftw[6][4] =
{
    {0x47, 0xAD, 0xAC, 0x59},  // PI4 tone0       * The relevant Frequency Tuning Words
    {0x47, 0xAD, 0xC2, 0x32},  // PI4 tone1       * Six FTW are needed for a PI4 + CW + carrier sequence
    {0x47, 0xAD, 0xD8, 0x0A},  // PI4 tone2       * Set each FTW set to the number of bytes need
    {0x47, 0xAD, 0xED, 0xE2},  // PI4 tone3       * PI4: index 0-3. CW FSK: index 4. CW carrier: index 5
    {0x47, 0xAD, 0x9F, 0xF8},  // CW -FSK tone    *
    {0x47, 0xAD, 0xB7, 0x45}   // CW carrier tone * Example: 50,399 MHz using 6 x 30 MHz = 180 MHz clock
};
// Public

AD9851::AD9851(byte devSCK, byte devSS, byte devReset, byte devUpdate)
{
    spiSCK = devSCK;
    spiSS = devSS;
    deviceReset = devReset;
    deviceUpdate = devUpdate;
}

void AD9851::Initialize(void)
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
    SPI.setBitOrder(LSBFIRST);

    PulseHigh(spiSCK);                      // Pulse the SPI SCK pin
    PulseHigh(deviceUpdate);                // Pulse the device update pin
}

void AD9851::SetFrequency(byte ftwNo)
{
    digitalWrite(spiSS, LOW);               // Select device

    for (int8_t i = 3; i >= 0; i--)         // Send each byte starting with LSB
        SPI.transfer(ftw[ftwNo][i]);

    SPI.transfer(0x01);                     // Send control byte, enable x6 multiplier
    PulseHigh(deviceUpdate);                // Update device

    digitalWrite(spiSS, HIGH);              // Deselect device
}

// Private

