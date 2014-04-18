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


const int led = 13;           // on-board led of Arduino Yun
const int hall_output = 12;    // output of the hall effect sensor

const int UID = 100;          // Unique Identifier for yun
const short broadcastInterval = 10; // Interval between broadcasts in seconds

int peaks = 0;                // peaks since last transmission
unsigned long epoch;          // UNIX timestamp when Arduino starts
unsigned long millisAtEpoch;  // millis at the time of timestamp
unsigned long lastBroadcast;  // millis at last broadcast


/////////////////////////////////////////////////////////////////////
void setup() {
  // Initialize Bridge
  Bridge.begin();

  // Initialize serial communications:
  Serial.begin(9600);
  
  // Initialize the on-board led as output and the pin connected
  // to the hall effect sensor as input:
  pinMode(led, OUTPUT);
  pinMode(hall_output, INPUT);

  // Wait for serial to be connected, remove this in the future
  while(!Serial);
  
  // Wait until connected to the Internet
  while(!isConnectedToInternet());
  
  // Sync clock with NTP
  setClock();
  
  // Get time from Linino in Unix timestamp format
  epoch = timeInEpoch();
  lastBroadcast = millisAtEpoch;
}


/////////////////////////////////////////////////////////////////////
void loop() {
  waitForPeak();
  if((millis() - lastBroadcast) > (broadcastInterval * 1000)) {
    broadcastPeaks();
  }
}

/////////////////////////////////////////////////////////////////////
void printVirtualTime() {
  Serial.print(epoch + ((millis() - millisAtEpoch) / 1000));
}


/////////////////////////////////////////////////////////////////////
// Wait for a peak. Turn the led on while hall effect output is 0V.
// Then turn led off and increase the number of peaks.
void waitForPeak() {
  while(digitalRead(hall_output) == HIGH);
  
  digitalWrite(led, HIGH);
  delay(10);  // small delay because a pass of the magnet might be counted as multiple peaks
  while(digitalRead(hall_output) == LOW);
  digitalWrite(led, LOW);
  
  peaks++;
}


/////////////////////////////////////////////////////////////////////
void broadcastPeaks() {
  Process p;
  
  printVirtualTime();
  Serial.print("\t");
  Serial.print("Broadcasting peaks: ");
  Serial.println(peaks);
  
  // TODO Replace uname with curl
  // curl -X POST -H "Content-Type: application/json" -d '{ "UID": UID,  "time_from": lastBroadcast,  "time_to": millis(),  "peaks": peaks }' http://ydorcareapi.azurewebsites.net/api/Measures
  p.runShellCommandAsynchronously("uname");
  
  /*  Uncomment for debugging only
      since the above command was run asynchronously
      in order not to stall the peaks measurement
  while(p.available() > 0) {
    Serial.println(p.readString());
  }
  */
  
  lastBroadcast = millis();
  peaks = 0;
}


/////////////////////////////////////////////////////////////////////
// Check if connected to the Internet
int isConnectedToInternet() {
  Process p;
  int result;
  
  Serial.print("Checking connectivity... ");

  p.runShellCommand("ping -W 1 -c 4 www.google.com >& /dev/null && echo 1 || echo 0");
  while(p.running());
  
  result = p.parseInt();
  Serial.println(result);
  return result;
}


/////////////////////////////////////////////////////////////////////
// Synchronize clock using NTP
void setClock() {
  Serial.println("Setting clock.");
  
  Process p;
  p.runShellCommand("ntpd -nqp 0.openwrt.pool.ntp.org");
  while(p.running());
}


/////////////////////////////////////////////////////////////////////
// Return timestamp of Linino
unsigned long timeInEpoch() {
  Process time;                   // process to run on Linuino
  char epochCharArray[12] = "";   // char array to be used for atol

  // Get UNIX timestamp
  time.begin("date");
  time.addParameter("+%s");
  time.run();
  
  // When execution is completed, store in charArray
  while (time.available() > 0) {
    millisAtEpoch = millis();
    time.readString().toCharArray(epochCharArray, 12);
  }
  
  // Return long with timestamp
  return atol(epochCharArray);
}