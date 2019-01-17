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

#include "mp3.h"

#define MP3_RX 26
#define MP3_TX 13

// NOTE: this implementation has a lot of delay() calls.
// this is needed because the serial comm will get unstable
// if commands are sent too fast.

MP3Player::MP3Player()
{
  // NOP
}

void MP3Player::init()
{
  Serial1.begin(9600, SERIAL_8N1, MP3_RX, MP3_TX);
  delay(1000); // wait for the serial init to settle
  Serial.println(F("Initializing MP3 Module..."));
  if (!this->mp3PlayerInstance.begin(Serial1))
  {
    Serial.println(F("FATAL: Unable to initialize MP3 module!"));
    while (true)
      ;
  }
  Serial.println(F("MP3 Module ready."));
  delay(1000);
}

void MP3Player::setVolume(uint8_t volume)
{
  this->mp3PlayerInstance.volume(volume);
  delay(1000);
  Serial.printf("MP3: Setting Volume to %d\n", this->mp3PlayerInstance.readVolume());
  delay(1000);
}

void MP3Player::volumeUp()
{
  this->mp3PlayerInstance.volumeUp();
  delay(1000);
  Serial.printf("MP3: Setting Volume to %d\n", this->mp3PlayerInstance.readVolume());
  delay(1000);
}

void MP3Player::volumeDown()
{
  this->mp3PlayerInstance.volumeDown();
  delay(1000);
  Serial.printf("MP3: Setting Volume to %d\n", this->mp3PlayerInstance.readVolume());
  delay(1000);
}

void MP3Player::playFolder(uint8_t folderNumber, boolean looped)
{
  if (looped)
  {
    this->mp3PlayerInstance.loopFolder(folderNumber);
  }
  else
  {
    this->mp3PlayerInstance.playFolder(folderNumber, 1);
  }
  delay(1000);
  Serial.printf("MP3: Playing Track %d\n", this->mp3PlayerInstance.readCurrentFileNumber());
  delay(1000);
}

void MP3Player::stop()
{
  this->mp3PlayerInstance.stop();
  Serial.printf("MP3: Stop\n");
  delay(1000);
}

void MP3Player::pause()
{
  this->mp3PlayerInstance.pause();
  Serial.printf("MP3: Pause\n");
  delay(1000);
}

void MP3Player::play()
{
  this->mp3PlayerInstance.start();
  delay(1000);
  Serial.printf("MP3: Playing Track %d\n", this->mp3PlayerInstance.readCurrentFileNumber());
  delay(1000);
}

void MP3Player::playTrack(int track)
{
  this->mp3PlayerInstance.play(track);
  delay(1000);
  Serial.printf("MP3: Playing Track %d\n", this->mp3PlayerInstance.readCurrentFileNumber());
  delay(1000);
}

void MP3Player::next()
{
  this->mp3PlayerInstance.next();
  delay(1000);
  Serial.printf("MP3: Playing Track %d\n", this->mp3PlayerInstance.readCurrentFileNumber());
  delay(1000);
}

void MP3Player::previous()
{
  this->mp3PlayerInstance.previous();
  delay(1000);
  Serial.printf("MP3: Playing Track %d\n", this->mp3PlayerInstance.readCurrentFileNumber());
  delay(1000);
}

uint8_t MP3Player::getNumFolders()
{
  uint8_t value = this->mp3PlayerInstance.readFileCounts();
  delay(1000);
  return value;
}

uint8_t MP3Player::getCurrentFileNumber()
{
  uint8_t value = this->mp3PlayerInstance.readCurrentFileNumber();
  delay(1000);
  return value;
}

uint8_t MP3Player::getState()
{
  uint8_t value = this->mp3PlayerInstance.readState();
  delay(1000);
  return value;
}

uint8_t MP3Player::getCurrentVolume()
{
  uint8_t value = this->mp3PlayerInstance.readVolume();
  delay(1000);
  return value;
}

void MP3Player::printInfoToConsole() {
  Serial.printf("MP3 Status Info:\n");
  Serial.printf("State: %d\n", this->mp3PlayerInstance.readState()); //read mp3 state
  Serial.printf("Volume: %d\n", this->mp3PlayerInstance.readVolume()); //read current volume
  Serial.printf("EQ Setting: %d\n", this->mp3PlayerInstance.readEQ()); //read EQ setting
  Serial.printf("File Count: %d\n", this->mp3PlayerInstance.readFileCounts()); //read all file counts in SD card
  Serial.printf("Current File: %d\n", this->mp3PlayerInstance.readCurrentFileNumber()); //read current play file number
  delay(1000);
}

void MP3Player::printStateToConsole()
{
  if (this->mp3PlayerInstance.available())
  {
    uint8_t type = this->mp3PlayerInstance.readType();
    int value = this->mp3PlayerInstance.read();
    switch (type)
    {
    case TimeOut:
      Serial.println(F("MP3: Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("MP3: Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("MP3: Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("MP3: Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("MP3: Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("MP3: Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("MP3 Error:"));
      switch (value)
      {
      case Busy:
        Serial.println(F("Card not found"));
        break;
      case Sleeping:
        Serial.println(F("Sleeping"));
        break;
      case SerialWrongStack:
        Serial.println(F("Get Wrong Stack"));
        break;
      case CheckSumNotMatch:
        Serial.println(F("Check Sum Not Match"));
        break;
      case FileIndexOut:
        Serial.println(F("File Index Out of Bound"));
        break;
      case FileMismatch:
        Serial.println(F("Cannot Find File"));
        break;
      case Advertise:
        Serial.println(F("In Advertise"));
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
  delay(1000);
}
