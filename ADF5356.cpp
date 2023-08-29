/**********************************************************************
**  Device: ADF5356                                                  **
**  File  : ADF5356.cpp                                              **
**                                                                   **
**  Created by Bo, OZ2M, http://www.rudius.net/oz2m                  **
**  Modificationa by Brian, GM8BJF and Bernie, GM4WZG                **
**                                                                   **
**  Description:                                                     **
**  This file is a sample code for your reference. About ADF5355     **
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

#include "ADF5356.h"

const uint32_t ftw[6][14] =
{
    {0x200C70, 0x85E7301, 0x887FFFF2, 0x3, 0x30008B84, 0x800025, 0x3540A076, 0x120000E7, 0x102D0428, 0xB0B3CC9, 0xC0107A, 0x61300B, 0x1041C, 0x1D},   // PI4 tone0       * The relevant Frequency Tuning Words
    {0x200C70, 0x85E98C1, 0x8353FFF2, 0x3, 0x30008B84, 0x800025, 0x3540A076, 0x120000E7, 0x102D0428, 0xB0B3CC9, 0xC0107A, 0x61300B, 0x1041C, 0x1D},   // PI4 tone1       * Six FTW are needed for a PI4 + CW + carrier sequence
    {0x200C70, 0x85EBE91, 0xC887FFF2, 0x3, 0x30008B84, 0x800025, 0x3540A076, 0x120000E7, 0x102D0428, 0xB0B3CC9, 0xC0107A, 0x61300B, 0x1041C, 0x1D},   // PI4 tone2       * Set each FTW set to the number of bytes need
    {0x200C70, 0x85EE451, 0x1E2BFFF2, 0x3, 0x30008B84, 0x800025, 0x3540A076, 0x120000E7, 0x102D0428, 0xB0B3CC9, 0xC0107A, 0x61300B, 0x1041C, 0x1D},   // PI4 tone3       * PI4: index 0-3. CW FSK: index 4. CW carrier: index 5
    {0x200C70, 0x85E4561, 0x143FFFF2, 0x3, 0x30008B84, 0x800025, 0x3540A076, 0x120000E7, 0x102D0428, 0xB0B3CC9, 0xC0107A, 0x61300B, 0x1041C, 0x1D},   // CW -FSK tone    *
    {0x200C70, 0x85E85E1, 0x85EBFFF2, 0x3, 0x30008B84, 0x800025, 0x3540A076, 0x120000E7, 0x102D0428, 0xB0B3CC9, 0xC0107A, 0x61300B, 0x1041C, 0x1D}    // CW carrier tone * Example: 1296,9 MHz using a 26 MHz reference
};

/*
    Bo, OZ2M: It is possible to modify/speed up changing frequency
              if some registers are identical. Reloading of identical
              contents, typically found in registers 3-12, is superfluous.
              If so, you will have to modify the SetFrequency function
              slightly yourself. But don't forget to load all registers
              at least ones. This can be done in the Initialize function.
              Also loading all registers can cause clicks because of PLL
              recalibration. Thus you may have to experiment a bit.
*/

// Public

ADF5356::ADF5356(byte LE)
{
    pinLE = LE;
}

void ADF5356::Initialize(void)
{
    SPI.begin();                            // Setup SPI bus
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);             // CPHA = 0 and Clock Polarity (CPOL) positive

    pinMode(pinLE, OUTPUT);
    digitalWrite(pinLE, HIGH);

    WriteRegister(ftw[5][13]);              // Load all regs for nominal frequency. Later on only relevant register(s) should be loaded
    WriteRegister(ftw[5][12]);              // This sequence is for f_PFD < 75 MHz. See datasheet if f_PFD > 75 MHz
    WriteRegister(ftw[5][11]);
    WriteRegister(ftw[5][10]);
    WriteRegister(ftw[5][9]);
    WriteRegister(ftw[5][8]);
    WriteRegister(ftw[5][7]);
    WriteRegister(ftw[5][6]);
    WriteRegister(ftw[5][5]);
    WriteRegister(ftw[5][4]);
    WriteRegister(ftw[5][3]);
    WriteRegister(ftw[5][2]);
    WriteRegister(ftw[5][1]);
    delayMicroseconds(161);
    WriteRegister(ftw[5][0]);
    delayMicroseconds(1);
}

void ADF5356::SetFrequency(byte ftwNo)
{
    WriteRegister(ftw[ftwNo][13]);           // This sequence is for f_PFD < 75 MHz. See datasheet if f_PFD > 75 MHz
    WriteRegister(ftw[ftwNo][10]); 
    WriteRegister(ftw[ftwNo][2]);
    WriteRegister(ftw[ftwNo][1]);
    delayMicroseconds(161);
    WriteRegister(ftw[ftwNo][0]);
    delayMicroseconds(1);
}

// Private

void ADF5356::WriteRegister(const uint32_t value)
{
    digitalWrite(pinLE, LOW);

    for (int8_t i = 3; i >= 0; i--)
        SPI.transfer((value >> 8 * i) & 0xFF);

    digitalWrite(pinLE, HIGH);
    delayMicroseconds(1);
    digitalWrite(pinLE, LOW);
}
