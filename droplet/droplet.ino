/*

  ********************************************************************************
  Copyright (C) 2014 Isaakidis Marios misaakidis@yahoo.gr

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
  ********************************************************************************
  

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

 */

#include <Process.h>


const int led = 13;         // on-board led of Arduino Yun
const int hall_input = 12;  // output of the hall effect sensor

const int UID = 100;        // Unique Identifier for yun

int peaks = 0;              // peaks since last transmission
unsigned long epoch;        // UNIX timestamp when Arduino starts

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
  // print virtual time
  Serial.println(epoch + (millis() / 1000));
}



unsigned long timeInEpoch() {
  Process time;                   // process to run on Linuino
  char epochCharArray[25] = "";   // char array to be used for atol

  // get UNIX timestamp
  if (!time.running())  {
    time.begin("date");
    time.addParameter("+%s");
    time.run();
  }
  
  // when execution is completed, store in charArray
  while (time.available() > 0) {
    time.readString().toCharArray(epochCharArray, 25);
  }
  
  // return long with timestamp
  return atol(epochCharArray);
}