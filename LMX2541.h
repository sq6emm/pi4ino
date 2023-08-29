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

#include <SPI.h>

#ifndef __LMX2541_H__
#define __LMX2541_H__

class LMX2541
{
    public:
        LMX2541(byte LE);
        void Initialize();
        void SetFrequency(byte ftwNo);                        // Freq 0-5
    private:
        void WriteRegister(const unsigned long value);
        byte pinLE;                                           // The Aurdino pin where the SPI LE to the device is controlled
};

#endif
