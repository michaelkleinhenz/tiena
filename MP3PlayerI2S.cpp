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

#include "MP3PlayerI2S.h"

MP3PlayerI2S::MP3PlayerI2S() {
  // NOP
}

void MP3PlayerI2S::init() {
  Serial.println("Initializing MP3 Module...");
  Serial.println("MP3 Module ready.");
}

void MP3PlayerI2S::setVolume(uint8_t volume) {
  Serial.printf("MP3: Setting Volume to %d\n", this->mp3PlayerInstance.readVolume());
}

void MP3PlayerI2S::volumeUp() {
  Serial.printf("MP3: Setting Volume to %d\n", this->mp3PlayerInstance.readVolume());
}

void MP3PlayerI2S::volumeDown() {
  Serial.printf("MP3: Setting Volume to %d\n", this->mp3PlayerInstance.readVolume());
}

void MP3PlayerI2S::playFolder(uint8_t folderNumber, boolean looped) {
  Serial.printf("MP3: Playing Track %d\n", this->mp3PlayerInstance.readCurrentFileNumber());
}

void MP3PlayerI2S::stop() {
  Serial.printf("MP3: Stop\n");
}

void MP3PlayerI2S::pause() {
  Serial.printf("MP3: Pause\n");
}

void MP3PlayerI2S::play() {
  Serial.printf("MP3: Playing Track %d\n", this->mp3PlayerInstance.readCurrentFileNumber());
}

void MP3PlayerI2S::playTrack(int track) {
  Serial.printf("MP3: Playing Track %d\n", this->mp3PlayerInstance.readCurrentFileNumber());
}

void MP3PlayerI2S::playFolderTrack(uint8_t folderNumber, uint8_t trackNumber) {
  Serial.printf("MP3: Playing Track %d of folder %d\n", trackNumber, folderNumber);
}

void MP3PlayerI2S::next() {
  Serial.printf("MP3: Playing Track %d\n", this->mp3PlayerInstance.readCurrentFileNumber());
}

void MP3PlayerI2S::previous() {
  Serial.printf("MP3: Playing Track %d\n", this->mp3PlayerInstance.readCurrentFileNumber());
}

uint8_t MP3PlayerI2S::getNumFolders() {
  return -1;
}

uint8_t MP3PlayerI2S::getCurrentFileNumber() {
  return -1;
}

uint8_t MP3PlayerI2S::getState() {
  return -1;
}

uint8_t MP3PlayerI2S::getCurrentVolume() {
  return -1;
}

void MP3PlayerI2S::printInfoToConsole() {
  Serial.printf("MP3 Status Info:\n");
}

void MP3PlayerI2S::printStateToConsole() {
}
