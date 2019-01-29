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
#include "MFRC522.h"

#include "NFCReaderMFRC522.h"

#define RST_PIN 22
#define SS_PIN 21

NFCReaderMFRC522::NFCReaderMFRC522() {
  // NOP
}

void NFCReaderMFRC522::init() {
  Serial.println(F("Initializing SPI Bus..."));
  SPI.begin();
  Serial.println("SPI Bus ready.");
  Serial.println(F("Initializing NFC Module..."));
  this->mfrc522 = new MFRC522(SS_PIN, RST_PIN);
  this->nfc_tag_present_prev = false;
  this->nfc_tag_present = false;
  this->_nfc_error_counter = 0;
  this->_tag_found = false;
  mfrc522->PCD_Init();
  Serial.println("NFC Module ready.");
  mfrc522->PCD_DumpVersionToSerial();
}

void NFCReaderMFRC522::loop() {
  // tag presence detection by metamorphious
  // https://github.com/miguelbalboa/rfid/issues/352
  this->nfc_tag_present_prev = this->nfc_tag_present;
  this->_nfc_error_counter += 1;
  if (this->_nfc_error_counter > 25) {
    this->_tag_found = false;
  }
  // detect tag without looking for collisions
  byte bufferATQA[2];
  byte bufferSize = sizeof(bufferATQA);
  // reset baud rates
  mfrc522->PCD_WriteRegister(mfrc522->TxModeReg, 0x00);
  mfrc522->PCD_WriteRegister(mfrc522->RxModeReg, 0x00);
  // reset ModWidthReg
  mfrc522->PCD_WriteRegister(mfrc522->ModWidthReg, 0x26);
  MFRC522::StatusCode result = mfrc522->PICC_RequestA(bufferATQA, &bufferSize);
  if (result == mfrc522->STATUS_OK){
    if (!mfrc522->PICC_ReadCardSerial()) { // since a PICC placed get serial and continue
      return;
    }
    // store the serial
    for (int i=0; i<4; i++){
      this->currentTagSerial[i] = mfrc522->uid.uidByte[i];
    }
    // read the card data by CptAwe
    // https://github.com/miguelbalboa/rfid/issues/384
    byte tagData[16][4];
    // reset data first
    for (int i=0; i<=15; i++) {
  		for (int j=0; j<=4; j++) {
	  		tagData[i][j] = 0;
      }
	  }
    MFRC522::StatusCode status;
    byte buffer[18]; // 16 (data) + 2 (CRC)
    for (byte page=0; page<=15; page+=4) {
      byte byteCount = sizeof(buffer);
      status=mfrc522->MIFARE_Read(page, buffer, &byteCount);
      if (status == mfrc522->STATUS_OK) {
        int i_=0;
        for (int i=page; i<=page+3; i++) {
          for (int j=0; j<=3; j++) {
            tagData[i][j]=buffer[4*i_ + j];
          }
          i_++;
        }
      }
    }
    // decode tag data
    int strIndex = 0;
    boolean inData = false;
    for (int i=6; i<16; i++) {
      for (int j=0; j<4; j++) {
        if (tagData[i][j] == 11 && !inData) {
          // 0B marks start of data
          inData = true;
        } else if (tagData[i][j] == 254 && inData) {
          // FE marks the end of data
          inData = false;
          break;
        } else if (inData) {
          this->currentTagData[strIndex] = char(tagData[i][j]);
          strIndex++;
        }
      }
    }
    // terminate string with \0
    this->currentTagData[strIndex] = '\0';
    // reset the error counters
    this->_nfc_error_counter = 0;
    this->_tag_found = true;        
  }
  this->nfc_tag_present = this->_tag_found;  
}

boolean NFCReaderMFRC522::tagPresent() {
  return this->nfc_tag_present;
}

byte* NFCReaderMFRC522::getCurrentTagSerial() {
  return this->currentTagSerial;
}

char* NFCReaderMFRC522::getCurrentTagData() {
  return this->currentTagData;
}

NFCPayload* NFCReaderMFRC522::getPayload() {
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

