/**********************************************************************
**  Device: ADF4351 and ADF4350                                      **
**  File  : ADF4351.cpp                                              **
**                                                                   **
**  Created by JA Soler EA3HMJ 05/2014, http://qlfecv.wordpress.com  **
**                                                                   **
**  Description:                                                     **
**  This file is a sample code for your reference. About ADF4351     **
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

#include "ADF4351.h"

const uint32_t ftw[6][6] =
{   
        {0xACB1C0, 0x800CEF9, 0x60004E42, 0x4B3, 0x85003C, 0x580005},   // PI4 tone0       * The relevant Frequency Tuning Words
        {0xACB2A8, 0x800D069, 0x60004E42, 0x4B3, 0x85003C, 0x580005},   // PI4 tone1       * Six FTW are needed for a PI4 + CW + carrier sequence
        {0xACC2C8, 0x800EA01, 0x60004E42, 0x4B3, 0x85003C, 0x580005},   // PI4 tone2       * Set each FTW set to the number of bytes need
        {0xACB280, 0x800D029, 0x60004E42, 0x4B3, 0x85003C, 0x580005},   // PI4 tone3       * PI4: index 0-3. CW FSK: index 4. CW carrier: index 5
        {0xACBB38, 0x800DE01, 0x60004E42, 0x4B3, 0x85003C, 0x580005},   // CW -FSK tone    *
        {0xAC81F8, 0x8008321, 0x60004E42, 0x4B3, 0x85003C, 0x580005}    // CW carrier tone * Example: 3456,3 MHz using a 10 MHZ reference, x3 to reach 10,3689 GHz
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

ADF4351::ADF4351(byte LE)
{
    pinLE = LE;
}

void ADF4351::Initialize(void)
{
    SPI.begin();                            // Setup SPI bus
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);             // CPHA = 0 and Clock Polarity (CPOL) positive

    pinMode(pinLE, OUTPUT);
    digitalWrite(pinLE, HIGH);

    for (int8_t i = 5; i >= 0; i--)        // Load all regs for nominal frequency. Later on only relevant register(s) should be loaded
        WriteRegister(ftw[5][i]);
}

void ADF4351::SetFrequency(byte ftwNo)
{
    for (int8_t i = 5; i >= 0; i--)
    {
        WriteRegister(ftw[ftwNo][i]);
        delayMicroseconds(1);
    }
}

// Private

void ADF4351::WriteRegister(const uint32_t value)
{
    digitalWrite(pinLE, LOW);

    for (int8_t i = 3; i >= 0; i--)
        SPI.transfer((value >> 8 * i) & 0xFF);

    digitalWrite(pinLE, HIGH);
    delayMicroseconds(1);
    digitalWrite(pinLE, LOW);
}
