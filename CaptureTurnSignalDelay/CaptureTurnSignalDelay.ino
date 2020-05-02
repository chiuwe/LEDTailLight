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
  // put your main code here, to run repeatedly:

}

void singal() {
  signalState = !signalState;
  if(signalState == LOW) {
    elapsedTime = millis() - startTime;
    Serial.println((int)(elapsedTime/1000L));
  } else {
    startTime = millis();
  }
}
