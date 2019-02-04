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
#include "FS.h"
#include "SD_MMC.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"

#include "Downloader.h"

Downloader::Downloader() {
  // NOP
}

boolean Downloader::remove(const char* path) {
  File root = SD_MMC.open(path);
  if (!root) {
    Serial.printf("DOWNLOADER: failed to find path '%s' for deletion\n", path);
    return false;
  }
  if (root.isDirectory()) {
    boolean success = true;
    File file = root.openNextFile();
    while (file) {
      boolean localSuccess = remove(file.name());
      success = success && localSuccess;
      file = root.openNextFile();
    }
    Serial.printf("DOWNLOADER: removing directory '%s'\n", path);
    if (SD_MMC.rmdir(path)){
      Serial.printf("DOWNLOADER: directory '%s' removed\n", path);
      return true && success;
    } else {
      Serial.printf("DOWNLOADER: error removing directory '%s'\n", path);
      return false;
    }
  } else {
    root.close();
    Serial.printf("DOWNLOADER: removing file '%s'\n", path);
    if (SD_MMC.remove(path)) {
      Serial.printf("DOWNLOADER: file '%s' removed\n", path);
      return true;
    } else {
      Serial.printf("DOWNLOADER: error removing file '%s'\n", path);
      return false;
    }
  }
}

boolean Downloader::removeBookById(const char* id) {
  std::string jsonPath = std::string() + "/" + id + ".json";
  char *jsonPath_cstr = new char[jsonPath.length() + 1];
  strcpy(jsonPath_cstr, jsonPath.c_str());
  Serial.printf("DOWNLOADER: removing book descriptor '%s'\n", jsonPath_cstr);
  this->remove(jsonPath_cstr);
  std::string bookDir = std::string() + "/" + id;
  char *bookDir_cstr = new char[bookDir.length() + 1];
  strcpy(bookDir_cstr, bookDir.c_str());
  Serial.printf("DOWNLOADER: removing book directory '%s'\n", bookDir_cstr);
  File root = SD_MMC.open(bookDir_cstr);
  if (!root) {
    Serial.printf("DOWNLOADER: failed to open book directory '%s'\n", bookDir_cstr);
    return false;
  }
  if (!root.isDirectory()) {
    Serial.printf("DOWNLOADER: book directory '%s' is not a directory\n", bookDir_cstr);
    return false;
  }
  boolean success = this->remove(bookDir_cstr);
  if (!success) {
    Serial.printf("DOWNLOADER: error removing book '%s'\n", id);
  } else {
    Serial.printf("DOWNLOADER: book '%s' removed\n", id);
  }
}

boolean Downloader::formatCard() {
  File root = SD_MMC.open("/");
  if (!root) {
    Serial.println("DOWNLOADER: failed to open root directory");
    return false;
  }
  if (!root.isDirectory()) {
    Serial.println("DOWNLOADER: root is not a directory");
    return false;
  }
  boolean success = true;
  // we iterate over the childs of / here as we don't want to remove / itself
  File file = root.openNextFile();
  while (file) {
    boolean localSuccess = this->remove(file.name());
    success = success && localSuccess;
    file = root.openNextFile();
  }
  return success;
}

boolean Downloader::downloadFile(const char* url, const char* filepath) {
  Serial.printf("DOWNLOADER: starting download '%s' to '%s'\n", url, filepath);
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      File f = SD_MMC.open(filepath, FILE_WRITE);
      if (f) {
        int written = 0; 
        // get lenght of document (is -1 when Server sends no Content-Length header)
        int len = http.getSize();
        // create buffer for read
        uint8_t buff[128] = { 0 };
        // get tcp stream
        WiFiClient * stream = http.getStreamPtr();
        // read all data from server
        while (http.connected() && (len > 0 || len == -1)) {
          // get available data size
          size_t size = stream->available();
          if(size) {
            // read up to 128 byte
            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
            // write it to file
            f.write(buff, c);
            if(len > 0) {
              len -= c;
              written += c;
            }
          }
          delay(1);
        }
        Serial.printf("DOWNLOADER: connection closed or file end, %d bytes written\n", written);
        f.close();
        return true;
      } else {
        Serial.println("DOWNLOADER: open file failed");
        return false;
      }
    }
  } else {
    Serial.printf("DOWNLOADER: GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    return false;
  }
  http.end();
  Serial.printf("DOWNLOADER: ending download, http result %d\n", httpCode);
  return true;
}

char* Downloader::copyStr(const char* input) {
  std::string temp = std::string() + input;
  char *output = new char[temp.length() + 1];
  strcpy(output, temp.c_str());
  return output;
}

