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

#include "Arduino.h"
#include "string.h"
#include "SPI.h"
#include "PN532.h"
#include "NfcAdapter.h"
#include "PN532_SPI.h"

#include "NFCReaderPN532.h"

#define RST_PIN 22
#define SS_PIN 21

NFCReaderPN532::NFCReaderPN532() {
  // NOP
}

void NFCReaderPN532::init() {
  this->mimetype = MIMETYPE;
  Serial.println(F("Initializing SPI Bus..."));
  SPI.begin();
  Serial.println("SPI Bus ready.");
  Serial.println(F("Initializing NFC Module..."));
  this->pn532spi = new PN532_SPI(SPI, 5);
  this->nfc = new NfcAdapter(*pn532spi);
  this->nfc_tag_present_prev = false;
  this->nfc_tag_present = false;
  this->_nfc_error_counter = 0;
  this->_tag_found = false;
  this->nfc->begin();
  Serial.println("NFC Module ready.");
}

void NFCReaderPN532::loop() {
  if (this->nfc->tagPresent()) {
    this->nfc_tag_present_prev = this->nfc_tag_present;
    NfcTag tag = this->nfc->read();
    uint8_t uidLength = tag.getUidLength();
    byte* uidBytes = new byte[uidLength];
    tag.getUid(uidBytes, uidLength);
    for (int i=0; i<4; i++){
      this->currentTagSerial[i] = uidBytes[i];
    }
    if (tag.hasNdefMessage()) {
        NdefMessage ndefMessage = tag.getNdefMessage();
        if (ndefMessage.getRecordCount()==1) {
          NdefRecord record = ndefMessage.getRecord(0);
          if (record.getTnf()==TNF_MIME_MEDIA) {
            int typeLength = record.getTypeLength();
            byte* type = new byte[typeLength];
            record.getType(type);
            if (memcmp(this->mimetype, type, strlen(MIMETYPE))) {
              // mimetype does not match, don't process
              Serial.printf("NFC: Tag record is not of mimetype '%s': %s\n", this->mimetype, type);
              return;
            };
            int payloadLength = record.getPayloadLength();
            byte* payloadBuffer = new byte(payloadLength);
            this->currentTagData = new char(payloadLength);
            record.getPayload(payloadBuffer);
            for (int i=0; i<payloadLength; i++) {
              this->currentTagData[i] = char(payloadBuffer[i]);
            }
          } else {
            Serial.println("NFC: Tag record is not of type TNF_MIME_MEDIA.");
          }
        } else {
          Serial.println("NFC: Tag has more or less than one NDEF message which is unsupported.");
        }
    }
  } else {
    this->nfc_tag_present = false;
    for (int i=0; i<4; i++){
      this->currentTagSerial[i] = 0;
    }
  }
}

boolean NFCReaderPN532::tagPresent() {
  return this->nfc_tag_present;
}

byte* NFCReaderPN532::getCurrentTagSerial() {
  return this->currentTagSerial;
}

char* NFCReaderPN532::getCurrentTagData() {
  return this->currentTagData;
}

NFCPayload* NFCReaderPN532::getPayload() {
  // TODO handle errors
    NFCPayload* payloadStruct = new NFCPayload();
    // Returns first token (id)
    char* token = strtok(this->currentTagData, "%");
    if (token != NULL)
      payloadStruct->id = token;
    // Keep printing tokens while one of the 
    // delimiters present in string. 
    token = strtok(NULL, "%"); 
    if (token != NULL)
      payloadStruct->type = atoi(token);   
    token = strtok(NULL, "%"); 
    if (token != NULL)
      payloadStruct->folder = atoi(token);
    token = strtok(NULL, "%"); 
    if (token != NULL)
      payloadStruct->track = atoi(token);
    token = strtok(NULL, "%"); 
    if (token != NULL)
      payloadStruct->title = token;
    token = strtok(NULL, "%"); 
    if (token != NULL)
      payloadStruct->url = token;
  return payloadStruct;
};

