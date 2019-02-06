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
  Serial.printf("MP3: read ID3 tag: '%s' = '", type);
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
  this->playlist = new SimpleQueue<uint8_t>(255);
  this->i2sOutput = new AudioOutputI2S();
  this->mp3AudioGenerator = new AudioGeneratorMP3();
  this->currentFileSource = NULL;
  this->currentFileSourceID3 = NULL;
  this->setVolume(3);
  this->state = STATE_STOPPED;
  Serial.println("MP3 Module ready.");
}

void MP3PlayerI2S::loop() {
  if (this->state == STATE_PLAYING && this->mp3AudioGenerator->isRunning()) {
    if (!this->mp3AudioGenerator->loop()) {
      this->mp3AudioGenerator->stop();
      this->state == STATE_STOPPED; 
      // ADDING THIS CRASHES WIFI START
      delay(250);
      if (this->playlist != NULL && this->currentBook != NULL && !this->playlist->count() <= 0) {
        Serial.println("MP3: playing next track");
        this->next();
      }
      // ADDING THIS CRASHES WIFI END
    }
  } // WAS IN THE ELSE HERE
}

void MP3PlayerI2S::setVolume(uint8_t volume) {
  // the esp8266audio library accepts gain values between 0 and 4
  // we accept volumes between 0 and 10
  if (volume >= 0 && volume <= 10) {
    this->currentVolume = volume;
    float gain = ((float)this->currentVolume)*0.4;
    Serial.printf("MP3: setting gain to %f (volume %d)\n", gain, volume);
    this->i2sOutput->SetGain(gain);
  } else {
    Serial.printf("MP3: invalid volume value %d\n", volume);
  }
}

void MP3PlayerI2S::volumeUp() {
  if (this->currentVolume < 10) {
    Serial.println("MP3: increasing volume");
    this->currentVolume++;
    this->setVolume(this->currentVolume);
  } else {
    Serial.println("MP3: volume at maximum");
  }
}

void MP3PlayerI2S::volumeDown() {
  if (this->currentVolume > 0) {
    Serial.println("MP3: decreasing volume");
    this->currentVolume--;
    this->setVolume(this->currentVolume);
  } else {
    Serial.println("MP3: volume at minimum");
  }
}

uint8_t MP3PlayerI2S::getCurrentVolume() {
  return this->currentVolume;
}

void MP3PlayerI2S::pause() {
  Serial.println("MP3: stopping playback");
  //this->mp3AudioGenerator->stop();
  this->state = STATE_STOPPED;
}

void MP3PlayerI2S::resume() {
  Serial.println("MP3: resuming playback");
  //this->mp3AudioGenerator->begin();
  this->state = STATE_PLAYING;
}

void MP3PlayerI2S::playBook(Book* book) {
  Serial.printf("MP3: queuing book '%s'\n", book->id);    
  this->playBookFromTrackNumber(book, 1);
}

void MP3PlayerI2S::playBookFromTrack(Book* book, Track* track) {
  this->playBookFromTrackNumber(book, track->number);
}

void MP3PlayerI2S::playBookFromTrackNumber(Book* book, uint8_t trackNumber) {
  Serial.printf("MP3: queuing book '%s' from track %d\n", book->id, trackNumber);    
  if (book == NULL || trackNumber < 1) {
    Serial.println("MP3: given book or track is NULL or zero");
    return;
  }
  this->currentBook = book;
  if (trackNumber > this->currentBook->tracks.size() || trackNumber < 1) {
    Serial.printf("MP3: invalid track number %d\n", trackNumber);    
    return;
  }
  Serial.printf("MP3: adding book '%s' to playlist\n", book->id);    
  Serial.printf("MP3: creating new playlist with size %d\n", this->currentBook->tracks.size());    
  this->playlist->clear();
  Serial.printf("MP3: queuing tracks from book '%s' starting at track %d\n", this->currentBook->id, trackNumber);  
  for (uint8_t i=trackNumber-1; i<this->currentBook->tracks.size(); i++) {
    Serial.printf("MP3: queuing track %d (index %d) of book '%s' to playlist queue\n", i+1, i, this->currentBook->id);
    this->playlist->push(i);
  }
  Serial.println("MP3: completed queuing tracks");  
  this->next();
}

