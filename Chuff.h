#ifndef Chuff_h 
 #define Chuff_h

#include <Arduino.h> //needed 



bool TimeToChuff(long Period);
void SetUPAudio(void);

void Chuff(void);
void AudioLoop(int SoundToPlay);
bool SoundPlaying (int ch);
void BeginPlay(int Channel,const char *wavfilename, uint8_t CVVolume);
void BeginPlayND(int Channel,const char *wavfilename, uint8_t CVVolume); //no deletes version used for the first play. after this can delete opened stuff
bool FileExists(const char *wavfilename);
void NEWChuff(String ChuffChoice, String ChuffChoiceFast,long ChuffSwitchSpeed);

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
#endif
