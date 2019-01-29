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

#include "AudioFileSourceMMC.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

AudioGeneratorMP3 *mp3;
AudioFileSourceID3 *id3;
AudioFileSourceMMC *file;
AudioOutputI2S *out;

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  (void)cbData;
  Serial.printf("ID3 callback for: %s = '", type);

  if (isUnicode) {
    string += 2;
  }
  
  while (*string) {
    char a = *(string++);
    if (isUnicode) {
      string++;
    }
    Serial.printf("%c", a);
  }
  Serial.printf("'\n");
  Serial.flush();
}

PN532_HSU *pn532hsu;
NfcAdapter *nfc;

void setup(void) {
    Serial.begin(115200);
    Serial.println("NDEF Reader");
    Serial1.begin(115200, SERIAL_8N1, 14, 27);
    pn532hsu = new PN532_HSU(Serial1);
    nfc = new NfcAdapter(*pn532hsu);
    nfc->begin();
/*
    if(!SD.begin(4)){
        Serial.println("Card Mount Failed");
        return;
    }
  Serial.printf("mp3 start\n");

  file = new AudioFileSourceSD("/01/001.mp3");
  id3 = new AudioFileSourceID3(file);
  id3->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");
  out = new AudioOutputI2S();
  mp3 = new AudioGeneratorMP3();
  mp3->begin(id3, out);
  */
}

void loop(void) {
    Serial.println("\nScan a NFC tag\n");
    if (nfc->tagPresent())
    {
        NfcTag tag = nfc->read();
        tag.print();
    }
    /*
    if (mp3->isRunning()) {
      if (!mp3->loop()) mp3->stop();
    } else {
      Serial.printf("mp3 done\n");
    }
    */
    delay(5000);
}

/*
#include <Arduino.h>
#include <WiFi.h>
#include "SD.h"

#include "AudioFileSourceSD.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

AudioGeneratorMP3 *mp3;
AudioFileSourceID3 *id3;
AudioFileSourceSD *file;
AudioOutputI2S *out;

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  (void)cbData;
  Serial.printf("ID3 callback for: %s = '", type);

  if (isUnicode) {
    string += 2;
  }
  
  while (*string) {
    char a = *(string++);
    if (isUnicode) {
      string++;
    }
    Serial.printf("%c", a);
  }
  Serial.printf("'\n");
  Serial.flush();
}

void setup()
{
  WiFi.mode(WIFI_OFF); 
  Serial.begin(115200);
  delay(1000);
  if(!SD.begin(4)){
        Serial.println("Card Mount Failed");
        return;
    }
  Serial.printf("mp3 start\n");

  file = new AudioFileSourceSD("/01/001.mp3");
  id3 = new AudioFileSourceID3(file);
  id3->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");
  out = new AudioOutputI2S();
  mp3 = new AudioGeneratorMP3();
  mp3->begin(id3, out);
}

void loop()
{
  if (mp3->isRunning()) {
    if (!mp3->loop()) mp3->stop();
  } else {
    Serial.printf("mp3 done\n");
    delay(1000);
  }
}
*/

/*
#include "rfid.h"
#include "mp3.h"

#define BUTTON_A_PIN 15
#define BUTTON_B_PIN 2
#define BUTTON_C_PIN 13

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
*/