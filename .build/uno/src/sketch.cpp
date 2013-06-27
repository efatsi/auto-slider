#include <Arduino.h>
void setup();
void loop();
void updateEncoder();
void increment();
void decrement();
void display();
void setLights();
void moveIt();
void step();
int distanceInterval();
void wait();
void turnOnGreenLight();
#line 1 "src/sketch.ino"
#include <DoubleCounter.h>
#include <Servo.h>

const int serialPin        = 8;
const int registerClockPin = 9;
const int serialClockPin   = 10;

const int servoPin = 5;

DoubleCounter counter(serialPin, registerClockPin, serialClockPin);
Servo servo;

int encoderPin1 = 2;
int encoderPin2 = 3;

int buttonPin = 6;
int switchPin = 7;

volatile int lastEncoded = 0;
long lastencoderValue    = 0;

int lastMSB = 0;
int lastLSB = 0;

// timeInterval, distanceInterval, reverse
int lights[] = {12, 11, 4};
int values[] = {1, 1, 0};
int mins[]   = {1, 1, 0};
int maxes[]  = {99, 10, 1};
int position = 0;

long stepStart;

int greenLight = 13;

void setup() {
  counter.init();

  servo.attach(servoPin);

  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(switchPin, INPUT);

  digitalWrite(encoderPin1, HIGH);
  digitalWrite(encoderPin2, HIGH);

  for (int i = 0; i < 3; i++) {
    // Multiply by 4 since updateEncoder()
    // increments/decrements values by 4
    // (read with division)
    values[i] *= 4;
    mins[i]   *= 4;
    maxes[i]  *= 4;

    pinMode(lights[i], OUTPUT);
  }

  pinMode(greenLight, OUTPUT);

  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);

}

void loop() {
  if (digitalRead(switchPin) == HIGH) {
    turnOnGreenLight();
    moveIt();
    digitalWrite(greenLight, LOW);
  }
  else {
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

void moveIt() {
  while (digitalRead(switchPin) == HIGH) {
    stepStart = millis();
    step();
    wait();
  }
}

void step() {
  int reverse = (values[2] / 4);
  if (reverse == 0) {
    servo.write(92 + distanceInterval());
  }
  else {
    servo.write(93 - distanceInterval());
  }
  delay(400);
  servo.write(93);
}

int distanceInterval() {
  return 5 + (values[1] / 4);
}

void wait() {
  while(millis() - stepStart < (values[0] / 4) * 1000) {}
}

void turnOnGreenLight() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(lights[i], LOW);
  }
  digitalWrite(greenLight, HIGH);
}
