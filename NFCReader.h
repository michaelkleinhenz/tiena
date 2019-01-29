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

#ifndef NFCREADER_H
#define NFCREADER_H

#define MIMETYPE "app/tiena"

typedef struct NFCPayload {
	char* title;
  char* id;
  uint8_t type;
	uint8_t folder;
	uint8_t track;
} NFCPayload;

class NFCReader {
  public:
    NFCReader();
    void init();
    void loop();
    boolean tagPresent();
    byte* getCurrentTagSerial();
    char* getCurrentTagData();
    //NFCPayload getPayload();
};

#endif