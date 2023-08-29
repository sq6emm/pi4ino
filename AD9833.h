/**********************************************************************
**  Device: AD9833                                                   **
**  File  : AD9833.h                                                 **
**                                                                   **
**  Created by Bo, OZ2M, http://www.rudius.net/oz2m                  **
**                                                                   **
**  Description                                                      **
**  This file is a sample code for your reference. About AD9833      **
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

#ifndef __AD9833_H__
#define __AD9833_H__

class AD9833
{
    public:
        AD9833 (byte devSCK, byte devUpdate);
        void Initialize();
        void SetFrequency(byte ftwNo);  // Freq 0-5
    private:
        byte spiSCK;                    // The Ardunio pin where the SPI clock is made
        byte deviceUpdate;              // The Ardunio pin where the device updated is controlled
};

#endif
