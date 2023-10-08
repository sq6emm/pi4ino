/*
    Arduino program to send a mixed mode PI4 + CW + carrier beacon sequence.

    Modify the "Beacon and hardware configuration" section to match message
    and frequency generating device in the relevant .cpp and .h files.

    If no valid GPS signal, in the NMEA $GPRMC, is found then no PI4 is sent
    but only CW and a 20 s carrier.

    Hardware
        Arduino Uno/Nano or other platform with Timer1 and Timer2 if audio output
        GPS receiver with NMEA $GPRMC data that is active high
        Green LED showing valid GPS signal
        Yellow LED showing lock detect if relevant
        Red LED showing TX: PI4 at half rate, CW and carrier
        A device capable of making RF/AF. Supported devices:
                AD9833, AD9850, AD9851, AD9912, AD9913
                ADF4350, ADF4351, ADF5355, ADF5356
                LMX2541
                Radio soft-DDS
                RDDS
                Si5351A, Si570
                Check www.rudius.net/oz2m/pi4ino for latest devices

    Software
        Arduino IDE 1.6.4 on Windows 7 Pro 32
        Online PI4 tool: www.rudius.net/oz2m/ngnb/pi4encoding.php

    Version
        1.7.1
            Corrected loading of ADF5355 and ADF5356 thanks to Brian, GM8BJF and Bernie, GM4WZG

    Copyright
        Bo, OZ2M, www.rudius.net/oz2m/pi4ino
*/

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <SPI.h>
#include <Wire.h>

// Hardware pins for GPS and LEDs
#define gpsRX    6                                  // Serial data from GPS at pin 6 (connect to TX on the GPS)
#define gpsTX    7                                  // Serial data to GPS at pin 7 (connect to RX on the GPS), may not be needed but depends on the GPS receiver

#define ledGPS  A0                                  // Green valid GPS LED on pin A0
#define ledLock A1                                  // Yellow lock detect LED on pin A1, reserved
#define ledTX   A2                                  // Red "TX" LED on pin A2

// CW definitions
#define cwSpace  4                                  // The CW space or "no carrier" tone
#define cwMark   5                                  // The CW mark or carrier tone

// GPS
#include <SoftwareSerial.h>
SoftwareSerial gpsSerial(gpsRX, gpsTX);             // GPS serial port
int gpsDelayComp = 0;
byte gpsSecond = 59;
unsigned int gpsCheckTimeout = 60;  // GPS check timeout in seconds

// Variables
static volatile boolean done = true;
static byte symbolNo;
byte symbols[146];
byte callLength;
byte locatorLength;

// *** Beacon and hardware configuration ***************************************
//
// Change this block to match the messages, frequency device and GPS receiver
//                    12345678
char pi4Message[9] = "SR6LB   ";                    // The PI4 message. Must be 8 charaters, left justified and padded with spaces if needed

char call[] = "SR6LB";                               // The CW call. Only A-Z, 0-9 and /
char locator[] = "JO81CJ";                          // The CW locator. Only A-X, 0-9 and /

const int gpsBaudRate = 9600;                       // The GPS NMEA Baud rate for the $GPRMC data frame
const int gpsDataDelay = 50;                       // The GPS data frame delay in ms vs. real time


    // ADF4157
    const byte deviceUpdate = 3;          // The Ardunio pin where the device update is controlled, if used
    #include "ADF4157.h"
    ADF4157 Device(deviceUpdate);

// *** End of configuration ***********************************

// *** Morse functions ***************************************
byte CharToMorse(char ch)
{   // e.g. 0x42 = 01000010 = 01000 010 = .- 2 bits long = A
    const byte letters[] = {0x42, 0x84, 0xA4, 0x83, 0x01, 0x24, 0xC3, 0x04, 0x02, 0x74, 0xA3, 0x44, 0xC2, 0x82, 0xE3, 0x64, 0xD4, 0x43, 0x03, 0x81, 0x23, 0x14, 0x63, 0x94, 0xB4, 0xC4};
    const byte numbers[] = {0x95, 0xFD, 0x7D, 0x3D, 0x1D, 0x0D, 0x05, 0x85, 0xC5, 0xE5, 0xF5};

    if (ch == 0)
        return (0);                                 // Not a valid morse character
    else if (ch < 'A')                              // Get then Morse pattern
        return (numbers[ch - '/']);
    else
        return (letters[ch - 'A']);
}

