/**********************************************************************
**  Device: Radio                                                    **
**  File  : Radio.h                                                  **
**                                                                   **
**  Created by Bo, OZ2M, http://www.rudius.net/oz2m                  **
**                                                                   **
**  Description                                                      **
**  This file is a sample code for your reference.                   **
**  Inspired by: interface.khm.de/index.php/lab/                     **
**               interfaces-advanced/arduino-dds-sinewave-generator  **
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

#ifndef __Radio_H__
#define __Radio_H__

class Radio
{
    public:
        Radio(byte pttPin, byte afSig);
        void Initialize();
        void SetFrequency(byte ftwNo); // Freq 0-5
    private:
        byte ptt;                      // The Arduino pin where PTT is made i the audio soft-DDS solution
        byte audio;                    // The Ardunio pin where the audio is present
};

#endif
