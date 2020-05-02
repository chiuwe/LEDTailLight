long startTime;
long elapsedTime;

const byte signalPin = 2;

volatile byte signalState = LOW;

void setup() {
  Serial.begin(9600);
  pinMode(signalPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(signalPin), signal, CHANGE);
}

void loop() {
}

void signal() {
  signalState = !signalState;
  if(signalState == LOW) {
    elapsedTime = millis() - startTime;
    if(elapsedTime > 1) {
      Serial.println(elapsedTime);
    }
  } else {
    startTime = millis();
  }
}
