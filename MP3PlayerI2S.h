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

#ifndef MP3PLAYERI2S_H
#define MP3PLAYERI2S_H

class MP3PlayerI2S {
  public:
    MP3PlayerI2S();
    void init();
    void loop();
    void setVolume(uint8_t volume);
    void volumeUp();
    void volumeDown();
    void playFolder(uint8_t folderNumber);
    void playFolderTrack(uint8_t folderNumber, uint8_t trackNumber);
    void stop();
    void pause();
    void play();
    void next();
    void previous();
    uint8_t getState();
    uint8_t getNumFolders();
    uint8_t getCurrentFileNumber();
    uint8_t getCurrentVolume();
    void printStateToConsole();
    void printInfoToConsole();
  private:
    static void metadataCallback(void *cbData, const char *type, bool isUnicode, const char *string);
    AudioGeneratorMP3 *mp3AudioGenerator;
    AudioFileSourceID3 *currentFileSourceID3;
    AudioFileSourceMMC *currentFileSource;
    AudioOutputI2S *i2sOutput;
};

#endif