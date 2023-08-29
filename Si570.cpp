/**********************************************************************
**  Device: Si570                                                    **
**  File  : Si570.cpp                                                **
**                                                                   **
**  Created by Bo, OZ2M, http://www.rudius.net/oz2m                  **
**                                                                   **
**  Description                                                      **
**  This file is a sample code for your reference. About AD9851      **
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

#include "Si570.h"

const byte ftw[6][6] =
{
    {0x41, 0x42, 0xD6, 0xB2, 0x28, 0xCE},  // PI4 tone0       * The relevant Frequency Tuning Words
    {0x41, 0x42, 0xD6, 0xB2, 0x76, 0x3B},  // PI4 tone1       * Six FTW are needed for a PI4 + CW + carrier sequence
    {0x41, 0x42, 0xD6, 0xB2, 0xC3, 0xA7},  // PI4 tone2       * Set each FTW set to the number of bytes need
    {0x41, 0x42, 0xD6, 0xB3, 0x11, 0x13},  // PI4 tone3       * PI4: index 0-3. CW FSK: index 4. CW carrier: index 5
    {0x41, 0x42, 0xD6, 0xB1, 0xFC, 0xEF},  // CW -FSK tone    *
    {0x41, 0x42, 0xD6, 0xB2, 0x4F, 0x85}   // CW carrier tone * Example: 144,170 MHz for fXTAL = 114,273176 MHz
};

/*
    Bo, OZ2M: It is possible to modify/speed up changing frequency
              if some registers are identical. Reloading of identical
              contents, typically found in registers 7-9/13-15, is superflous.
              If so, you will have to modifify the SetFrequency function
              slightly yourself. But don't forget to load all registers
              at least ones. This can be done in the Initialize function.
*/

// Public

Si570::Si570(byte devStartReg)
{
    startReg = devStartReg;
}

void Si570::Initialize()
{
    Wire.begin();

    // Set DCO center at the beacon's nominal frequency
    WriteRegister(135, 1);                           // Reset the Si570 just to be on the safe side
    WriteRegister(137, ReadRegister(137) | 0x10);    // Freeze DCO

    Wire.beginTransmission(0x55);
    Wire.write(startReg);                            // Send start register

    for (byte i = 0; i < 6; i++)
        Wire.write(ftw[5][i]);

    Wire.endTransmission();

    WriteRegister(137, ReadRegister(137) & 0xEF);   // Unfreece DCO
    WriteRegister(135, ReadRegister(135) & 0xDF);   // Set NewFreq bit
}

void Si570::SetFrequency(byte ftwNo)
{
    interrupts();                                   // Enable global interrupts - dangerous move but works ??
    // Is needed because Wire.h needs global interrupts enabled
    WriteRegister(135, ReadRegister(135) | 0x20);   // Halt frequency change until regs set, i.e. Freeze M bit = 1

    Wire.beginTransmission(0x55);
    Wire.write(startReg);                           // Send start register

    for (byte i = 0; i < 6; i++)
        Wire.write(ftw[ftwNo][i]);

    Wire.endTransmission();

    WriteRegister(135, ReadRegister(135) & 0xEF);   // Apply frequency change, i.e. Freeze M bit = 0
    WriteRegister(135, ReadRegister(135) & 0xDF);   // Set NewFreq bit
}

// Private

byte Si570::ReadRegister(byte regAdr)
{
    byte result;

    Wire.beginTransmission(0x55);
    Wire.write(regAdr);
    Wire.endTransmission(false);
    Wire.requestFrom((byte) 0x55, (byte) 1);
    result = Wire.read();
    Wire.endTransmission();
    return result;
}

void Si570::WriteRegister(byte regAdr, byte value)
{
    Wire.beginTransmission(0x55);
    Wire.write(regAdr);
    Wire.write(value);
    Wire.endTransmission();
}
