/*
  MMCAudioFile
  Support for SD_MMC card files to be used by AudioGenerator
  Extension to the ESP8266Audio Library 
*/

#include "Arduino.h"
#include "SD_MMC.h"

#include "AudioFileSource.h"

#ifndef AUDIOFILESOURCEMMC_H
#define AUDIOFILESOURCEMMC_H

class AudioFileSourceMMC : public AudioFileSource
{
  public:
    AudioFileSourceMMC();
    AudioFileSourceMMC(const char *filename);
    virtual ~AudioFileSourceMMC() override;  
    virtual bool open(const char *filename) override;
    virtual uint32_t read(void *data, uint32_t len) override;
    virtual bool seek(int32_t pos, int dir) override;
    virtual bool close() override;
    virtual bool isOpen() override;
    virtual uint32_t getSize() override;
    virtual uint32_t getPos() override;
  private:
    File audiofile;
};

#endif

