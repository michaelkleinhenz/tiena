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
#include "AudioFileSourceMMC.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "MP3PlayerI2S.h"

MP3PlayerI2S::MP3PlayerI2S() {
  // NOP
}

void MP3PlayerI2S::metadataCallback(void *cbData, const char *type, bool isUnicode, const char *string) {
  (void)cbData;
  Serial.printf("MP3: ID3 callback for: %s = '", type);
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

void MP3PlayerI2S::init() {
  Serial.println("Initializing MP3 Module...");
  if(!SD_MMC.begin()) {
    Serial.println("MP3: Card Reader Init Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if  (cardType == CARD_NONE) {
    Serial.println("MP3: No SD_MMC Card found");
    return;
  }
  this->i2sOutput = new AudioOutputI2S();
  this->mp3AudioGenerator = new AudioGeneratorMP3();
  Serial.println("MP3 Module ready.");
}

void MP3PlayerI2S::loop() {
  if (this->mp3AudioGenerator->isRunning() && !this->mp3AudioGenerator->loop()) {
    this->mp3AudioGenerator->stop();
    Serial.printf("MP3: Playback finished\n");
  }
}

void MP3PlayerI2S::setVolume(uint8_t volume) {
  //Serial.printf("MP3: Setting Volume to %d\n", this->mp3PlayerInstance.readVolume());
  // TODO implement
}

void MP3PlayerI2S::volumeUp() {
  //Serial.printf("MP3: Setting Volume to %d\n", this->mp3PlayerInstance.readVolume());
  // TODO implement
}

void MP3PlayerI2S::volumeDown() {
  //Serial.printf("MP3: Setting Volume to %d\n", this->mp3PlayerInstance.readVolume());
  // TODO implement
}

void MP3PlayerI2S::playFolder(uint8_t folderNumber) {
  this->playFromFolderTrack(folderNumber, 0);
}

void MP3PlayerI2S::stop() {
  Serial.printf("MP3: Stop\n");
  // TODO implement
}

void MP3PlayerI2S::pause() {
  Serial.printf("MP3: Pause\n");
  // TODO implement
}

void MP3PlayerI2S::play() {
  Serial.printf("MP3: Resume Playing Audio\n");
  this->currentFileSource = new AudioFileSourceMMC("/01/001.mp3");
  this->currentFileSourceID3 = new AudioFileSourceID3(currentFileSource);
  this->currentFileSourceID3->RegisterMetadataCB(this->metadataCallback, (void*)"ID3TAG");
  this->mp3AudioGenerator->begin(currentFileSourceID3, i2sOutput);
}

void MP3PlayerI2S::playFromFolderTrack(uint8_t folderNumber, uint8_t trackNumber) {
  Serial.printf("MP3: Playing from Track %d of folder %d\n", trackNumber, folderNumber);
  char buffer[12];
  sprintf(buffer, "/%02d/%03d.mp3", folderNumber, trackNumber);
  Serial.printf("MP3: Playing File %s\n", buffer);
  this->currentFileSource = new AudioFileSourceMMC(buffer);
  this->currentFileSourceID3 = new AudioFileSourceID3(currentFileSource);
  this->currentFileSourceID3->RegisterMetadataCB(this->metadataCallback, (void*)"ID3TAG");
  this->mp3AudioGenerator->begin(currentFileSourceID3, i2sOutput);
}

void MP3PlayerI2S::next() {
  //Serial.printf("MP3: Playing Track %d\n", this->mp3PlayerInstance.readCurrentFileNumber());
  // TODO implement
}

void MP3PlayerI2S::previous() {
  //Serial.printf("MP3: Playing Track %d\n", this->mp3PlayerInstance.readCurrentFileNumber());
  // TODO implement
}

uint8_t MP3PlayerI2S::getNumFolders() {
  return -1;
  // TODO implement
}

uint8_t MP3PlayerI2S::getCurrentFileNumber() {
  return -1;
  // TODO implement
}

uint8_t MP3PlayerI2S::getState() {
  return -1;
  // TODO implement
}

uint8_t MP3PlayerI2S::getCurrentVolume() {
  return -1;
  // TODO implement
}

void MP3PlayerI2S::printInfoToConsole() {
  Serial.printf("MP3 Status Info:\n");
  // TODO implement
}

void MP3PlayerI2S::printStateToConsole() {
  // TODO implement
}
