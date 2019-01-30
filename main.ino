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

#include <pthread.h>
#include <WiFi.h>

#include "DisplaySSD1306.h"
//#include "MP3PlayerI2S.h"
#include "NFCReaderPN532.h"

#define BUTTON_A_PIN 15
#define BUTTON_B_PIN 2
#define BUTTON_C_PIN 13

int buttonAState = 0;
int buttonBState = 0;
int buttonCState = 0;

//MP3PlayerI2S *mp3Player;
NFCReaderPN532 *nfcReader;

byte lastSeenTagID[4];
boolean lastTagPresentState = false;

/*
void *startLoopSound(void *mp3PlayerImpl) {
  // when this terminates, the thread ends.
  // this should run forever.
  while (true) {
    ((MP3PlayerI2S*)mp3PlayerImpl)->loop();
  }
}
*/

void *startLoopNFC(void *nfcReaderImpl) {
  // when this terminates, the thread ends.
  // this should run forever.
  while (true) {
    Serial.println("NFC: running loop");
    ((NFCReaderPN532*)nfcReaderImpl)->loop();
    if (((NFCReaderPN532*)nfcReaderImpl)->tagPresent()) {
      Serial.println("NFC: loop completed, tag present");
      NFCPayload* payload = ((NFCReaderPN532*)nfcReaderImpl)->getPayload();
      Serial.printf("NFC: tag id '%s'\n", payload->id);
      Serial.printf("NFC: tag type '%02d'\n", payload->type);
      Serial.printf("NFC: tag title '%s'\n", payload->title);
      Serial.printf("NFC: tag folder '%02d'\n", payload->folder);
      Serial.printf("NFC: tag track '%03d'\n", payload->track);
      Serial.printf("NFC: tag url '%s'\n", payload->url);
    } else {
      Serial.println("NFC: loop completed, no tag present");
    }
    // only checking NFC every 2 seconds
    delay(2000);
    /*
    Serial.println("MAIN 1");
    // handle nfc events
    boolean currentPresentState = ((NFCReaderPN532*)nfcReaderImpl)->tagPresent();
    Serial.println("MAIN 2");
    if (!currentPresentState && lastTagPresentState) {
      Serial.println("MAIN 3");
      Serial.println("NFC: Tag Removed");
      lastTagPresentState = currentPresentState;
    }
    Serial.println("MAIN 4");
    if (currentPresentState && !lastTagPresentState) {
      Serial.println("NFC: Tag Detected");
      lastTagPresentState = currentPresentState;
      // read and output uuid
      Serial.println("MAIN 5");
      byte *tagUUID = ((NFCReaderPN532*)nfcReaderImpl)->getCurrentTagId();
      Serial.println("MAIN 6");
      // check if new tag is equal to the last tag
      if (!memcmp(tagUUID, lastSeenTagID, 4)) {
        Serial.println("MAIN 7");
        // the IDs are equal
        Serial.print("NFC: Prior Tag Detected: ");
      } else {
        Serial.println("MAIN 8");
        Serial.print("NFC: New Tag Detected: ");
      }
      Serial.println("MAIN 9");
      for (int i = 0; i < 4; i++) {
        lastSeenTagID[i] = tagUUID[i];
        Serial.print(tagUUID[i], HEX);
      }
      Serial.println("MAIN 10");
      Serial.print("\n");
      Serial.println(((NFCReaderPN532*)nfcReaderImpl)->getCurrentTagData());
      Serial.println("MAIN 11");
    }
    Serial.println("MAIN 12");
  */
  }
}

void startThreads() {
  pthread_t threads[2];
  int returnValue;
  /*
  returnValue = pthread_create(&threads[0], NULL, startLoopSound, (void*)mp3Player);
  if (returnValue) {
    Serial.println("SYS: An error has occurred on starting sound loop");
  }
  */
  returnValue = pthread_create(&threads[1], NULL, startLoopNFC, (void*)nfcReader);
  if (returnValue) {
    Serial.println("SYS: An error has occurred on starting NFC loop");
  }
}

void setup() {
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  Serial.println("Tiena RFID-based Audiobook Player");
  //mp3Player = new MP3PlayerI2S();
  nfcReader = new NFCReaderPN532();
  //mp3Player->init();
  nfcReader->init();
  //pinMode(BUTTON_A_PIN, INPUT);
  //pinMode(BUTTON_B_PIN, INPUT);
  //pinMode(BUTTON_C_PIN, INPUT);
  startThreads();
  Serial.println("Boot complete.");
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
  // button loop
  //loopButtons();

  
}
