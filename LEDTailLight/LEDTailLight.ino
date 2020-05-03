#include "FastLED.h"
#include <EEPROM.h>

#define NUM_LEDS 97
#define BRIGHTNESS 255
#define PIN 10
#define DELAY 10
#define EYE_SIZE 5
#define DEBUG 1

CRGB leds[NUM_LEDS];

const byte leftPin = 2;
const byte rightPin = 3;
const byte hazardPin = 4;
const byte reversePin = 5;
const byte runningPin = 6;
const byte strobePin = 7;

volatile byte reverseState = LOW;
volatile byte runningState = LOW;
volatile byte strobeState = LOW;

void setup()
{
  if(DEBUG) {Serial.begin(9600);}
  pinMode(leftPin, INPUT_PULLUP);
  pinMode(rightPin, INPUT_PULLUP);
  pinMode(hazardPin, INPUT_PULLUP);
  pinMode(reversePin, INPUT_PULLUP);
  pinMode(runningPin, INPUT_PULLUP);
  pinMode(strobePin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(leftPin), leftTurn, RISING);
  attachInterrupt(digitalPinToInterrupt(rightPin), rightTurn, RISING);
  attachInterrupt(digitalPinToInterrupt(hazardPin), hazard, RISING);
  attachInterrupt(digitalPinToInterrupt(reversePin), reverseLights, CHANGE);
  attachInterrupt(digitalPinToInterrupt(runningPin), runningLights, CHANGE);
  attachInterrupt(digitalPinToInterrupt(strobePin), strobeLights, CHANGE);
  reverseState = digitalRead(reversePin);
  runningState = digitalRead(runningPin);
  strobeState = digitalRead(strobePin);
  FastLED.addLeds<WS2811, PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  if(reverseState == HIGH) {
    setAll(0xff, 0xff, 0xff);
  } else {
    if(runningState == LOW) {
      setAll(0,0,0);
    } else {
      setAll(0xff/4, 0, 0);
    }
  }
  while(strobeState) {
   // create some sort of strobe pattern 
  }
}

//Hazard Out Pattern
void centerToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay) {
  for(int i =((NUM_LEDS-EyeSize)/2); i >= -EyeSize; i--) {
    setAll(0,0,0);
    
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    
    setPixel(NUM_LEDS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS-i-j, red, green, blue); 
    }
    setPixel(NUM_LEDS-i-EyeSize-1, red/10, green/10, blue/10);
    
    showStrip();
    delay(SpeedDelay);
  }
  delay(SpeedDelay);
}

// Hazard In Pattern
void outsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay) {
  for(int i = -EyeSize; i<=((NUM_LEDS-EyeSize)/2); i++) {
    setAll(0,0,0);
    
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    
    setPixel(NUM_LEDS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS-i-j, red, green, blue); 
    }
    setPixel(NUM_LEDS-i-EyeSize-1, red/10, green/10, blue/10);
    
    showStrip();
    delay(SpeedDelay);
  }
  delay(SpeedDelay);
}

// Right Turn Signal Pattern
void leftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay) {
  for(int i = -EyeSize; i < NUM_LEDS+EyeSize; i++) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(SpeedDelay);
}

// Left Turn Signal Pattern
void rightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay) {
  for(int i = NUM_LEDS+EyeSize; i > -EyeSize; i--) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(SpeedDelay);
}

void leftTurn() {
  rightToLeft(0xff, 0, 0, EYE_SIZE, DELAY);
}

void rightTurn() {
  rightToLeft(0xff, 0, 0, EYE_SIZE, DELAY);
}

void hazard() {
  centerToOutside(0xff, 0, 0, EYE_SIZE, DELAY*2);
}

//Reverse Lights
void reverseLights() {
  reverseState = !reverseState;
}

void runningLights() {
  runningState = !runningState;
  if(DEBUG) {
    Serial.print("running state: ");
    Serial.println(runningState);
  }
}

void strobeLights() {
  strobeState = !strobeState;
}

// ******************************
// ** FastLed Common Functions **
// ******************************

// Apply LED color changes
void showStrip() {
   FastLED.show();
}

// Set a LED color (not yet visible)
void setPixel(int Pixel, byte red, byte green, byte blue) {
   if(Pixel > -1 & Pixel < NUM_LEDS) {
      leds[Pixel].r = red;
      leds[Pixel].g = green;
      leds[Pixel].b = blue;
   }
}

// Set all LEDs to a given color and apply it (visible)
void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}
