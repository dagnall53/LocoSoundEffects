#include "Chuff.h"
//#define AudioDebug;


#define _AudioNoDAC


                       //see   https://github.com/earlephilhower/ESP8266Audio
  #include "AudioFileSourceSPIFFS.h"      
  #include "AudioGeneratorWAV.h"
  #include "AudioOutputMixer.h"
#ifdef _AudioDAC
  #include "AudioOutputI2S.h"
#endif
#ifdef _AudioNoDAC
  #include "AudioOutputI2SNoDAC.h"
  #endif 


int ChuffCycle;  
long LastChuff;
bool WavDoneMsgSent;
bool PlayingSoundEffect;
bool ChuffPlaying;
extern bool Audio_Setup_Problem;

AudioGeneratorWAV *wav[2];
AudioFileSourceSPIFFS *file[2];

AudioOutputMixer *mixer;
AudioOutputMixerStub *stub[2];

#ifdef _AudioDAC
  AudioOutputI2S *out;
#endif
 #ifdef _AudioNoDAC
  AudioOutputI2SNoDAC *out;
#endif

void SetUPAudio(void){ 
 // out = new AudioOutputI2SNoDAC();
   #ifdef _AudioDAC
  out = new AudioOutputI2S();     Serial.printf("-- Using I2S DAC -- \n");
  #endif
 #ifdef _AudioNoDAC
  out = new AudioOutputI2SNoDAC(); Serial.printf("-- Using I2S No DAC -- \n");
#endif
  mixer = new AudioOutputMixer(128, out);
 // wav = new AudioGeneratorWAV();
  WavDoneMsgSent=false;
  ChuffCycle=0;
  LastChuff=millis()+1000;
  Serial.printf("-- Sound System Initiating -- \n");
  //BeginPlay("/initiated.wav");// this wav file will play before anything else.
  //BeginPlay("/Class 4 Guard's Whistle.wav");



   BeginPlayND (0,"/F1.wav",100);//Bell   this wav file will play before anything else, setting up channel 0 but does not do the file, stub,wav deletes, because nothing should be open yet.
   BeginPlayND (1,"/F2.wav",64);//   this wav file will play next before anything else, setting up channel 1.

  PlayingSoundEffect=true;   
  ChuffPlaying=false;
  delay(500); // allow time for setups..
}
bool TimeToChuff(long Period){
   if (millis()<=(LastChuff+Period)){return false;}
    else {LastChuff=millis();return true;}
    }


void BeginPlay (int Channel,const char *wavfilename, uint8_t CVVolume){
  if (!FileExists(wavfilename)){
     #ifdef _SERIAL_Audio_DEBUG
               Serial.print(" .wav file missing");
          #endif
                               return;
                               }

  if (wav[Channel]->isRunning()) {   wav[Channel]->stop();  //delay(1);
          #ifdef _SERIAL_Audio_DEBUG
               Serial.print(" Truncated to restart");
          #endif
                   }//truncate a playing wav 

  
  float Volume;
   Volume=(float)CVVolume*128/16384;  // 128 was overall volume control (CV[100])
    
  delete stub[Channel]; //housekeeping ?   
    stub[Channel] = mixer->NewInput();
    stub[Channel]->SetGain(Volume);
                             
  delete file[Channel]; //housekeeping ?   
    file[Channel]= new AudioFileSourceSPIFFS(wavfilename);
   
  delete wav[Channel]; //housekeeping ?   
    wav[Channel] = new AudioGeneratorWAV();
    wav[Channel]->begin(file[Channel], stub[Channel]); 
  
  //ChuffDebugState=1;
  #ifdef _LOCO_SERVO_Driven_Port  
  if (Channel==1){PlayingSoundEffect=true;  // Stops any more playing on this channel until sound is finished //if ch1 (SFX) works ch 0 (Chuffs) will, but we do not need lots of debug messages for chuffs
                  Volume=Volume*100; // convert to % just for message
                 // DebugSprintfMsgSend( sprintf ( DebugMsg, " Playing SFX file <%s> at %.0f%% volume",wavfilename,Volume));
                 }
   #else   // options for stationary nodes
                  Volume=Volume*100; // convert to % just for message
                  //DebugSprintfMsgSend( sprintf ( DebugMsg, " Playing SFX file <%s> at %.0f%% volume",wavfilename,Volume));
                 
   #endif              
}

