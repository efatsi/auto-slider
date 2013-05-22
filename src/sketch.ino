//From bildr article: http://bildr.org/2012/08/rotary-encoder-arduino/
#include <DoubleCounter.h>
#include <Counter.h>

//these pins can not be changed 2/3 are special pins
int encoderPin1 = 2;
int encoderPin2 = 3;
int encoderSwitchPin = 7; //push button switch

const int serialPin        = 8;
const int registerClockPin = 9;
const int serialClockPin   = 10;

DoubleCounter counter(serialPin, registerClockPin, serialClockPin);

volatile int lastEncoded = 0;

int values[] = {1, 2, 3};
int mins[]   = {1, 2, 3};
int maxes[]  = {11, 22, 99};
int position = 0;

long lastencoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;

void setup() {
  Serial.begin (9600);
  counter.init();

  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);
  pinMode(encoderSwitchPin, INPUT);

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

  // Multiple (and read with division) by 4 since updateEncoder()
  // increments/decrements values by 4
  for (int i= 0; i < 3; i++) {
    values[i] *= 4;
    mins[i]   *= 4;
    maxes[i]  *= 4;
  }

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);

}

void loop() {
  int buttonState = digitalRead(encoderSwitchPin);
  if (buttonState == 1) {
    counter.draw((values[position] / 4));
  }
  if (buttonState == 0) {
    position = (position + 1) % 3;
    counter.draw((values[position] / 4));
    delay(500);
  }

}

void updateEncoder() {
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) increment();
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) decrement();

  lastEncoded = encoded; //store this value for next time
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
