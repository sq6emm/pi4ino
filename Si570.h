/**********************************************************************
**  Device: Si570                                                    **
**  File  : Si570.h                                                  **
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

#include <Wire.h>

#ifndef __Si570_H__
#define __Si570_H__

class Si570
{
    public:
        Si570(byte devStartReg);
        void Initialize();
        void SetFrequency(byte ftwNo); // Freq 0-5
    private:
        byte startReg;                  // The Si570 start register 7/13
        byte ReadRegister(byte regAdr);
        void WriteRegister(byte regAdr, byte value);
};

#endif
