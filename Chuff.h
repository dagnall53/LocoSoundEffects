#include "AudioFileSourceSPIFFS.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SDAC.h"


bool TimeToChuff(long Period);
void SetUpChuff(void);
void BeginPlay(const char *wavfilename);
void Chuff(void);
void AudioLoop(int SoundToPlay);
bool SoundEffectPlaying(void);

/*expected wav files are:
 Initiate sound:
   /initiated.wav
 
 CHUFFS:
  /ch1.wav
  /ch2.wav
  /ch3.wav
  /ch4.wav
Whistle:
  /whistle.wav
Brake Squeal
  /brakes.wav




*/
