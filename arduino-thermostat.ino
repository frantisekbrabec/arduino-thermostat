/*
  Module for temperature meter with multiplexed 4digit LED display
  Arduino Mini operate different types of display with common A or C
  Common pins must be driven through transistor, segments through resisors
  Temperature is measured thanks DS18B20 on 6 pin and is displayed 100.0 C
  Use: MsTimer2, OneWire and DallasTemperature libraries
  
  This example code is in the public domain.
  S. Pechal 2018, Version: 1.0
 */
#include <OneWire.h>
#include <DallasTemperature.h>
#include <MsTimer2.h>
// You must define display type here
#define VQEX1
#include "leddispd.h"

// Define pin number for connected DS18B20
const int pinDS = 6;


const int pinRelay = 7;
const int tempLow = 15;
const int tempHigh = 20;

// OneWire instance for bus control
OneWire oneWireDS(pinDS);
// Dallas Temperature sensor instance
DallasTemperature sensDS(&oneWireDS);  

// Digit segments to display
unsigned char disparr[4];

// Table for chars '0'-'9','°','-','P','L','H',' '
unsigned char chartab[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x63,0x40,0x73,0x38,0x76,0x00};
// Sequence to switch digits off
unsigned char cleardig[4] = {COM4,COM1,COM2,COM3};
// Sequence to switch digits on
unsigned char ondigit[4] = {COM1,COM2,COM3,COM4};

// Set decimal point at position
void SetDP(unsigned char DPposit)
{
  if(DPposit < 4) disparr[DPposit] |= 0x80;
}

// Set first digit with minus
void Set1digit(unsigned char digit1, bool sign)
{
  if(sign) disparr[0] |= chartab[0x0B];
  if(digit1) disparr[0] |= chartab[digit1];
}

// Clear display
void ClrDisp(void)
{
  disparr[0] = 0; disparr[1] = 0; disparr[2] = 0; disparr[3] = 0;
}

// Integer to display
void IntToDisp(int dispnum)
{
  unsigned char fract;
  bool minussig=0;

  Serial.println(dispnum);

  ClrDisp();  
  if(dispnum > MAXDISVAL) {  // Test very high value - show "HI"
    disparr[1] = chartab[0x0E];
    disparr[2] = chartab[0x01];
    return;
  }
  if(dispnum < MINDISVAL) {  // Test very low value - show "LO"
    disparr[1] = chartab[0x0D];
    disparr[2] = chartab[0x00];
    return;
  }
  // Process negative numbers
  if(dispnum < 0) {
    dispnum= -dispnum;
    minussig=1;
  }
  // Calculate particular digits
  fract = dispnum % 10;
  disparr[3] = chartab[fract];
  dispnum /= 10;
  fract = dispnum % 10;
  disparr[2] = chartab[fract];
  dispnum /= 10;
  fract = dispnum % 10;
  disparr[1] = chartab[fract];
  dispnum /= 10;
  Set1digit(dispnum,minussig); // First digit is special case
}

