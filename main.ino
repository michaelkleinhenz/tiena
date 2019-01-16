/* 
 * Tiena - RFID MP3 Player
 * Copyright (C) 2019 Michael Kleinhenz <michael@kleinhenz.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

// this example will play a track and then
// every five seconds play another track
//
// it expects the sd card to contain these three mp3 files
// but doesn't care whats in them
//
// sd:/mp3/0001.mp3
// sd:/mp3/0002.mp3
// sd:/mp3/0003.mp3

#include <DFMiniMp3.h>

// implement a notification class,
// its member methods will get called
class Mp3Notify
{
public:
  static void OnError(uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }

  static void OnPlayFinished(uint16_t globalTrack)
  {
    Serial.println();
    Serial.print("Play finished for #");
    Serial.println(globalTrack);
  }

  static void OnCardOnline(uint16_t code)
  {
    Serial.println();
    Serial.print("Card online ");
    Serial.println(code);
  }

  static void OnCardInserted(uint16_t code)
  {
    Serial.println();
    Serial.print("Card inserted ");
    Serial.println(code);
  }

  static void OnCardRemoved(uint16_t code)
  {
    Serial.println();
    Serial.print("Card removed ");
    Serial.println(code);
  }
};

// instance a DFMiniMp3 object,
// defined with the above notification class and the hardware serial class
//
//DFMiniMp3 mp3 = null;

// Some arduino boards only have one hardware serial port, so a software serial port is needed instead.
// comment out the above definition and uncomment these lines
//SoftwareSerial secondarySerial(10, 11); // RX, TX
//DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(secondarySerial);

/*
Just few bullet points to sum up

Connect EN to 3.3v
Connect GPIO0 to GND
Connect GPIO15 to GND to flash
Remember to connect the reference GND to the GND of esp8266
5.Lastly correctly match the tx and rx lines

*/
HardwareSerial mp3Serial(1);
DFMiniMp3<HardwareSerial, Mp3Notify> mp3(mp3Serial);

#define RXD2 2
#define TXD2 15 // DO NOT USE FOR UART, PREVENTS FLASHING, NEEDS TO BE LOW TO FLASH

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  mp3Serial.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial Txd is on pin: " + String(TXD2));
  Serial.println("Serial Rxd is on pin: " + String(RXD2));

  mp3.begin();

  uint16_t volume = mp3.getVolume();
  Serial.print("volume ");
  Serial.println(volume);
  mp3.setVolume(24);

  uint16_t count = mp3.getTotalTrackCount();
  Serial.print("files ");
  Serial.println(count);

  Serial.println("starting...");
}

void waitMilliseconds(uint16_t msWait)
{
  uint32_t start = millis();

  while ((millis() - start) < msWait)
  {
    // calling mp3.loop() periodically allows for notifications
    // to be handled without interrupts
    mp3.loop();
    delay(1);
  }
}

void loop()
{
  Serial.println("track 1");
  //mp3.playFile(0);
  mp3.playMp3FolderTrack(1); // sd:/mp3/0001.mp3

  waitMilliseconds(5000);
  Serial.println("track 2");
  mp3.playMp3FolderTrack(2); // sd:/mp3/0002.mp3

  waitMilliseconds(5000);

  Serial.println("track 3");
  mp3.playMp3FolderTrack(3); // sd:/mp3/0002.mp3

  waitMilliseconds(5000);

}