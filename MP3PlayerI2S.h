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

#include "SimpleQueue.h"
#include "Downloader.h"
#include "AudioFileSourceMMC.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

#ifndef MP3PLAYERI2S_H
#define MP3PLAYERI2S_H

#define STATE_STOPPED 0
#define STATE_PLAYING 1

class MP3PlayerI2S {
  public:
    MP3PlayerI2S();
    void init();
    void loop();
    void setVolume(uint8_t volume);
    void volumeUp();
    void volumeDown();
    uint8_t getCurrentVolume();
    void playBook(Book* book);
    void playBookFromTrack(Book* book, Track* track);
    void playBookFromTrackNumber(Book* book, uint8_t trackNumber);
    void pause();
    void resume();
    void restart();
    void next();
    void previous();
    Track* getCurrentTrack();
    Book* getCurrentBook();
    uint8_t getState();
    void printStateToConsole();
  private:
    static void metadataCallback(void *cbData, const char *type, bool isUnicode, const char *string);
    AudioGeneratorMP3 *mp3AudioGenerator;
    AudioFileSourceID3 *currentFileSourceID3;
    AudioFileSourceMMC *currentFileSource;
    AudioOutputI2S *i2sOutput;
    Book* currentBook;
    uint8_t currentTrackIndex;
    uint8_t currentVolume;
    SimpleQueue<uint8_t>* playlist;
    uint8_t state;
};

#endif