void BeginPlayND (int Channel,const char *wavfilename, uint8_t CVVolume){ 
  //"no deletes" version that is used at startup to set thing s
 if (!FileExists(wavfilename)){
  Audio_Setup_Problem=true;
  Serial.print("WAV File [");Serial.print(wavfilename);
  Serial.println("] Missing- Switching off Audio");
  return;
     }
  uint32_t NOW;
  NOW=micros();
  float Volume;
  Volume=(float)CVVolume*128/16384;  // the 128 here was CV[100].. the overall gain control 
// #ifndef _LOCO_SERVO_Driven_Port
  Volume=0.9; // difficult to access the CV's if not a loco, so just use 90% 
// #endif
  //delete stub[Channel];
  stub[Channel] = mixer->NewInput();
  stub[Channel]->SetGain(Volume);

 //if (wav[Channel]->isRunning()) {wav[Channel]->stop();
 //Serial.printf("*audio previous wav stopped\n");
   //                      }
                        
  //delete file[Channel]; //housekeeping ?
  file[Channel]= new AudioFileSourceSPIFFS(wavfilename);
  
  //delete wav[Channel];
  wav[Channel] = new AudioGeneratorWAV();
  wav[Channel]->begin(file[Channel], stub[Channel]);  
  PlayingSoundEffect=true;
 
  Serial.printf(" Setting up First SFX play CH:");
  Serial.print(Channel);
  Serial.printf("  Playing <");
  Serial.print(wavfilename);  
  Serial.printf("> at volume :");
  Serial.println(Volume);

}


void Chuff (void){
  if (!PlayingSoundEffect){
   if (wav[0]->isRunning()) {wav[0]->stop(); //delete file; delay(1);
                          }// truncate play
   switch (ChuffCycle){ LastChuff=millis();
                        
                              case 0:BeginPlay(0,"/ch1.wav",128);ChuffCycle=1;break;
                              case 1:BeginPlay(0,"/ch2.wav",128);ChuffCycle=2;break;
                              case 2:BeginPlay(0,"/ch3.wav",128);ChuffCycle=3;break;
                              case 3:BeginPlay(0,"/ch4.wav",128);ChuffCycle=0;break;
}}
}
void AudioLoop(int32_t TimeNow){
if (!Audio_Setup_Problem) {
  
 #ifdef SteamOutputPin
              if ((SteamOnStarted+SteamPulseDuration)<=TimeNow){digitalWrite(NodeMCUPinD[SteamOutputPin],LOW);}
 #endif 

bool SoundPlaying;

SoundPlaying=false;   
   if (wav[0]->isRunning()) { //ch 0 is chuffs
              SoundPlaying=true; 
              if (!wav[0]->loop()) { wav[0]->stop(); stub[0]->stop();//Running[0]=false;Serial.printf("stopping 0\n"); }
                          #ifdef _SERIAL_Audio_DEBUG 
                              Serial.printf("S0\n");
                          #endif
                         }
                        }

  if (wav[1]->isRunning()) {
              SoundPlaying=true; 
              //ChuffDebugState=2;
              if (!wav[1]->loop()) { //ChuffDebugState=3;
                            wav[1]->stop(); stub[1]->stop();
                            PlayingSoundEffect=false;//Running[1]=false;Serial.printf("stopping 1\n");}
                            #ifdef _SERIAL_Audio_DEBUG
                              Serial.printf("S1\n");
                            #endif
                            }
                           }else {//ChuffDebugState=4;
                                 if (PlayingSoundEffect) {PlayingSoundEffect=false;//ChuffDebugState=5;
                           #ifdef _SERIAL_Audio_DEBUG
                              Serial.printf("S3\n");
                            #endif
                           }
                            }
  }
}


bool SoundEffectPlaying(void){
    return PlayingSoundEffect;
    }
  
bool FileExists(const char *wavfilename){

// esp8266 version 
  File TestObj = SPIFFS.open(wavfilename,"r");
    if ( !TestObj ){ 
      Serial.print(" SFX File not found [");Serial.print(wavfilename);Serial.println("]");
        malloc(TestObj);  //  this needed 
        return false; 
        }
 malloc(TestObj); 
 return true;
}
 
