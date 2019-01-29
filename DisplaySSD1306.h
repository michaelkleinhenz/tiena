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
#include "Adafruit_SSD1306.h"

#ifndef DISPLAYSSD1306_H
#define DISPLAYSSD1306_H

class DisplaySSD1306 {
  public:
    DisplaySSD1306();
    void init();
    void displayTrackView(char* title, uint8_t track, uint8_t volume);
    void clear();
  private:
    Adafruit_SSD1306 *display;
};

#endif