void MP3PlayerI2S::next() {
  Serial.println("MP3: next track");
  if (this->playlist == NULL || this->currentBook == NULL) {
    Serial.println("MP3: no book or playlist to play");
    return;
  }
  if (this->playlist->count() <= 0) {
    Serial.println("MP3: playlist is empty");
    this->state = STATE_STOPPED;
    return;
  }
  Serial.println("MP3: popping next track from queue");
  uint8_t thisTrackIndex = this->playlist->pop();
  if (thisTrackIndex > this->currentBook->tracks.size()-1 || thisTrackIndex < 0) {
    Serial.printf("MP3: invalid track number %d\n", thisTrackIndex);
    return;
  }
  this->currentTrackIndex = thisTrackIndex;
  Serial.printf("MP3: next track is track %d from playlist size %d\n", this->currentTrackIndex, this->currentBook->tracks.size());
  this->state = STATE_PLAYING;
  if (!(this->currentBook)) {
    Serial.println("MP3: book empty");
    return;
  }
  Track* thisTrack = this->currentBook->tracks[this->currentTrackIndex];
  const char* trackFilepath = thisTrack->filepath;
  Serial.printf("MP3: next track is '%s'\n", trackFilepath);
  if (this->currentTrackIndex+1 != thisTrack->number) {
    Serial.printf("MP3: track number and index+1 are different %d != %d\n", thisTrack->number, this->currentTrackIndex+1);
    return;
  }
  Serial.println("MP3: freeing resources");
  if (this->currentFileSourceID3 != NULL) {
    //delete this->currentFileSource;
    Serial.println("MP3: freeing AudioFileSourceID3");
    this->currentFileSourceID3->close();
  }
  if (this->currentFileSource != NULL) {
    //delete this->currentFileSource;
    Serial.println("MP3: freeing AudioFileSourceMMC");
    this->currentFileSource->close();
  }
  Serial.println("MP3: creating new AudioFileSourceMMC wait");
  delay(1000);
  Serial.println("MP3: creating new AudioFileSourceMMC");
  this->currentFileSource = new AudioFileSourceMMC(trackFilepath); // THIS CRASHES ON SECOND FILE ACCESS
  Serial.println("MP3: creating new AudioFileSourceMMC completed");
  /*
  if (this->currentFileSourceID3 != NULL) {
    delete this->currentFileSourceID3;
  }
  */
  Serial.println("MP3: creating new AudioFileSourceID3");
  this->currentFileSourceID3 = new AudioFileSourceID3(currentFileSource);
  Serial.println("MP3: registering metadataCallback");
  this->currentFileSourceID3->RegisterMetadataCB(this->metadataCallback, (void*)"ID3TAG");
  Serial.println("MP3: beginning playback");
  this->mp3AudioGenerator->begin(currentFileSourceID3, this->i2sOutput);
}

void MP3PlayerI2S::previous() {
  Serial.println("MP3: previous track NOT IMPLEMENTED");
  // TODO implement
}

void MP3PlayerI2S::restart() {
  Serial.println("MP3: restarting track NOT IMPLEMENTED");
  // TODO implement
}

Track* MP3PlayerI2S::getCurrentTrack() {
  if (this->currentTrackIndex > 0 && this->currentBook != NULL && this->currentBook->tracks.size() <= this->currentTrackIndex) {
    return this->currentBook->tracks[this->currentTrackIndex];
  } else {
    return NULL;
  }
}

Book* MP3PlayerI2S::getCurrentBook() {
  return this->currentBook;
}

uint8_t MP3PlayerI2S::getState() {
  // TODO implement
  return -1;
}

void MP3PlayerI2S::printStateToConsole() {
  // TODO implement
}
