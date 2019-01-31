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

String lastSeenTagID;
boolean lastTagPresentState = false;

void downloadBook() {
  File f = SD_MMC.open("/downloads/test.pdf", FILE_WRITE);
  if (f) {
    http.begin("https://www.dropbox.com/s/0nt98kwsnokmqzd/2003Jangatesmoviemaker.pdf?dl=1");
    int httpCode = http.GET();
    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            WiFiClient* stream = http.getStreamPtr();
            while (stream->available()) {
                char c = stream->read();
                f.print(c);
            }
        }
    }
    else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    f.close();
    http.end();
  }
}

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
  /*
  returnValue = pthread_create(&threads[0], NULL, startLoopSound, (void*)mp3Player);
  if (returnValue) {
    Serial.println("SYS: An error has occurred on starting sound loop");
  }
  */
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
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  Serial.println("Tiena RFID-based Audiobook Player");
  if(!SD_MMC.begin()) {
    Serial.println("MP3: Card Reader Init Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if  (cardType == CARD_NONE) {
    Serial.println("MP3: No SD_MMC Card found");
    return;
  }
  //mp3Player = new MP3PlayerI2S();
  nfcReader = new NFCReaderPN532();
  //mp3Player->init();
  nfcReader->init();
  //pinMode(BUTTON_A_PIN, INPUT);
  //pinMode(BUTTON_B_PIN, INPUT);
  //pinMode(BUTTON_C_PIN, INPUT);
  startThreads();
  Serial.println("SYS: boot complete");
}

void loop() {
  // NOP
}
