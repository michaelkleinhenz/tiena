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
  Serial.println("SPI Bus ready");
  Serial.println(F("Initializing NFC Module..."));
  this->pn532spi = new PN532_SPI(SPI, 5);
  this->nfc = new NfcAdapter(*pn532spi);
  this->nfc_tag_present_prev = false;
  this->nfc_tag_present = false;
  this->_nfc_error_counter = 0;
  this->_tag_found = false;
  this->nfc->begin();
  Serial.println("NFC Module ready");
}

void NFCReaderPN532::loop() {
  if (this->nfc->tagPresent()) {
    this->nfc_tag_present_prev = this->nfc_tag_present;
    NfcTag tag = this->nfc->read();
    // TODO: check if we have a memory leak or lost memory here
    this->currentTagId = tag.getUidString();
    Serial.printf("NFC: detected tag id '%s'\n", this->currentTagId.c_str());
    if (tag.hasNdefMessage()) {
        NdefMessage ndefMessage = tag.getNdefMessage();
        if (ndefMessage.getRecordCount()==1) {
          NdefRecord record = ndefMessage.getRecord(0);
          if (record.getTnf()==TNF_MIME_MEDIA) {
            String type = record.getType();
            Serial.printf("NFC: tag has media message with type '%s'\n", type.c_str());
            if (!type.equalsIgnoreCase(this->mimetype)) {
              Serial.printf("NFC: Tag record is not of mimetype '%s': %s\n", this->mimetype, type.c_str());
            }
            int payloadLength = record.getPayloadLength();
            byte* payloadBuffer = new byte[payloadLength];
            Serial.printf("NFC: reading tag payload, length %d\n", payloadLength);
            record.getPayload(payloadBuffer);
            Serial.println("NFC: payload read completed");
            // TODO: check if we have a memory leak or lost memory here
            this->currentTagData = "";
            for (int i=0; i<payloadLength; i++) {
              this->currentTagData += char(payloadBuffer[i]);
            }
            Serial.printf("NFC: payload data: '%s'\n", this->currentTagData.c_str());
            this->nfc_tag_present = true;
          } else {
            Serial.println("NFC: tag record is not of type TNF_MIME_MEDIA.");
            this->nfc_tag_present = false;
          }
        } else {
          Serial.println("NFC: tag has more or less than one NDEF message which is unsupported");
          this->nfc_tag_present = false;
        }
    }
  } else {
    Serial.println("NFC: no tag present");
    this->nfc_tag_present = false;
    for (int i=0; i<4; i++) {
      this->currentTagId[i] = 0;
    }
  }
}

boolean NFCReaderPN532::tagPresent() {
  return this->nfc_tag_present;
}

String NFCReaderPN532::getCurrentTagId() {
  return this->currentTagId;
}

String NFCReaderPN532::getCurrentTagData() {
  return this->currentTagData;
}

NFCPayload* NFCReaderPN532::getPayload() {
  // TODO handle errors, check if there is a memory leak here
  NFCPayload* payloadStruct = new NFCPayload();
  int length = this->currentTagData.length()+1;
  char* buffer = new char[length];
  this->currentTagData.toCharArray(buffer, length);
  // Returns first token (id)
  char* token = strtok(buffer, "%");
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

