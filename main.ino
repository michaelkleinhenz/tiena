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
#include <FS.h>
#include <SD_MMC.h>
#include <HTTPClient.h>

#include "DisplaySSD1306.h"
#include "MP3PlayerI2S.h"
#include "NFCReaderPN532.h"
#include "Downloader.h"

#define BUTTON_A_PIN 15
#define BUTTON_B_PIN 2
#define BUTTON_C_PIN 13

int buttonAState = 0;
int buttonBState = 0;
int buttonCState = 0;

MP3PlayerI2S *mp3Player;
NFCReaderPN532 *nfcReader;
Downloader *downloader;

String lastSeenTagID;
boolean lastTagPresentState = false;

void setupWifi() {
  //WiFi.mode(WIFI_STA);
  //WiFi.mode(WIFI_OFF);
  char* ssid = "Quendor";
  char* password = "linuxtag";
  Serial.printf("WIFI: connecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("WIFI: still trying to connect");
  }
  Serial.printf("WIFI: connection successful, IP address: %s\n", WiFi.localIP().toString().c_str());
}

void *startLoopNFC(void *nfcReaderImpl) {
  // when this terminates, the thread ends.
  // this should run forever.
  while (true) {
    Serial.println("NFC: running loop");
    ((NFCReaderPN532*)nfcReaderImpl)->loop();
    Serial.print("NFC: loop completed - ");
    boolean currentPresentState = ((NFCReaderPN532*)nfcReaderImpl)->tagPresent();
    if (((NFCReaderPN532*)nfcReaderImpl)->tagPresent()) {
      Serial.println("tag present");
    } else {
      Serial.println("no valid tag present");
    }
    if (!currentPresentState && lastTagPresentState) {
      Serial.println("NFC: tag was removed");
      lastTagPresentState = currentPresentState;
    }
    if (currentPresentState && !lastTagPresentState) {
      Serial.println("NFC: tag detected");
      lastTagPresentState = currentPresentState;
      // check if new tag is equal to the last tag
      String currentTagId = ((NFCReaderPN532*)nfcReaderImpl)->getCurrentTagId();
      if (lastSeenTagID && lastSeenTagID.equals(currentTagId)) {
        // the IDs are equal
        Serial.println("NFC: prior tag detected");
        // TODO check if we're plaing, if not, start. If yes, do nothing
      } else {
        Serial.println("NFC: new tag detected");
        NFCPayload* payload = ((NFCReaderPN532*)nfcReaderImpl)->getPayload();
        Serial.printf("NFC: tag id '%s'\n", payload->id);
        Serial.printf("NFC: tag type '%02d'\n", payload->type);
        Serial.printf("NFC: tag title '%s'\n", payload->title);
        Serial.printf("NFC: tag folder '%02d'\n", payload->folder);
        Serial.printf("NFC: tag track '%03d'\n", payload->track);
        Serial.printf("NFC: tag url '%s'\n", payload->url);
        // TODO download, store, start playback
      }
      lastSeenTagID = currentTagId;
    }
    // only checking NFC every 2 seconds
    Serial.println("NFC: sleeping 2s");
    delay(2000);
  }
}

void *loopButtons(void *param) {
  while (true) {
    buttonAState = digitalRead(BUTTON_A_PIN);
    if (buttonAState == 1) {
      Serial.println("KEYS: detected key A pressed");
    }
    if (buttonAState == 0) {
    }
    buttonBState = digitalRead(BUTTON_B_PIN);
    if (buttonBState == 1) {
      Serial.println("KEYS: detected key B pressed");
    }
    if (buttonBState == 0) {
    }
    buttonCState = digitalRead(BUTTON_C_PIN);
    if (buttonCState == 1) {
      Serial.println("KEYS: detected key C pressed");
    }
    if (buttonCState == 0) {
    }
    delay(10);
  }
}

void startThreads() {
  pthread_t threads[3];
  int returnValue;
  returnValue = pthread_create(&threads[1], NULL, startLoopNFC, (void*)nfcReader);
  if (returnValue) {
    Serial.println("SYS: an error has occurred when starting NFC loop");
  }
  /*
  returnValue = pthread_create(&threads[2], NULL, startLoopButtons, (void*)-1);
  if (returnValue) {
    Serial.println("SYS: An error has occurred on starting buttons loop");
  }
  */
}

void setup() {
  Serial.begin(115200);
  Serial.println("Tiena RFID-based Audiobook Player");
  if(!SD_MMC.begin()) {
    Serial.println("SYS: Card Reader Init Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("SYS: No SD_MMC Card found");
    return;
  }
  Serial.println("SYS: init MP3 player..");
  mp3Player = new MP3PlayerI2S();
  Serial.println("SYS: init NFC reader..");
  nfcReader = new NFCReaderPN532();
  Serial.println("SYS: init downloader reader..");
  downloader = new Downloader();
  Serial.println("SYS: finishing init MP3 player..");
  mp3Player->init();
  Serial.println("SYS: finishing init NFC reader..");
  nfcReader->init();
  //pinMode(BUTTON_A_PIN, INPUT);
  //pinMode(BUTTON_B_PIN, INPUT);
  //pinMode(BUTTON_C_PIN, INPUT);
  Serial.println("SYS: starting system threads..");
  startThreads();
  Serial.println("SYS: setting up WIFI..");
  //setupWifi(); // Somehow this crashes with an infinite loop if the mp3.loop() is changed
  Serial.println("SYS: launching demo..");
  //downloader->formatCard();
  downloader->checkOrDownloadBook("/01.json", "https://s3-eu-west-1.amazonaws.com/tiena-files/01.json");
  Book* book = downloader->getBookDescriptorForId("01");
  for (int i=0; i<book->tracks.size(); i++) {
    Serial.printf("SYS: track %d - '%s' in path '%s'\n", book->tracks[i]->number, book->tracks[i]->title, book->tracks[i]->filepath);
  }
  mp3Player->playBook(book);
  Serial.println("SYS: boot complete");
}

void loop() {
  // the mp3 player loop needs to be on the main thread because
  // the limited stack crashed the system otherwise.
  mp3Player->loop();
}
