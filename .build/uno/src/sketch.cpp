#include <Arduino.h>
void setup();
void loop();
void updateEncoder();
void increment();
void decrement();
void display();
void setLights();
#line 1 "src/sketch.ino"
//From bildr article: http://bildr.org/2012/08/rotary-encoder-arduino/
#include <DoubleCounter.h>

//these pins can not be changed 2/3 are special pins
int encoderPin1 = 2;
int encoderPin2 = 3;

int buttonPin = 7;
int switchPin = 12;

const int serialPin        = 8;
const int registerClockPin = 9;
const int serialClockPin   = 10;

DoubleCounter counter(serialPin, registerClockPin, serialClockPin);

volatile int lastEncoded = 0;

int lights[] = {4, 5, 6};
int values[] = {1, 2, 3};
int mins[]   = {1, 2, 3};
int maxes[]  = {50, 70, 99};
int position = 0;

long lastencoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;

void setup() {
  Serial.begin (9600);
  counter.init();

  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(switchPin, INPUT);

  // turn pullup resistors on
  digitalWrite(encoderPin1, HIGH);
  digitalWrite(encoderPin2, HIGH);

  // Multiple (and read with division) by 4 since updateEncoder()
  // increments/decrements values by 4
  // Also set the pin mode for our indicators
  for (int i = 0; i < 3; i++) {
    values[i] *= 4;
    mins[i]   *= 4;
    maxes[i]  *= 4;

    pinMode(lights[i], OUTPUT);
  }

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);

}

void loop() {
  // if (digitalRead(switchPin) == LOW) {
  //   noInterrupts();
  //   counter.draw(88);
  // }
  // else {
  if (true) {
    // interrupts();
    display();

    if (digitalRead(buttonPin) == LOW) {
      position = (position + 1) % 3;
      display();
      delay(250);
    }
  }
}

void updateEncoder() {
  int MSB = digitalRead(encoderPin1);
  int LSB = digitalRead(encoderPin2);

  int encoded = (MSB << 1) | LSB;
  int sum  = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) increment();
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) decrement();

  lastEncoded = encoded;
}

void increment() {
  values[position] ++;
  if (values[position] > maxes[position]) {
    values[position] = maxes[position];
  }
}

void decrement() {
  values[position] --;
  if (values[position] < mins[position]) {
    values[position] = mins[position];
  }
}

void display() {
  counter.draw((values[position] / 4));
  setLights();
}

void setLights() {
  for (int i = 0; i < 3; i++) {
    if (i == position) {
      digitalWrite(lights[i], HIGH);
    }
    else {
      digitalWrite(lights[i], LOW);
    }
  }
}
