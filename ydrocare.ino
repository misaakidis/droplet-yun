/*
  ydrocare

  Residential water consumption monitor with Arduino Yun.

  Hardware:
    * Arduino Yun board (connected via Ethernet to the Internet)
    * Hall Effect Sensor A 42E

  The circuit:
    * Vcc of the hall effect sensor to +3.3V
    * GND of the hall effect sensor to ground
    * OUTPUT of the hall effect sensor to digital pin 12
    * 220 Ohm transistor between Vcc and Output of the sensor

  created 17 Apr 2014
  by Marios Isaakidis
  team YdroCare

  https://misaakidis.github.io/blog/2014/04/water-arduino.html

  This example code is in the public domain.

 */

#include <Process.h>


const int led = 13;        // on-board led of Arduino Yun
const int hall_input = 12;  // output of the hall effect sensor

const int UID = 100;

int peaks = 0;
unsigned long epoch;
char epochCharArray[25] = "";

Process time;

void setup() {
  // Initialize Bridge
  Bridge.begin();

  // Initialize serial communications:
  Serial.begin(9600);
  
  // Initialize the on-board led as output and the pin connected
  // to the hall effect sensor as input:
  pinMode(led, OUTPUT);
  pinMode(hall_input, INPUT);

  // Wait for serial to be connected, remove this in the future
  while (!Serial);
  
  // Initialize time from Linino in Unix timestamp format
  epoch = timeInEpoch();
}

void loop() {
  delay(200);
  Serial.println(epoch + (millis() / 1000));
}



unsigned long timeInEpoch() {
  if (!time.running())  {
    time.begin("date");
    time.addParameter("+%s");
    time.run();
  }
  
  while (time.available() > 0) {
    time.readString().toCharArray(epochCharArray, 25);
  }
  
  return atol(epochCharArray);
}