void SendMorse(const char *info, const byte msgLen)
{
    byte i, j;
    byte morse;
    byte morseLength;

    for (i = 0; i < msgLen; i++)
    {
        morse = info[i];
        morseLength = morse & 0x07;                 // Bit2 to Bit0 of morse is the length

        for (j = 0; j < morseLength; j++)
        {
            Device.SetFrequency(cwMark);
            digitalWrite(ledTX, HIGH);              // Turn TX LED on

            if ((morse & 0x80) == 0x80)             // If MSB 0 = dot, 1 = dash,
                delay(300);                         // It is a dash, so wait 3 dot durations
            else
                delay(100);                         // It is a dot, so wait 1 dot duration

            Device.SetFrequency(cwSpace);
            digitalWrite(ledTX, LOW);               // Turn TX LED off
            delay(100);
            morse = morse << 1;                     // Point to next bit
        }
        delay(200);                                 // Inter morse character pause, 1 dot duration already "gone" so only 2 left
    }
}
// *** End of Mourse functions ********************************************

// *** PI4 functions and data *********************************************
//
//     Do NOT modify unless you really know what you are doing!
//
//                               _123456789_123456789_123456789_1234567
const char pi4Chars[] PROGMEM = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ /";
const byte pi4Vector[] PROGMEM =
{
    0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1,
    1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0,
    0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0,
    1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1,
    0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1
};

byte Parity(uint32_t value)
{
    byte even = 0;
    byte bitNo;

    for (bitNo = 0; bitNo < 32; bitNo++)
        if (((value >> bitNo) & 0x01) != 0)
            even = 1 - even;

    return even;
}

void PI4MakeMessage(char *msg)
{
    // Source encoding
    uint64_t sourceEnc = 0;

    for (byte i = 0; i < 8; i++)
        sourceEnc = sourceEnc * 38 + (uint64_t) (strchr_P(pi4Chars, msg[i]) - pi4Chars);

    // Convolutional Encoding
    const uint32_t poly1 = 0xF2D05351;
    const uint32_t poly2 = 0xE4613C47;
    uint32_t n = 0;
    byte convEnc[146] = {0};

    byte i = 0;
    for (byte j = 0; j < 73; j++)
    {
        n <<= 1;
        if ((sourceEnc & 0x20000000000LLU) != 0)
            n |= 1;
        sourceEnc <<= 1;

        convEnc[i++] = Parity(n & poly1);
        convEnc[i++] = Parity(n & poly2);
    }

    // Interleaving
    byte p = 0;
    byte r = 0;
    byte interleaved[146] = {0};

    for (int i = 0; i <= 255; i++)
    {
        for (byte j = 0; j < 8; j++)
        {
            if (((i >> j) & 0x01) == 0x01)
                r |= 1 << (7 - j);
            else
                r &= ~(1 << (7 - j));
        }

        if ((p < 146) && (r < 146))
            interleaved[r] = convEnc[p++];
    }

    //  Merge with Sync Vector
    for (byte i = 0; i < 146; i++)
        symbols[i] = pgm_read_byte(&pi4Vector[i]) + 2 * interleaved[i];
}
// *** End of PI4 encoding functions *****************************************

// *** GPS functions *********************************************************