Book* Downloader::parseDescriptorFile(const char* jsonPath) {
  // open descriptor, read into string
  String fileContent;
  File f = SD_MMC.open(jsonPath, FILE_READ);
  if (f && f.size()) {
    Serial.printf("DOWNLOADER: read descriptor file '%s'\n", jsonPath);    
    while (f.available()){
      fileContent += char(f.read());
    }
    f.close();
  } else {
    Serial.println("DOWNLOADER: descriptor file read failed");
    return NULL;
  }
  Serial.println("DOWNLOADER: read descriptor file completed");
  // parse json from string
  DynamicJsonBuffer jsonBuffer;
  Serial.println("DOWNLOADER: parsing descriptor file");
  JsonObject& root = jsonBuffer.parseObject(fileContent);
  Book* book = new Book();
  book->id = this->copyStr(root["id"]);
  book->title = this->copyStr(root["title"]);
  book->coverimage = this->copyStr(root["coverimage"]);
  book->tracks = std::vector<Track*>(root["tracks"].size());
  for (int i=0; i<root["tracks"].size(); i++) {
    JsonObject& track = root["tracks"][i];
    book->tracks[i] = new Track();
    book->tracks[i]->number = track["number"];
    book->tracks[i]->title = this->copyStr(track["title"]);
    book->tracks[i]->filename = this->copyStr(track["filename"]);
    std::string track_path = std::string() + "/" + book->id + "/" + book->tracks[i]->filename;
    char *track_path_cstr = new char[track_path.length() + 1];
    strcpy(track_path_cstr, track_path.c_str());
    book->tracks[i]->filepath = track_path_cstr;
    book->tracks[i]->byteSize = track["bytes"];
    book->tracks[i]->md5sum = this->copyStr(track["md5sum"]);
    book->tracks[i]->url = this->copyStr(track["url"]);
    Serial.printf("DOWNLOADER: parsed track for book '%s': %d - '%s' in path '%s'\n", book->id, book->tracks[i]->number, book->tracks[i]->title, book->tracks[i]->filepath);
  }
  return book;
}

boolean Downloader::checkOrDownloadBookData(const char* jsonPath) {
  Book* book = this->parseDescriptorFile(jsonPath);
  Serial.printf("DOWNLOADER: parsed descriptor for book '%s' ('%s') in path '%s'\n", book->id, book->title, jsonPath);
  // create the folder if it does not exist yet, downloadFile() expects the path to be existing
  std::string book_folder = std::string() + "/" + book->id;
  char *book_folder_cstr = new char[book_folder.length() + 1];
  strcpy(book_folder_cstr, book_folder.c_str());
  SD_MMC.mkdir(book_folder_cstr);
  // check or download the files
  boolean success = true;
  for (int i=0; i<book->tracks.size(); i++) {
    Serial.printf("DOWNLOADER: checking file %d of book id '%s'\n", i, book->id);
    Track* track = book->tracks[i];
    File f = SD_MMC.open(track->filepath, FILE_READ);
    if (f && f.size()==track->byteSize) {
      f.close();
      Serial.printf("DOWNLOADER: track '%s' already exists\n", track->filepath);
    } else {
      f.close();
      Serial.printf("DOWNLOADER: track '%s' does not exist or has wrong size, downloading from '%s'\n", track->filepath, track->url);
      boolean fileDownloadSuccess = this->downloadFile(track->url, track->filepath);
      success = success && fileDownloadSuccess;
    }
  }
  return success;
}

boolean Downloader::checkOrDownloadBook(const char* jsonPath, const char* url) {
  File f = SD_MMC.open(jsonPath, FILE_READ);
  if (f && f.size()) {
    Serial.printf("DOWNLOADER: descriptor file already available '%s'\n", jsonPath);    
    f.close();
  } else {
    Serial.println("DOWNLOADER: descriptor file not available, downloading..");
    if (!this->downloadFile(url, jsonPath)) {
      Serial.println("DOWNLOADER: download descriptor file failed");
      return false;
    };
  }
  Serial.println("DOWNLOADER: checking book data..");
  if (!this->checkOrDownloadBookData(jsonPath)) {
      Serial.println("DOWNLOADER: download descriptor file failed");
      return false;
  } else {
    Serial.println("DOWNLOADER: book check complete");
    return true;
  }
}

Book* Downloader::getBookDescriptorForId(const char* id) {
  String path;
  path = path + "/" + id + ".json";  
  Book* book = this->parseDescriptorFile(path.c_str());
  if (book == NULL) {
    Serial.printf("DOWNLOADER: descriptor file for id '%s' does not exist\n", id);
    return NULL;
  }
  Serial.printf("DOWNLOADER: parsed descriptor for book '%s' in path '%s'\n", id, path.c_str());
  return book;
};
