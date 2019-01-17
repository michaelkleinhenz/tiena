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

#include "rfid.h"

RFIDModule rfidModule;

byte lastSeenTagID[4];
boolean lastTagPresentState = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Tiena RFID-based Audiobook Player");
  rfidModule.init();
  Serial.println("Boot complete.");
}

void loop() {
  // let the rfid module loop
  rfidModule.loop();

  // get the rfid status
  if (!rfidModule.tagPresent() && lastTagPresentState) {
    Serial.println("RFID: Tag Removed");
    lastTagPresentState = rfidModule.tagPresent();
  }
  if (rfidModule.tagPresent() && !lastTagPresentState) {
    Serial.println("RFID: Tag Detected");
    lastTagPresentState = rfidModule.tagPresent();
    // read and output uuid
    byte *tagUUID = rfidModule.getCurrentTagSerial();
    // check if new tag is equal to the last tag
    if (!memcmp(tagUUID, lastSeenTagID, 4)) {
      // the IDs are equal
      Serial.print("RFID: Prior Tag Detected: ");
    } else {
      Serial.print("RFID: New Tag Detected: ");
    }
    for (int i = 0; i < 4; i++) {
      lastSeenTagID[i] = tagUUID[i];
      Serial.print(tagUUID[i], HEX);
    }
    Serial.print("\n");
    Serial.println(rfidModule.getCurrentTagData());
  }
}

/*
#include "mp3.h"

MP3Player mp3Player;

int buttonAPin = 15;
int buttonBPin = 2;
int buttonCPin = 4;
int buttonAState = 0;
int buttonBState = 0;
int buttonCState = 0;

void setup()
{
  // init serial console
  Serial.begin(115200);

  pinMode(buttonAPin, INPUT);
  pinMode(buttonBPin, INPUT);
  pinMode(buttonCPin, INPUT);

  mp3Player.init();
  mp3Player.printInfoToConsole();

  mp3Player.setVolume(30);
  mp3Player.playTrack(1);
}

void loop()
{
  buttonAState = digitalRead(buttonAPin); // put your main code here, to run repeatedly:
  if (buttonAState == 1)
  {
    Serial.println("BUTTON A ON");
  }
  if (buttonAState == 0)
  {
    Serial.println("BUTTON A OFF");
  }

  buttonBState = digitalRead(buttonBPin); // put your main code here, to run repeatedly:
  if (buttonBState == 1)
  {
    Serial.println("BUTTON B ON");
  }
  if (buttonBState == 0)
  {
    Serial.println("BUTTON B OFF");
  }

  buttonCState = digitalRead(buttonCPin); // put your main code here, to run repeatedly:
  if (buttonCState == 1)
  {
    Serial.println("BUTTON C ON");
  }
  if (buttonCState == 0)
  {
    Serial.println("BUTTON C OFF");
  }

  static unsigned long timer = millis();
  if (millis() - timer > 3000)
  {
    timer = millis();
    mp3Player.next(); //Play next mp3 every 3 second.
  }
  mp3Player.printStateToConsole();
}
*/