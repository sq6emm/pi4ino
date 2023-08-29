/*
    Arduino program to print the default Si570 registers
    and deduce HS_DIV, N1 and RFREQ.

    Hardware
        Arduino Uno/Nano using I2C pins A4 (SDA) and A5 (SCL)
  
    Software
        Arduino IDE 1.0.6
  
    Version
        1.0.0
  
    Copyright
        Bo, OZ2M, www.rudius.net/oz2m/pi4ino
*/

#include <Wire.h>
#include <Arduino.h>

byte WriteRegister(byte regAdr, byte value)
{
    Wire.beginTransmission(0x55);
    Wire.write(regAdr);
    Wire.write(value);
    Wire.endTransmission();
}

byte ReadRegister(byte regAdr)
{
    byte result;

    Wire.beginTransmission(0x55);
    Wire.write(regAdr);
    Wire.endTransmission(false);
    Wire.requestFrom((byte) 0x55, (byte) 1);
    result = Wire.read();
    Wire.endTransmission();
    
    return(result);
}

void Print_HS_DIV(byte startAdr)
{
    byte regVal;
    char buffer[30];
    
    regVal = ReadRegister(startAdr);
    sprintf(buffer, "HS_DIV      : %d", (regVal >> 5) + 4);
    Serial.println(buffer);
}

void Print_N1(byte startAdr)
{
    byte regVal;
    char buffer[30];    
    
    regVal = ((ReadRegister(startAdr) & 0x1F) << 2) + (ReadRegister(startAdr + 1) >> 6) + 1;
    sprintf(buffer, "N1          : %d", regVal);
    Serial.println(buffer);
}

void Print_RFREQ(byte startAdr)
{
    uint64_t RFREQ;
    char buffer[30];    
    
    RFREQ = (ReadRegister(startAdr) & 0x3F) << 8;
    RFREQ = (RFREQ + ReadRegister(startAdr + 1)) << 8;
    RFREQ = (RFREQ + ReadRegister(startAdr + 2)) << 8;
    RFREQ = (RFREQ + ReadRegister(startAdr + 3)) << 8;
    RFREQ += ReadRegister(startAdr + 4);
    sprintf(buffer, "RFREQ * 2^28: %lu", RFREQ / 1000000LU); 
    Serial.print(buffer);  
    sprintf(buffer, "%06lu ", RFREQ % 1000000LU); 
    Serial.println(buffer);
    
    sprintf(buffer, "            : %02X %02X %02X %02X %02X", ReadRegister(startAdr) & 0x3F, ReadRegister(startAdr + 1), ReadRegister(startAdr + 2), ReadRegister(startAdr + 3), ReadRegister(startAdr + 4));
    Serial.println(buffer);
}

void PrintRegisters()
{
    char buffer[30];

    Serial.println("Register      Value");
    Serial.println("-------------------");
    for (int i = 7; i < 19; i++)
    {
        sprintf(buffer, "%03d         : %02X", i, ReadRegister(i));
        Serial.println(buffer);
    }
    
    sprintf(buffer, "%03d         : %02X", 135, ReadRegister(135));
    Serial.println(buffer);
    sprintf(buffer, "%03d         : %02X", 137, ReadRegister(137));
    Serial.println(buffer);
    
    Serial.println("\n20 PPM and 50 PPM devices");
    Serial.println("-------------------------");
    
    Print_HS_DIV(7);
    Print_N1(7);
    Print_RFREQ(8);

    Serial.println("\n7 PPM devices");
    Serial.println("-------------");

    Print_HS_DIV(13);
    Print_N1(13);
    Print_RFREQ(14);
}

void setup()
{
    Serial.begin(9600);    
    Serial.println("\n\n* * *  Si570 register dump  * * *");
    Serial.println("by Bo, OZ2M, www.rudius.net/oz2m\n");

    Wire.begin();
    
    WriteRegister(135, 1);  // Load defaults into registers

    PrintRegisters();
}

void loop()
{
}
