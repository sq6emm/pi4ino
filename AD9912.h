/**********************************************************************
**  Device: AD9912                                                   **
**  File  : AD9912.h                                                 **
**                                                                   **
**  Created by Bo, OZ2M, http://www.rudius.net/oz2m                  **
**                                                                   **
**  Description                                                      **
**  This file is a sample code for your reference. About AD9912      **
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

#ifndef __AD9912_H__
#define __AD9912_H__

class AD9912
{
    public:
        AD9912(byte spiSS, byte deviceReset, byte deviceUpdate);
        void Initialize();
        void SetFrequency(byte ftwNo); // Freq 0-5
    private:
        byte spiSS;                    // The Aurdino pin where the SPI SS to the device is controlled
        byte deviceReset;              // The Ardunio pin where the device reset is controlled
        byte deviceUpdate;             // The Ardunio pin where the device updated is controlled
};

#endif
