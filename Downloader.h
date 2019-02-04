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
#include "ArduinoJson.h"

#ifndef DOWNLOADER_H
#define DOWNLOADER_H

typedef struct Track {
	uint8_t number;
	const char* title;
  const char* filename;
  const char* filepath;
  int byteSize;
  const char* md5sum;
  const char* url;
} Track;

typedef struct Book {
	const char* id;
  const char* title;
  const char* coverimage;
  std::vector<Track*> tracks;
} Book;

class Downloader {
  public:
    Downloader();
    boolean checkOrDownloadBook(const char* jsonPath, const char* url);
    Book* getBookDescriptorForId(const char* id);
    boolean formatCard();
    boolean removeBookById(const char* id);
  private:
    char* copyStr(const char* input);
    boolean remove(const char* path);
    boolean downloadFile(const char* url, const char* filepath);
    Book* parseDescriptorFile(const char* jsonPath);
    boolean checkOrDownloadBookData(const char* jsonPath);
};

#endif