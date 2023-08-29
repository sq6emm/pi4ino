/**********************************************************************
**  Device: ADF4157                                                  **
**  File  : ADF4157.cpp                                              **
**                                                                   **
**  Created by Dawid Szymanski SQ6EMM 08/2023                        **
**  Based on ADF4351 by:                                             **
**            JA Soler EA3HMJ 05/2014, http://qlfecv.wordpress.com   **
**                                                                   **
**  Description:                                                     **
**  This file is a sample code for your reference. About ADF4157     **
**  module use on Arduino with SPI.                                  **
**                                                                   **
**  This code is free software; you can redistribute it and/or       **
**  modify it under the terms of the GNU Lesser General Public       **
**  License as published by the Free Software Foundation; either     **
**  version 2.1 of the License, or (at your option)                  **
**  any later version.                                               **
**                                                                   **
**  Copyright (C) 2014 JA Soler EA3HMJ.                              **
**                                                                   **
**********************************************************************/

#include <Arduino.h>

#include "ADF4157.h"

const uint32_t ftw[6][6] =
{   
        {0x30206B88, 0xA3D8001, 0xD108002, 0x43, 0x4},   // PI4 tone0       * The relevant Frequency Tuning Words
        {0x30206B88, 0xB020001, 0xD108002, 0x43, 0x4},   // PI4 tone1       * Six FTW are needed for a PI4 + CW + carrier sequence
        {0x30206B88, 0xBC68001, 0xD108002, 0x43, 0x4},   // PI4 tone2       * Set each FTW set to the number of bytes need
        {0x30206B88, 0xC8B8001, 0xD108002, 0x43, 0x4},   // PI4 tone3       * PI4: index 0-3. CW FSK: index 4. CW carrier: index 5
        {0x30206B88, 0x9500001, 0xD108002, 0x43, 0x4},   // CW -FSK tone    *
        {0x30206B88, 0xA9F8001, 0xD108002, 0x43, 0x4}    // CW carrier tone * Example: 1296,805 MHz using a 10 MHZ reference
};

/*
    Bo, OZ2M: It is possible to modify/speed up changing frequency
              if some registers are identical. Reloading of identical
              contents, typically found in registers 2-5, is superfluous.
              If so, you will have to modify the SetFrequency function
              slightly yourself. But don't forget to load all registers
              at least ones. This can be done in the Initialize function.
              Also loading all registers can cause clicks because of PLL
              recalibration. Thus you may have to experiment a bit.
*/

// Public

ADF4157::ADF4157(byte LE)
{
    pinLE = LE;
}

void ADF4157::Initialize(void)
{
    SPI.begin();                            // Setup SPI bus
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);             // CPHA = 0 and Clock Polarity (CPOL) positive

    pinMode(pinLE, OUTPUT);
    digitalWrite(pinLE, HIGH);

    for (int8_t i = 4; i >= 0; i--)        // Load all regs for nominal frequency. Later on only relevant register(s) should be loaded
        WriteRegister(ftw[5][i]);
}

void ADF4157::SetFrequency(byte ftwNo)
{
    for (int8_t i = 4; i >= 0; i--)
    {
        WriteRegister(ftw[ftwNo][i]);
        delayMicroseconds(1);
    }
}

// Private

void ADF4157::WriteRegister(const uint32_t value)
{
    digitalWrite(pinLE, LOW);

    for (int8_t i = 3; i >= 0; i--)
        SPI.transfer((value >> 8 * i) & 0xFF);

    digitalWrite(pinLE, HIGH);
    delayMicroseconds(1);
    digitalWrite(pinLE, LOW);
}