// Interrupt procedure connected to Timer2  
void leddisp()
{
  static unsigned char digit=0;
  
  digitalWrite(cleardig[digit], TURNOFF);   // turn off old digit LED display
  // Set appropriate pins for new digit
  switch (digit) {
    case 0: {
      if(disparr[0] & 0x01) digitalWrite(SEG0A, TURNON); else digitalWrite(SEG0A, TURNOFF);
      if(disparr[0] & 0x02) digitalWrite(SEG0B, TURNON); else digitalWrite(SEG0B, TURNOFF);
      if(disparr[0] & 0x04) digitalWrite(SEG0C, TURNON); else digitalWrite(SEG0C, TURNOFF);
      if(disparr[0] & 0x08) digitalWrite(SEG0D, TURNON); else digitalWrite(SEG0D, TURNOFF);
      if(disparr[0] & 0x10) digitalWrite(SEG0E, TURNON); else digitalWrite(SEG0E, TURNOFF);
      if(disparr[0] & 0x20) digitalWrite(SEG0F, TURNON); else digitalWrite(SEG0F, TURNOFF);
      if(disparr[0] & 0x40) digitalWrite(SEG0G, TURNON); else digitalWrite(SEG0G, TURNOFF);
      if(disparr[0] & 0x80) digitalWrite(SEG0H, TURNON); else digitalWrite(SEG0H, TURNOFF);
    } break;
    case 1: {
      if(disparr[1] & 0x01) digitalWrite(SEG1A, TURNON); else digitalWrite(SEG1A, TURNOFF);
      if(disparr[1] & 0x02) digitalWrite(SEG1B, TURNON); else digitalWrite(SEG1B, TURNOFF);
      if(disparr[1] & 0x04) digitalWrite(SEG1C, TURNON); else digitalWrite(SEG1C, TURNOFF);
      if(disparr[1] & 0x08) digitalWrite(SEG1D, TURNON); else digitalWrite(SEG1D, TURNOFF);
      if(disparr[1] & 0x10) digitalWrite(SEG1E, TURNON); else digitalWrite(SEG1E, TURNOFF);
      if(disparr[1] & 0x20) digitalWrite(SEG1F, TURNON); else digitalWrite(SEG1F, TURNOFF);
      if(disparr[1] & 0x40) digitalWrite(SEG1G, TURNON); else digitalWrite(SEG1G, TURNOFF);
      if(disparr[1] & 0x80) digitalWrite(SEG1H, TURNON); else digitalWrite(SEG1H, TURNOFF);
    } break;
    case 2: {
      if(disparr[2] & 0x01) digitalWrite(SEG2A, TURNON); else digitalWrite(SEG2A, TURNOFF);
      if(disparr[2] & 0x02) digitalWrite(SEG2B, TURNON); else digitalWrite(SEG2B, TURNOFF);
      if(disparr[2] & 0x04) digitalWrite(SEG2C, TURNON); else digitalWrite(SEG2C, TURNOFF);
      if(disparr[2] & 0x08) digitalWrite(SEG2D, TURNON); else digitalWrite(SEG2D, TURNOFF);
      if(disparr[2] & 0x10) digitalWrite(SEG2E, TURNON); else digitalWrite(SEG2E, TURNOFF);
      if(disparr[2] & 0x20) digitalWrite(SEG2F, TURNON); else digitalWrite(SEG2F, TURNOFF);
      if(disparr[2] & 0x40) digitalWrite(SEG2G, TURNON); else digitalWrite(SEG2G, TURNOFF);
      if(disparr[2] & 0x80) digitalWrite(SEG2H, TURNON); else digitalWrite(SEG2H, TURNOFF);
    } break;
    case 3: {
      if(disparr[3] & 0x01) digitalWrite(SEG3A, TURNON); else digitalWrite(SEG3A, TURNOFF);
      if(disparr[3] & 0x02) digitalWrite(SEG3B, TURNON); else digitalWrite(SEG3B, TURNOFF);
      if(disparr[3] & 0x04) digitalWrite(SEG3C, TURNON); else digitalWrite(SEG3C, TURNOFF);
      if(disparr[3] & 0x08) digitalWrite(SEG3D, TURNON); else digitalWrite(SEG3D, TURNOFF);
      if(disparr[3] & 0x10) digitalWrite(SEG3E, TURNON); else digitalWrite(SEG3E, TURNOFF);
      if(disparr[3] & 0x20) digitalWrite(SEG3F, TURNON); else digitalWrite(SEG3F, TURNOFF);
      if(disparr[3] & 0x40) digitalWrite(SEG3G, TURNON); else digitalWrite(SEG3G, TURNOFF);
      if(disparr[3] & 0x80) digitalWrite(SEG3H, TURNON); else digitalWrite(SEG3H, TURNOFF);
    } break;
  }
  digitalWrite(ondigit[digit], TURNON);   // turn on new digit LED display
  // and pointer to next digit
  ++digit &= 0x03; 
} 

// the setup function runs once when you press reset or power the board
void setup() {

  Serial.begin(9600);
  Serial.println("Starting");
  
  // initialize digital pins for display driving
  pinMode(COM1, OUTPUT);
  pinMode(COM2, OUTPUT);
  pinMode(COM3, OUTPUT);
  pinMode(COM4, OUTPUT);
  pinMode(SEG0A, OUTPUT);
  pinMode(SEG0B, OUTPUT);
  pinMode(SEG0C, OUTPUT);
  pinMode(SEG0D, OUTPUT);
  pinMode(SEG0E, OUTPUT);
  pinMode(SEG0F, OUTPUT);
  pinMode(SEG0G, OUTPUT);
  pinMode(SEG0H, OUTPUT);
  // Initialize Timer2 for 4 ms period
  MsTimer2::set(4, leddisp); // 4 ms period
  MsTimer2::start();
  // Start sensor communication
  sensDS.begin();
}

// *****************************************************
// Main loop - measure temerature on pinDS pin
unsigned int value, sigpoint;
float tempval;
// The loop function runs over and over again forever
void loop() {
  // Read the value from the temperture sensor(s)
  sensDS.requestTemperatures();
  // Get temperature value
  tempval=sensDS.getTempCByIndex(0);

  if (tempval != DEVICE_DISCONNECTED_C)
  {
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(tempval);

    if (tempval < tempLow)
      digitalWrite(pinRelay, TURNOFF);
    if (tempval > tempHigh)
      digitalWrite(pinRelay, TURNON);

  }
  else
  {
    Serial.println("Error: Could not read temperature data");
  }

  
  value = (int)(tempval * 10);
  // Display value on display
  IntToDisp(value);
  // Set decimal point position
  SetDP(2);
  // Change last digit to degree symbol
  ++sigpoint &= 0x0001;
  if(sigpoint) disparr[3] = chartab[0x0A];
  delay(300);
}
