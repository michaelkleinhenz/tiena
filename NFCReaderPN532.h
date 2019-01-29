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
#include "NFCReader.h"

#ifndef NFCPN532_H
#define NFCPN532_H

class NFCReaderPN532 : public NFCReader {
  public:
    NFCReaderPN532();
    void init();
    void loop();
    boolean tagPresent();
    byte* getCurrentTagSerial();
    char* getCurrentTagData();
    //NFCPayload getPayload();
  private:
    PN532_SPI *pn532spi;
    NfcAdapter *nfc;
    byte currentTagSerial[4];
    char currentTagData[16*4];
    bool nfc_tag_present_prev;
    bool nfc_tag_present;
    int _nfc_error_counter;
    bool _tag_found;
    char *mimetype;
};

#endif