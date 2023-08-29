/**********************************************************************
**  Device: ADF5355                                                  **
**  File  : ADF5355.h                                                **
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

#include <SPI.h>

#ifndef __ADF5355_H__
#define __ADF5355_H__

class ADF5355
{
    public:
        ADF5355(byte LE);
        void Initialize();
        void SetFrequency(byte ftwNo);                       // Freq 0-5
    private:
        void WriteRegister(const uint32_t value);
        byte pinLE;                                         // The Aurdino pin where the SPI LE to the device is controlled
};

#endif
