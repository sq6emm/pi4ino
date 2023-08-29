/**********************************************************************
**  Device: Radio                                                    **
**  File  : Radio.cpp                                                **
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

#include <Arduino.h>
#include "avr/pgmspace.h"
#include "Radio.h"

const byte sine256[] PROGMEM =
{
    127, 130, 133, 136, 139, 143, 146, 149, 152, 155, 158, 161, 164, 167, 170, 173, 176, 178, 181, 184, 187, 190, 192, 195, 198, 200, 203, 205, 208, 210, 212, 215, 217, 219, 221, 223, 225, 227, 229, 231, 233, 234, 236, 238, 239, 240,
    242, 243, 244, 245, 247, 248, 249, 249, 250, 251, 252, 252, 253, 253, 253, 254, 254, 254, 254, 254, 254, 254, 253, 253, 253, 252, 252, 251, 250, 249, 249, 248, 247, 245, 244, 243, 242, 240, 239, 238, 236, 234, 233, 231, 229, 227, 225, 223,
    221, 219, 217, 215, 212, 210, 208, 205, 203, 200, 198, 195, 192, 190, 187, 184, 181, 178, 176, 173, 170, 167, 164, 161, 158, 155, 152, 149, 146, 143, 139, 136, 133, 130, 127, 124, 121, 118, 115, 111, 108, 105, 102, 99, 96, 93, 90, 87, 84, 81, 78,
    76, 73, 70, 67, 64, 62, 59, 56, 54, 51, 49, 46, 44, 42, 39, 37, 35, 33, 31, 29, 27, 25, 23, 21, 20, 18, 16, 15, 14, 12, 11, 10, 9, 7, 6, 5, 5, 4, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 5, 5, 6, 7, 9, 10, 11, 12, 14, 15, 16, 18, 20, 21, 23, 25, 27, 29, 31,
    33, 35, 37, 39, 42, 44, 46, 49, 51, 54, 56, 59, 62, 64, 67, 70, 73, 76, 78, 81, 84, 87, 90, 93, 96, 99, 102, 105, 108, 111, 115, 118, 121, 124
};

// PI4, -FSK and CW carrier tones offset by +1 kHz
const uint32_t ftw[] = {230377224, 262463188, 294549153, 326635117, 212195178, 246420206};
// PI4, -FSK and CW carrier tones without offset! For testing only
//const uint32_t ftw[] = {93477110, 125563074, 157649038, 189735003, 75295063, 109520092};

// Private

// Timer2 ISR variables
volatile uint32_t phaseAcc = 0;  // Phase accumulator
volatile uint32_t stw;           // Soft DDS tuning word

ISR(TIMER2_OVF_vect)
{
    phaseAcc = phaseAcc + stw;
    OCR2A = pgm_read_byte_near(sine256 + (phaseAcc >> 24));
}

// Public

Radio::Radio(byte pttPin, byte afSig)
{
    ptt = pttPin;
    audio = afSig;
}

void Radio::Initialize(void)
{
    pinMode(ptt, OUTPUT);
    digitalWrite(ptt, LOW);        // Put radio in TX

    pinMode(audio, OUTPUT);        // Enable audio output pin

    // Configure Timer2
    TCCR2A = 0b10000001;           // Clear OC2A on Compare Match and Phase Correct Mode 1
    TCCR2B = 0b00000001;           // Clock prescaling is 1

    TIMSK2 = 1;                    // Enable Timer2 interrupt
}

void Radio::SetFrequency(byte ftwNo)
{
    stw = ftw[ftwNo];
}
