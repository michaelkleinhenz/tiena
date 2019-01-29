/*
  AudioFileSourceMMC
  Support for SD_MMC card files to be used by AudioGenerator
  Extension to the ESP8266Audio Library 
*/

#include "AudioFileSourceMMC.h"

AudioFileSourceMMC::AudioFileSourceMMC() {
  // NOP
}

AudioFileSourceMMC::AudioFileSourceMMC(const char *filename) {
  open(filename);
}

bool AudioFileSourceMMC::open(const char *filename) {
  audiofile = SD_MMC.open(filename, FILE_READ);
  return audiofile;
}

AudioFileSourceMMC::~AudioFileSourceMMC() {
  if (audiofile) 
    audiofile.close();
}

uint32_t AudioFileSourceMMC::read(void *data, uint32_t len) {
  return audiofile.read(reinterpret_cast<uint8_t*>(data), len);
}

bool AudioFileSourceMMC::seek(int32_t pos, int dir) {
  if (!audiofile) 
    return false;
  if (dir==SEEK_SET) 
   return audiofile.seek(pos);
  else if (dir==SEEK_CUR) 
   return audiofile.seek(f.position() + pos);
  else if (dir==SEEK_END) 
   return audiofile.seek(f.size() + pos);
  else
   return false;
}

bool AudioFileSourceMMC::close() {
  audiofile.close();
  return true;
}

bool AudioFileSourceMMC::isOpen() {
  return audiofile?true:false;
}

uint32_t AudioFileSourceMMC::getSize() {
  if (!audiofile) 
    return 0;
  return audiofile.size();
}

uint32_t AudioFileSourceMMC::getPos() {
  if (!audiofile) 
    return 0;
  return audiofile.position();
}
