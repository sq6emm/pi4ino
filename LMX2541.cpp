/**********************************************************************
**  Device: LMX2541                                                  **
**  File  : LMX2541.cpp                                              **
**                                                                   **
**  Created by                                                       **
**                                                                   **
**  Description:                                                     **
**  This file is a sample code for your reference. About LMX2541     **
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

#include "LMX2541.h"

const uint32_t ftw[6][12] =
{   //Reg/#7/0        13/1        12/2         9/3         8/4         6/5         5/6         4/7         3/8         2/9        1/10        0/11        registers are in the in same order as the CodeLoader output
    {0x00000017, 0x000008D, 0x0000001C, 0x28001409, 0x0111CE58, 0x001F3326, 0xA0040005, 0xFF4820A4, 0x00A87303, 0x040C3502, 0x00000011, 0x4D581030},   // PI4 tone0       * The relevant Frequency Tuning Words
    {0x00000017, 0x000008D, 0x0000001C, 0x28001409, 0x0111CE58, 0x001F3326, 0xA0040005, 0xFF4820A4, 0x00A87303, 0x040C3502, 0x00000011, 0x4D571030},   // PI4 tone1       * Six FTW are needed for a PI4 + CW + carrier sequence
    {0x00000017, 0x000008D, 0x0000001C, 0x28001409, 0x0111CE58, 0x001F3326, 0xA0040005, 0xFF4820A4, 0x00A87303, 0x040C3502, 0x00000011, 0x4D561030},   // PI4 tone2       * Set each FTW set to the number of bytes need
    {0x00000017, 0x000008D, 0x0000001C, 0x28001409, 0x0111CE58, 0x001F3326, 0xA0040005, 0xFF4820A4, 0x00A87303, 0x040C3502, 0x00000011, 0x4D551030},   // PI4 tone3       * PI4: index 0-3. CW FSK: index 4. CW carrier: index 5
    {0x00000017, 0x000008D, 0x0000001C, 0x28001409, 0x0111CE58, 0x001F3326, 0xA0040005, 0xFF4820A4, 0x00A87303, 0x040C3502, 0x00000011, 0x4D581030},   // CW -FSK tone    *
    {0x00000017, 0x000008D, 0x0000001C, 0x28001409, 0x0111CE58, 0x001F3326, 0xA0040005, 0xFF4820A4, 0x00A87303, 0x040C3502, 0x00000011, 0x4D581030}    // CW carrier tone * Example: 1296,900 MHz for a LMX2541-Q2690
};

/*
    Bo, OZ2M: It is possible to modify/speed up changing frequency
              if some registers are identical. Reloading of identical
              contents, typically found in some registers, is superfluous.
              If so, you will have to modify the SetFrequency function
              slightly yourself. But don't forget to load all registers
              at least ones. This can be done in the Initialize function.
              Also loading all registers can cause clicks because of PLL
              recalibration initiated by loading register 0/index 11.
              Thus you may have to experiment a bit.
*/

// Public

LMX2541::LMX2541(byte LE)
{
    pinLE = LE;
}

void LMX2541::Initialize(void)
{
    SPI.begin();                            // Setup SPI bus
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);             // CPHA = 0 and Clock Polarity (CPOL) positive

    pinMode(pinLE, OUTPUT);
    digitalWrite(pinLE, HIGH);

    for (byte i = 0; i < 12; i++)            // Load all regs for nominal frequency. Later on only relevant register(s) should be loaded
        WriteRegister(ftw[5][i]);
}

void LMX2541::SetFrequency(byte ftwNo)
{
    for (byte i = 9; i < 12; i++)            // Write only register 2 to 0, index 9 to 11 to device (order as CodeLoader output)
        WriteRegister(ftw[ftwNo][i]);
}

// Private

void LMX2541::WriteRegister(const unsigned long value)
{
    digitalWrite(pinLE, LOW);

    for (int8_t i = 3; i >= 0; i--)             // Write MSB first
        SPI.transfer((value >> 8 * i) & 0xFF);

    digitalWrite(pinLE, HIGH);
    digitalWrite(pinLE, LOW);
}
