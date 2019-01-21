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
#include <Preferences.h>

#include "storage.h"

Storage::Storage() {
  // NOP
}

uint8_t Storage::retrieveTrackForBook(char* bookId) {
  this->preferences.begin("tiena", true);
  uint8_t track = this->preferences.getUInt(bookId, 0);
  this->preferences.end();
  return track;
}

boolean Storage::storeTrackForBook(char* bookId, uint8_t track) {
  this->preferences.begin("tiena", true);
  this->preferences.putUInt(bookId, track);
  this->preferences.end();
  return true;
}

boolean Storage::clear() {
  this->preferences.begin("tiena", true);
  this->preferences.clear();
}