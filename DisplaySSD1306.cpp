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
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "DisplaySSD1306.h"

#define DISPLAY_RST -1 
#define DISPLAY_I2CADDR 0x3C

DisplaySSD1306::DisplaySSD1306() {
  this->display = new Adafruit_SSD1306(DISPLAY_RST);
}

void DisplaySSD1306::init() {
  this->display->clearDisplay();
  this->display->display();
}

void DisplaySSD1306::displayTrackView(char* title, uint8_t track, uint8_t volume) {
  // TODO
}

void DisplaySSD1306::clear() {
  this->display->clearDisplay();
  this->display->display();
}

