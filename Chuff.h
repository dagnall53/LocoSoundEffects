#ifndef Chuff_h 
 #define Chuff_h

#include <Arduino.h> //needed 



bool TimeToChuff(long Period);
void SetUPAudio(void);

void Chuff (String ChuffChoice);
void AudioLoop(int SoundToPlay);
bool SoundPlaying (int ch);
void BeginPlay(int Channel,const char *wavfilename, uint8_t CVVolume);
void BeginPlayND(int Channel,const char *wavfilename, uint8_t CVVolume); //no deletes version used for the first play. after this can delete opened stuff
bool FileExists(const char *wavfilename);
void NEWChuff(String ChuffChoice, String ChuffChoiceFast,long ChuffSwitchSpeed);
void SetChuffPeriod(uint8_t Speed, int WavsPerRevolution );
/*




*/
#endif
