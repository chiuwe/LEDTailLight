#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#define NUM_LEDS 97
#define BRIGHTNESS 255
#define PIN 10
#define SPEED_DELAY 10
#define RETURN_DELAY 50
#define EYE_SIZE 5
#define DEBUG 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

const byte leftPin = 2;
const byte rightPin = 3;
const byte hazardPin = 4;
const byte reversePin = 5;
const byte runningPin = 6;
const byte strobePin = 7;

volatile byte leftState = LOW;
volatile byte rightState = LOW;
volatile byte hazardState = LOW;
volatile byte reverseState = LOW;
volatile byte runningState = LOW;
volatile byte strobeState = LOW;

enum Signal {OFF, RUNNING, HAZARD, LEFT, RIGHT, STROBE, REVERSE};
Signal currentState;

void setup() {
  if(DEBUG) {Serial.begin(9600);}
  currentState = OFF;
  pinMode(leftPin, INPUT_PULLUP);
  pinMode(rightPin, INPUT_PULLUP);
  pinMode(hazardPin, INPUT_PULLUP);
  pinMode(reversePin, INPUT_PULLUP);
  pinMode(runningPin, INPUT_PULLUP);
  pinMode(strobePin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(leftPin), leftTurn, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rightPin), rightTurn, CHANGE);
  attachInterrupt(digitalPinToInterrupt(hazardPin), hazard, CHANGE);
  attachInterrupt(digitalPinToInterrupt(reversePin), reverseLights, CHANGE);
  attachInterrupt(digitalPinToInterrupt(runningPin), runningLights, CHANGE);
  attachInterrupt(digitalPinToInterrupt(strobePin), strobeLights, CHANGE);
  leftState = digitalRead(leftPin);
  rightState = digitalRead(rightPin);
  hazardState = digitalRead(hazardPin);
  reverseState = digitalRead(reversePin);
  runningState = digitalRead(runningPin);
  strobeState = digitalRead(strobePin);
  strip.begin();
  strip.show();
}

void loop() {
  switch(currentState) {
    case LEFT:
      rightToLeft(0xff, 0, 0, EYE_SIZE, SPEED_DELAY, RETURN_DELAY);
      break;
    case RIGHT:
      leftToRight(0xff, 0, 0, EYE_SIZE, SPEED_DELAY, RETURN_DELAY);
      break;
    case HAZARD:
      centerToOutside(0xff, 0, 0, EYE_SIZE, SPEED_DELAY * 2, RETURN_DELAY);
      break;
    case RUNNING:
      setAll(0xff/4, 0, 0);
      break;
    case REVERSE:
      setAll(0xff, 0xff, 0xff);
      break;
    case STROBE:
      while(strobeState) {
        // create some sort of strobe pattern 
      }
      break;
    case OFF:
      setAll(0, 0, 0);
      break;
    default:
      setAll(0, 0, 0);
      break;
  }
}

//Hazard Out Pattern
void centerToOutside(byte red, byte green, byte blue, int eyeSize, int speedDelay, int returnDelay) {
  for(int i =((NUM_LEDS-eyeSize)/2); i >= -eyeSize; i--) {
    setAll(0,0,0);
    
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= eyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+eyeSize+1, red/10, green/10, blue/10);
    
    setPixel(NUM_LEDS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= eyeSize; j++) {
      setPixel(NUM_LEDS-i-j, red, green, blue); 
    }
    setPixel(NUM_LEDS-i-eyeSize-1, red/10, green/10, blue/10);
    
    showStrip();
    delay(speedDelay);
  }
  delay(returnDelay);
}

// Hazard In Pattern
void outsideToCenter(byte red, byte green, byte blue, int eyeSize, int speedDelay, int returnDelay) {
  for(int i = -eyeSize; i<=((NUM_LEDS-eyeSize)/2); i++) {
    setAll(0,0,0);
    
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= eyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+eyeSize+1, red/10, green/10, blue/10);
    
    setPixel(NUM_LEDS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= eyeSize; j++) {
      setPixel(NUM_LEDS-i-j, red, green, blue); 
    }
    setPixel(NUM_LEDS-i-eyeSize-1, red/10, green/10, blue/10);
    
    showStrip();
    delay(speedDelay);
  }
  delay(returnDelay);
}

// Right Turn Signal Pattern
void leftToRight(byte red, byte green, byte blue, int eyeSize, int speedDelay, int returnDelay) {
  for(int i = -eyeSize; i < NUM_LEDS+eyeSize; i++) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= eyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+eyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(speedDelay);
  }
  delay(returnDelay);
}

// Left Turn Signal Pattern
void rightToLeft(byte red, byte green, byte blue, int eyeSize, int speedDelay, int returnDelay) {
  for(int i = NUM_LEDS+eyeSize; i > -eyeSize; i--) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= eyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+eyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(speedDelay);
  }
  delay(returnDelay);
}

// reverse > strobe > left/right > hazard > running > off
void updateSignalState(Signal newState) {
  if(reverseState == LOW) {
    if(strobeState == LOW) {
      if(newState == REVERSE || newState == STROBE) {
        currentState = newState;
      } else {
        if(newState == OFF) {
          // check if any other state is set (left/right/hazard/running)
          if(leftState == HIGH || rightState == HIGH) {
            currentState = (leftState == HIGH) ? LEFT : RIGHT;
          } else if(hazardState == HIGH) {
            currentState = HAZARD;
          } else if(runningState == HIGH) {
            currentState = RUNNING;
          } else {
            currentState = newState;
          }
        } else {
          // check if newState has precedence over currentState
          if(newState > currentState) {
            currentState = newState;
          }
        }
      }
    }
  }
}

void leftTurn() {
  leftState = !leftState;
  updateSignalState((leftState == HIGH) ? LEFT : OFF);
}

void rightTurn() {
  rightState = !rightState;
  updateSignalState((rightState == HIGH) ? RIGHT : OFF);
}

void hazard() {
  hazardState = !hazardState;
  updateSignalState((hazardState == HIGH) ? HAZARD : OFF);
}

void reverseLights() {
  reverseState = !reverseState;
  updateSignalState((reverseState == HIGH) ? REVERSE : OFF);
}

void runningLights() {
  runningState = !runningState;
  updateSignalState((runningState == HIGH) ? RUNNING : OFF);
}

void strobeLights() {
  strobeState = !strobeState;
  updateSignalState((strobeState == HIGH) ? STROBE : OFF);
}

// *******************************
// ** NeoPixel Common Functions **
// *******************************

// Apply LED color changes
void showStrip() {
   strip.show();
}

// Set a LED color (not yet visible)
void setPixel(int Pixel, byte red, byte green, byte blue) {
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
}

// Set all LEDs to a given color and apply it (visible)
void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}
