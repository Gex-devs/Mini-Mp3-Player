#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"
#include <DFPlayerMini_Fast.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <SoftwareSerial.h>
#include <Bounce2.h>

SoftwareSerial SoftSerial(10, 11);

// Declared functions
void printDetail(uint8_t type, int value);
void initMiniPlayer();
void initMiniDisplay();
void PlayerUiHandler();
void ControllerInit();
void ControlHandler(uint8_t EncoderA, uint8_t EncoderB, uint8_t PushBtn);

// Rotary Encoder pins
uint8_t RPinA = 3;
uint8_t RPinB = 4;
uint8_t RotaryPushBtn = 7;

// Tft Display Pin
uint8_t TFT_CS = A5;
uint8_t TFT_DC = A4;
uint8_t TFT_RST = A3;

#define TFT_CS A5
#define TFT_RST A3
#define TFT_DC A4

#define TFT_SCLK A1
#define TFT_MOSI A2

// 5v Pin
uint8_t Vout = 2;

DFPlayerMini_Fast Thedfplayer;
DFRobotDFPlayerMini myDFPlayer;
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// button handler
// Bounce debouncer = Bounce();
bool singleClickDetected = false;
bool doubleClickDetected = false;
bool longHoldDetected = false;
unsigned long lastClickTime = 0;

const int debounceInterval = 10;  
const int longHoldDuration = 1000;

void setup()
{
  Serial.begin(9600);
  SoftSerial.begin(9600);
  Serial.println("Mini Player Starting...");
  initMiniPlayer();
  initMiniDisplay();
  // ControllerInit();
}

void loop()
{

  ControlHandler(RPinA, RPinB, RotaryPushBtn);

  // PlayerUiHandler();
}

void initPins()
{

  // TODO: Assign all pins respectively using Registers if possible

  pinMode(RPinA, INPUT);
  pinMode(RPinB, INPUT);
  pinMode(RotaryPushBtn, INPUT_PULLUP);

  pinMode(Vout, OUTPUT);
}

void initMiniPlayer()
{
  // Enable loop

  if (!Thedfplayer.begin(SoftSerial, true))
  { // Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    // while (true)
    // {
    //   delay(0); // Code to compatible with ESP8266 watch dog.
    // }
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.enableLoopAll();

  Thedfplayer.volume(5);
  Thedfplayer.play(1);
}

void PlayerUiHandler()
{
  static int currentSdTrack = Thedfplayer.currentSdTrack();
  static int currentnumSdTracks = Thedfplayer.numSdTracks();
  static int currentVolume = Thedfplayer.currentVolume();

  if (currentSdTrack != Thedfplayer.currentSdTrack())
  {

    currentSdTrack = Thedfplayer.currentSdTrack();
    // Display current song information
    tft.setCursor(10, 10);
    tft.print("Song: ");
    tft.print(currentnumSdTracks);
  }

  if (currentnumSdTracks != Thedfplayer.numSdTracks())
  {
    currentnumSdTracks = Thedfplayer.numSdTracks();
    // Display total songs information
    tft.setCursor(10, 40);
    tft.print("Total: ");
    tft.print(currentnumSdTracks);
  }

  if (currentVolume != Thedfplayer.currentVolume())
  {
    currentVolume = Thedfplayer.currentVolume();
    // Display volume information
    tft.setCursor(10, 70);
    tft.print("Volume: ");
    tft.print(currentVolume);
  }
}

// void ControllerInit()
// {
//   debouncer.attach(RotaryPushBtn);
//   debouncer.interval(debounceInterval);
// }

// Rotary Encoder Input Handler
void ControlHandler(uint8_t EncoderA, uint8_t EncoderB, uint8_t PushBtn)
{

  static int pinALast = digitalRead(EncoderA);
  static int aVal;

  aVal = digitalRead(EncoderA);

  if (aVal != pinALast)
  { // Means the knob is rotating
    // if the knob is rotating, we need to determine direction
    // We do that by reading pin B.
    if (digitalRead(EncoderB) != aVal)
    { // Means pin A Changed first - We're Rotating Clockwise
      Thedfplayer.incVolume();
      Serial.println("Volume Increased");
    }
    else
    { // Otherwise B changed first and we're moving CCW
      Thedfplayer.decVolume();
      Serial.println("Volume Decreased");
    }
  }
  pinALast = aVal;

  // button handler
  // debouncer.update();
  // unsigned long currentMillis = millis();

  // if (debouncer.fell()) {
  //   // Button pressed, check for single-click or double-click
  //   if ((currentMillis - lastClickTime) < 250) {
  //     // Double-click detected
  //     Serial.println("Double Click Detected");
  //     doubleClickDetected = true;
  //     Thedfplayer.playNext();
  //   } else {
  //     // Single-click detected, but wait to see if it's a double-click
  //     singleClickDetected = true;
  //     lastClickTime = currentMillis;
  //   }
  // }

  // if (debouncer.rose() && singleClickDetected) {
  //   // Button released, and it was a single-click
  //   Serial.println("Single Click Detected");
  //   singleClickDetected = false;  // Reset for the next click
  //   Thedfplayer.resume();

  // }

  // // Check for long-hold
  // if (debouncer.read() == LOW && !longHoldDetected) {
  //   if ((currentMillis - lastClickTime) >= longHoldDuration) {
  //     // Long-hold detected
  //     Serial.println("Long Hold Detected");
  //     longHoldDetected = true;
  //     Thedfplayer.pause();
  //   }
  // }

  // // Check for long-hold release
  // if (debouncer.rose() && longHoldDetected) {
  //   Serial.println("Long Hold Released");
  //   longHoldDetected = false;  // Reset for the next long-hold
  // }
}

void initMiniDisplay()
{

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK); // Clear the screen

  tft.setRotation(3);

  tft.setCursor(10, 10);

  tft.setTextSize(1);

  tft.println("Gex mini Player");

  tft.setCursor(10, 50);

  tft.setTextSize(1);

  tft.print("Current Version: 1.0");
}