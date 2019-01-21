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
#include "DFRobotDFPlayerMini.h"

#ifndef MP3_H
#define MP3_H

class MP3Player {
  public:
    MP3Player();
    void init();
    void setVolume(uint8_t volume);
    void volumeUp();
    void volumeDown();
    void playFolder(uint8_t folderNumber, boolean looped);
    void playFolderTrack(uint8_t folderNumber, uint8_t trackNumber);
    void playTrack(int track);
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
    DFRobotDFPlayerMini mp3PlayerInstance;
};

#endif