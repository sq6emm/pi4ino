/**********************************************************************
**  Device: ADF4351 and ADF4350                                      **
**  File  : ADF4351.h                                                **
**                                                                   **
**  Created by JA Soler EA3HMJ 05/2014, http://qlfecv.wordpress.com  **
**                                                                   **
**  Description:                                                     **
**  This file is a sample code for your reference. About ADF4351     **
**  module use on Arduino with SPI.                                  **
**                                                                   **
**  This code is free software; you can redistribute it and/or       **
**  modify it under the terms of the GNU Lesser General Public       **
**  License as published by the Free Software Foundation; either     **
**  version 2.1 of the License, or (at your option)                  **
**  any later version.                                               **
**                                                                   **
**  Copyright (C) 2014 JA Soler EA3HMJ.                              **
**                                                                   **
**********************************************************************/

#include <SPI.h>

#ifndef __ADF4351_H__
#define __ADF4351_H__

class ADF4351
{
    public:
        ADF4351(byte LE);
        void Initialize();
        void SetFrequency(byte ftwNo);                       // Freq 0-5
    private:
        void WriteRegister(const uint32_t value);
        byte pinLE;                                         // The Aurdino pin where the SPI LE to the device is controlled
};

#endif
