/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

// Pin assignment 
byte outPin1 = 13;
byte outPin2 = 12;
byte intNumber1 = 0;
byte intNumber2 = 1;

// Variables
int pulse1Length = 100;  // 10 * 100uS
int pulse2Length = 100;  // 10 * 100uS
int counter1;
int counter2;

boolean pulsing1 = false;
boolean pulsing2 = false;

// Interrupt routines
void pulse1() {
  digitalWrite(outPin1, HIGH);
  counter1 = 0;
  pulsing1 = true;
}

void pulse2() {
  digitalWrite(outPin2, HIGH);
  counter2 = 0;
  pulsing2 = true;
}

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(outPin1, OUTPUT);
  attachInterrupt(intNumber1, pulse1, RISING);
  pinMode(outPin2, OUTPUT);
  attachInterrupt(intNumber2, pulse2, RISING);
}

// the loop routine runs over and over again forever:
void loop() {
  delayMicroseconds(100);

  if (pulsing1) {
    counter1++;
    if (counter1 > pulse1Length) {
      digitalWrite(outPin1, LOW);
      pulsing1 = false;
    }
  }

  if (pulsing2) {
    counter2++;
    if (counter2 > pulse2Length) {
      digitalWrite(outPin2, LOW);
      pulsing2 = false;
    }
  }
}
