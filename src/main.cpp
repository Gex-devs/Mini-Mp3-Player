#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>



//Rotary Encoder pins
uint8_t RPinA = 23;
uint8_t RPinB = 24;
uint8_t RotaryPushBtn = 25;

// Tft Display Pin
uint8_t TFT_CS = ;
uint8_t TFT_DC = ;
uint8_t TFT_RST = ;

// PowerOutPin
uint8_t Vout = 26;


DFRobotDFPlayerMini myDFPlayer;
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int Setup()
{

  Serial.begin(115200);
  Serial.println("Mini Player Starting...");
}

void loop()
{


}

void initPins()
{

  // TODO: Assign all pins respectively using Registers if possible

  pinMode(RPinA,INPUT);
  pinMode(RPinB,INPUT);
  pinMode(RotaryPushBtn,INPUT);


  pinMode(Vout,OUTPUT);

}


void initMiniPlayer()
{
  if (!myDFPlayer.begin(Serial, /*isACK = */ true, /*doReset = */ true))
  { // Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true)
    {
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(10); // Set volume value. From 0 to 30
  // myDFPlayer.play(1);    // Play the first mp3
}

void MiniPlayerHanlder()
{
  if (myDFPlayer.available())
  {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); // Print the detail message from DFPlayer to handle different errors and states.
  }
}

void printDetail(uint8_t type, int value)
{
  switch (type)
  {
  case TimeOut:
    Serial.println(F("Time Out!"));
    break;
  case WrongStack:
    Serial.println(F("Stack Wrong!"));
    break;
  case DFPlayerCardInserted:
    Serial.println(F("Card Inserted!"));
    break;
  case DFPlayerCardRemoved:
    Serial.println(F("Card Removed!"));
    break;
  case DFPlayerCardOnline:
    Serial.println(F("Card Online!"));
    break;
  case DFPlayerUSBInserted:
    Serial.println("USB Inserted!");
    break;
  case DFPlayerUSBRemoved:
    Serial.println("USB Removed!");
    break;
  case DFPlayerPlayFinished:
    Serial.print(F("Number:"));
    Serial.print(value);
    Serial.println(F(" Play Finished!"));
    break;
  case DFPlayerError:
    Serial.print(F("DFPlayerError:"));
    switch (value)
    {
    case Busy:
      Serial.println(F("Card not found"));
      break;
    case Sleeping:
      Serial.println(F("Sleeping"));
      break;
    case SerialWrongStack:
      Serial.println(F("Get Wrong Stack"));
      break;
    case CheckSumNotMatch:
      Serial.println(F("Check Sum Not Match"));
      break;
    case FileIndexOut:
      Serial.println(F("File Index Out of Bound"));
      break;
    case FileMismatch:
      Serial.println(F("Cannot Find File"));
      break;
    case Advertise:
      Serial.println(F("In Advertise"));
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

// Rotary Encoder Input Handler
void ControlHandler(uint8_t EncoderA, uint8_t EncoderB, uint8_t PushBtn)
{

  /*Encoder A = CLK
    Encoder B = DT
    PushBtn = SW
  */

  static unsigned long DebounceTime = 0;
  static unsigned int LastBtnRead = digitalRead(PushBtn);

  static int pinALast = digitalRead(EncoderA); // Assigned Here only once

  int aVal = digitalRead(EncoderA);

  if (aVal != pinALast)
  { // Means the knob is rotating
    // if the knob is rotating, we need to determine direction
    // We do that by reading pin B.
    if (digitalRead(EncoderB) != aVal)
    {
      // Means pin A Changed first - We're Rotating Clockwise
      myDFPlayer.volumeUp();
    }
    else
    { // Otherwise B changed first and we're moving CCW

      myDFPlayer.volumeDown();
    }
  }
  pinALast = aVal;

  if (digitalRead(PushBtn) != LastBtnRead)
  {
    DebounceTime = millis();

    if (DebounceTime - millis() > 3000)
    {
      if (PushBtn == LOW)
      {
        // TODO: find out the state and change accordingly from pause to play
        myDFPlayer.readState();
      }
    }
  }

  LastBtnRead = digitalRead(PushBtn);
}


int initMiniDisplay(){

  tft.initR(INITR_MINI160x80);

  Serial.println(F("Initialized Mini Display"));

  tft.fillScreen(ST77XX_BLACK);


  // Test Purpose

  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextWrap(true);
  tft.setTextSize(8);
  tft.setRotation(-45);
  tft.print("Gex");
}