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
#include "mp3.h"

#define BUTTON_A_PIN 15
#define BUTTON_B_PIN 2
#define BUTTON_C_PIN 4

int buttonAState = 0;
int buttonBState = 0;
int buttonCState = 0;

MP3Player mp3Player;
RFIDModule rfidModule;

byte lastSeenTagID[4];
boolean lastTagPresentState = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Tiena RFID-based Audiobook Player");
  rfidModule.init();
  //mp3Player.init();
  Serial.println("Boot complete.");

  pinMode(BUTTON_A_PIN, INPUT);
  pinMode(BUTTON_B_PIN, INPUT);
  pinMode(BUTTON_C_PIN, INPUT);

  /*
  mp3Player.setVolume(30);
  mp3Player.playTrack(1);
  */
}

void loopButtons() {
  buttonAState = digitalRead(BUTTON_A_PIN);
  if (buttonAState == 1) {
    Serial.println("KEYS: Detected Key A Pressed");
  }
  if (buttonAState == 0) {
  }
  buttonBState = digitalRead(BUTTON_B_PIN);
  if (buttonBState == 1) {
    Serial.println("KEYS: Detected Key B Pressed");
  }
  if (buttonBState == 0) {
  }
  buttonCState = digitalRead(BUTTON_C_PIN);
  if (buttonCState == 1) {
    Serial.println("KEYS: Detected Key C Pressed");
  }
  if (buttonCState == 0) {
  }
}

void loop() {
  // rfid module loop
  rfidModule.loop();

  // button loop
  loopButtons();

  // handle mp3 events  
  /*
  static unsigned long timer = millis();
  if (millis() - timer > 3000) {
    timer = millis();
    mp3Player.next(); //Play next mp3 every 3 second.
  }
  mp3Player.printStateToConsole();
  */

  // handle rfid events
  boolean currentPresentState = rfidModule.tagPresent();
  if (!currentPresentState && lastTagPresentState) {
    Serial.println("RFID: Tag Removed");
    lastTagPresentState = currentPresentState;
  }
  if (currentPresentState && !lastTagPresentState) {
    Serial.println("RFID: Tag Detected");
    lastTagPresentState = currentPresentState;
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
