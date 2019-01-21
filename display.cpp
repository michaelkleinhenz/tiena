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
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#include "display.h"

#define TFT_RST -1 
#define TFT_CS 4
#define TFT_DC 3

Display::Display() {
  // NOP
  this->tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
}

void Display::init() {
  this->tft->initR(INITR_144GREENTAB);
  this->clear();
}

void Display::displayTrackView(char* title, uint8_t track, uint8_t volume) {
  // TODO
}

void Display::clear() {
  this->tft->fillScreen(ST7735_BLACK);
  this->tft->setCursor(0, 0);
  delay(3000);
}