byte WaitUntil59(const byte theSecond, const int theDelay, const unsigned int theTimeout) {
  boolean found = false;
  String r;
  unsigned int current_second = 0;
  unsigned long checkStart = millis();
  unsigned int internalTimeout = theTimeout * 1000;
  digitalWrite(ledGPS, LOW);  // Turn GPS valid off before checking status

  gpsSerial.begin(gpsBaudRate);

  while (current_second != theSecond) {
    if (millis() - checkStart > internalTimeout) { break; }  // stop if timeout reached
    if (gpsSerial.available() > 0) {
      r = gpsSerial.readStringUntil('\n');
      if (r.startsWith("$GPRMC")) {
        if (r.charAt(17) == 'A') {
          digitalWrite(ledGPS, HIGH);  // Turn GPS status on
          checkStart = millis();
        } else {
          digitalWrite(ledGPS, LOW);  // Turn GPS status on
        }
        current_second = r.substring(11, 13).toInt();
        Serial.println(current_second);
        if (current_second == theSecond) {
          found = true;
          delay(theDelay);
          break;
        }
      }
    }
  }
  gpsSerial.end();
  return found;
}

// *** End of GPS functions *********************************************

// *** Interrupt functions **********************************************

ISR(TIMER1_COMPA_vect)    // Timer1 compare interrupt service routine
{
    if (symbolNo < 146)
    {
        Device.SetFrequency(symbols[symbolNo]);     // Generate the relevant frequency
        digitalWrite(ledTX, !(symbolNo & 0x01));    // Toggle TX LED
        symbolNo++;                                 // Increment symbol counter for next time
    }
    else
        done = true;                                // Completed MGM sequence
}

void SendPI4()
{
    noInterrupts();                                 // Disable global interrupts
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;

    // 41667 is nominal value if xtal is 16,000000 MHz. Change value below to measure 3,00000 Hz on the A0 pin
    // OCR1A = 42000;                                  // Compare match register 16 MHz/64/6 Hz
    OCR1A = 41667;                                  // Compare match register 16 MHz/64/6 Hz
    TCCR1B = 0b00001011;                            // CTC mode and 64 prescaler
    TIMSK1 |= (1 << OCIE1A);                        // Enable timer compare interrupt

    symbolNo = 0;
    done = false;
    interrupts();                                   // Enable global interrupts

    while (!done);                                  // Loop until all symbols are sent

    TIMSK1 = 0;                                     // Stop interrupt Timer1 interrupt
}
// *** End of interrupt functions *****************************************

void setup()
{
    byte i;

    pinMode(ledTX, OUTPUT);
    digitalWrite(ledTX, LOW);
    pinMode(ledGPS, OUTPUT);
    digitalWrite(ledGPS, LOW);

    // Prepare PI4
    PI4MakeMessage(pi4Message);                     // Convert the PI4 message to symbols

    // Prepare CW call
    callLength = strlen(call);                      // Find the length of the CW call
    for (i = 0; i < callLength; i++)                // Convert call to Morse
        call[i] = CharToMorse(call[i]);

    // Prepare CW locator
    locatorLength = strlen(locator);                // Find the length of the CW locator
    for (i = 0; i < locatorLength; i++)             // Convert locator to Morse
        locator[i] = CharToMorse(locator[i]);

    // Frequency device setup
    Device.Initialize();                            // Initialize the device


    if (gpsDataDelay > 0)                           // Calc the GPS compensation delay and second
    {   // If any delay at all
        gpsSecond = 59 - (gpsDataDelay / 1000) - 1;
        gpsDelayComp = (int) 1000 - (gpsDataDelay % 1000);
    }
}

void loop()
{
    digitalWrite(ledTX, HIGH);
    Device.SetFrequency(cwMark);                    // Send carrier until first valid GPS $GPRMC frame received

    if (WaitUntil59(gpsSecond, gpsDelayComp, gpsCheckTimeout))       // Send only PI4 if GPS signal is valid
    {
        delay(500);
        Device.SetFrequency(cwSpace);               // Send "no carrier"
        digitalWrite(ledTX, LOW);
        delay(333);                                 // Is 500 ms but interrupt does not fire until another 167 ms

        SendPI4();
    }
    else
        delay(20000);                               // 20 s carrier if no valid GPS
    Device.SetFrequency(cwSpace);                   // Send "no carrier"
    digitalWrite(ledTX, LOW);
    delay(666);

    // Send CW call
    SendMorse(call, callLength);
    delay(500);                                     // Pause between CW call and locator

    // Send CW locator
    SendMorse(locator, locatorLength);
    delay(500);                                     // Pause between CW locator and carrier
}
