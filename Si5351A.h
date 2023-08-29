/**********************************************************************
**  Device: Si5351A                                                  **
**  File  : Si5351A.h                                                **
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

#include <Wire.h>

#ifndef __Si5351A_H__
#define __Si5351A_H__

class Si5351A
{
    public:
        Si5351A();
        void Initialize();
        void SetFrequency(byte ftwNo); // Freq 0-5
    private:
        void WriteRegister(byte regAdr, byte value);
};

#endif
