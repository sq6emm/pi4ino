/**********************************************************************
**  Device: Si5351A                                                  **
**  File  : Si5351A.cpp                                              **
**                                                                   **
**  Created by Bo, OZ2M, http://www.rudius.net/oz2m                  **
**                                                                   **
**  Description                                                      **
**  This file is a sample code for your reference. About Si5351A     **
**  module use on Arduino with I2C.                                  **
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

#include "Si5351A.h"

const byte xtalLoad = 0xD2;  // The 25 MHz or 27 MHz crystal loading capacitance. Valid values: 0x12 : 6 pF, 0x92: 8 pF and 0xD2: 10 pF (default). See datasheet for more information

const byte ftw[6][11] =
    // R:26    27    28    29    30    31    32    33    44    45    46
{
    {0xFF, 0xFE, 0x00, 0x0F, 0x4C, 0xFE, 0x61, 0x4C, 0x00, 0x01, 0x00},  // PI4 tone0       * The relevant Frequency Tuning Words
    {0xFF, 0xFE, 0x00, 0x0F, 0x4C, 0xFE, 0x7E, 0xCC, 0x00, 0x01, 0x00},  // PI4 tone1       * Six FTW are needed for a PI4 + CW + carrier sequence
    {0xFF, 0xFE, 0x00, 0x0F, 0x4C, 0xFE, 0x9C, 0x4C, 0x00, 0x01, 0x00},  // PI4 tone2       * Set each FTW set to the number of bytes need
    {0xFF, 0xFE, 0x00, 0x0F, 0x4C, 0xFE, 0xB9, 0xCC, 0x00, 0x01, 0x00},  // PI4 tone3       * PI4: index 0-3. CW FSK: index 4. CW carrier: index 5
    {0xFF, 0xFE, 0x00, 0x0F, 0x4C, 0xFE, 0x50, 0x4C, 0x00, 0x01, 0x00},  // CW -FSK tone    *
    {0xFF, 0xFE, 0x00, 0x0F, 0x4C, 0xFE, 0x70, 0xCC, 0x00, 0x01, 0x00}   // CW carrier tone * Example: 144,170 MHz for fXTAL = 25 MHz
};

/*
    Bo, OZ2M: It is possible to modify/speed up changing frequency
              if some registers are identical. Reloading of identical
              contents, typically found in registers 26-33/44-46, is superflous.
              If so, you will have to modifify the SetFrequency function
              slightly yourself. But don't forget to load all registers
              at least ones. This can be done in the Initialize function.
*/

// Public

Si5351A::Si5351A()
{
}

void Si5351A::Initialize()
{
    Wire.begin();

    WriteRegister(117, 0xA0);                       // Reset the Si5351A just to be on the safe side
    WriteRegister(183, xtalLoad);                   // Set xtal loading capacitance

    WriteRegister( 3, 0xFE);                        // Output Enable Control, enable only CLK0_OEB
    WriteRegister( 9, 0xFE);                        // OEB Pin Enable Control Mask, enable only OEB_CLK0
    WriteRegister(15, 0x00);                        // PLL Input Source, select the XTAL input as the reference clock for PLLA
    for (byte i = 16; i < 24; i++)
        WriteRegister (i, 0x80);                    // CLKi Control, power down CLKi
    WriteRegister(24, 0x03);                        // CLK3–0 Disable State, unused are low and never disable CLK0

    WriteRegister(42, 0x00);                        // MS0_P3 = c = 1
    WriteRegister(43, 0x01);
    WriteRegister(47, 0x00);                        // MS0_P2 = b = 0
    WriteRegister(48, 0x00);
    WriteRegister(49, 0x00);

    WriteRegister(16, 0x4F);                        // Power up CLK0, MS0 operates in integer mode, Output Clock 0 is not inverted, Select MultiSynth 0 as the source for CLK0 and 8 mA

    SetFrequency(5);                                // Set carrier to coarse adjust PLL
    WriteRegister(177, 0x20);
}

void Si5351A::SetFrequency(byte ftwNo)
{
    interrupts();                                   // Enable global interrupts - dangerous move but works ??
    // Is needed because Wire.h needs global interrupts enabled
    WriteRegister(26, ftw[ftwNo][0]);
    WriteRegister(27, ftw[ftwNo][1]);
    WriteRegister(28, ftw[ftwNo][2]);
    WriteRegister(29, ftw[ftwNo][3]);
    WriteRegister(30, ftw[ftwNo][4]);
    WriteRegister(31, ftw[ftwNo][5]);
    WriteRegister(32, ftw[ftwNo][6]);
    WriteRegister(33, ftw[ftwNo][7]);
    WriteRegister(44, ftw[ftwNo][8]);
    WriteRegister(45, ftw[ftwNo][9]);
    WriteRegister(46, ftw[ftwNo][10]);
}

// Private

void Si5351A::WriteRegister(byte regAdr, byte value)
{
    Wire.beginTransmission(0x60);
    Wire.write(regAdr);
    Wire.write(value);
    Wire.endTransmission();
}
