#include "Chuff.h"
//#define AudioDebug;
int ChuffCycle;  
long LastChuff;
bool WavDoneMsgSent;
bool PlayingSoundEffect;
bool ChuffPlaying;
AudioGeneratorWAV *wav;
AudioFileSourceSPIFFS *file;
AudioOutputI2S *out;

void SetUpChuff(void){ 
  out = new AudioOutputI2S();
  wav = new AudioGeneratorWAV();
  WavDoneMsgSent=false;
  ChuffCycle=0;
  LastChuff=millis()+1000;
  Serial.printf("-- Sound System Initiating -- \n");
  //BeginPlay("/initiated.wav");// this wav file will play before anything else.
  BeginPlay("/Class 4 Guard's Whistle.wav");
  PlayingSoundEffect=true;   
  ChuffPlaying=false;
  delay(500); // allow time for setups..
}
bool TimeToChuff(long Period){
   if (millis()<=(LastChuff+Period)){return false;}
    else {LastChuff=millis();return true;}
    }
void BeginPlay(const char *wavfilename){
       PlayingSoundEffect=true;WavDoneMsgSent=false;
       if ( (wavfilename=="/ch1.wav")||
            (wavfilename=="/ch2.wav")||
            (wavfilename=="/ch3.wav")||
            (wavfilename=="/ch4.wav")   )
           {PlayingSoundEffect=false;
            ChuffPlaying=true;
            #ifdef AudioDebug
            Serial.printf("Chuff");
            #endif
           }
       else {
        if (wav->isRunning()) {
                           wav->stop(); delete file;
                              #ifdef AudioDebug
                              Serial.printf("-Stopped\n");
                              #endif
                  }
        #ifdef AudioDebug 
        Serial.print(wavfilename);
        #endif
            }
    
                  
     file = new AudioFileSourceSPIFFS(wavfilename);
     wav->begin(file, out);
  
  
  }

void Chuff (void){
  if (!PlayingSoundEffect){
   if (wav->isRunning()) {wav->stop(); delete file; delay(1);
                           #ifdef AudioDebug
                              Serial.printf("-Truncated\n");
                              #endif
                          }// truncate play
   switch (ChuffCycle){ LastChuff=millis();
                        
                              case 0:BeginPlay("/ch1.wav");ChuffCycle=1;break;
                              case 1:BeginPlay("/ch2.wav");ChuffCycle=2;break;
                              case 2:BeginPlay("/ch3.wav");ChuffCycle=3;break;
                              case 3:BeginPlay("/ch4.wav");ChuffCycle=0;break;
}}
}
void AudioLoop(int x){
  if (wav->isRunning()) {
                         if (!wav->loop()) {wav->stop();}
                        } 
             else {
                  if (!WavDoneMsgSent){
                     WavDoneMsgSent=true;  
                     PlayingSoundEffect=false;
                     delete file;
                     #ifdef AudioDebug
                     Serial.printf(" -WAV done\n");
                     #endif
                     }
                    }
}
  bool SoundEffectPlaying(void){
    return PlayingSoundEffect;
    }
  
  
