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
extern bool SoundPlaying1;
extern bool SoundPlaying0;
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

   PlayingSoundEffect=false;
   ChuffPlaying=false;
   BeginPlayND (0,"/F1.wav",100);//F1 is ivor    this wav file will play before anything else, setting up channel 0 but does not do the file, stub,wav deletes, because nothing should be open yet.
  
   BeginPlayND (1,"/F1.wav",100);//   this wav file will play next before anything else, setting up channel 1.

  //PlayingSoundEffect=true;   
  
  delay(1500); // allow time for setups..
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
//  if (!PlayingSoundEffect){
   if (wav[0]->isRunning()) {wav[0]->stop(); //delete file; delay(1);
                          }// truncate play
   switch (ChuffCycle){ LastChuff=millis();
                        
                              case 0:BeginPlay(0,"/ch1.wav",128);ChuffCycle=1;break;
                              case 1:BeginPlay(0,"/ch2.wav",128);ChuffCycle=2;break;
                              case 2:BeginPlay(0,"/ch3.wav",128);ChuffCycle=3;break;
                              case 3:BeginPlay(0,"/ch4.wav",128);ChuffCycle=0;break;
}//}
}
void AudioLoop(int32_t TimeNow){
if (!Audio_Setup_Problem) {
  
 #ifdef SteamOutputPin
              if ((SteamOnStarted+SteamPulseDuration)<=TimeNow){digitalWrite(NodeMCUPinD[SteamOutputPin],LOW);}
 #endif 

     SoundPlaying0=false; 
     SoundPlaying1=false;  
  if (wav[0]->isRunning()) { //ch 0 is chuffs
              SoundPlaying0=true; 
              if (!wav[0]->loop()) { wav[0]->stop(); stub[0]->stop();//Running[0]=false;Serial.printf("stopping 0\n"); }
                          #ifdef _SERIAL_Audio_DEBUG 
                              Serial.printf("S0\n");
                          #endif
                         }
                        }

  if (wav[1]->isRunning()) {
              SoundPlaying1=true; 
              if (!wav[1]->loop()) { 
                            wav[1]->stop(); stub[1]->stop();
                            PlayingSoundEffect=false;//Running[1]=false;Serial.printf("stopping 1\n");}
                            #ifdef _SERIAL_Audio_DEBUG
                              Serial.printf("S1\n");
                            #endif
                            }
                           }else {
                                 if (PlayingSoundEffect) {PlayingSoundEffect=false;
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

void NEWChuff(String ChuffChoice, String ChuffChoiceFast,long ChuffSwitchSpeed){
#if defined (_Audio) && defined (_LOCO_SERVO_Driven_Port)
  String Chuff;
  float Speed;
  Speed =(10000)/(Chuff_wav_period*Wavs_Per_Revolution);  // ~250ms "4 Chuffs_wav_period" at 10MPH
   if((ChuffsOn())&& (POWERON)){ //F9 is chuffs on
   if (wav[0]->isRunning()) {wav[0]->stop();  //delay(1);
                      #ifdef _SERIAL_Audio_DEBUG
                        Serial.print("Chuff -Trunc- ");
                      #endif
                     }//truncate play
   
   
   #ifdef SteamOutputPin  //steamoutputpin stuff  here for one puff per wav send 
      SteamOnStarted=millis(); digitalWrite(NodeMCUPinD[SteamOutputPin],HIGH); //steamoutputpin stuff  here for one puff per chuff 
   #endif

   if (Speed <= ChuffSwitchSpeed){ 
                 Wavs_Per_Revolution=4;
                 if(Last_Wavs_Per_Revolution != Wavs_Per_Revolution){
                    SetChuffPeriod(Last_DCC_Speed_Demand,Wavs_Per_Revolution);  delay(1); // revise chuff timing, use last stored speed setting
                                          Last_Wavs_Per_Revolution=Wavs_Per_Revolution;  }
                 
                 switch (ChuffCycle){
                  
                              case 0:Chuff=ChuffChoice+"1.wav";BeginPlay(0,Chuff.c_str(),CV[110]);ChuffCycle=1;
                                  //Stuff here only for strobe use, one per rev to help set chuff rate
                                  //SteamOnStarted=millis(); digitalWrite(NodeMCUPinD[SteamOutputPin],HIGH);
                                                                                                              break;
                              case 1:Chuff=ChuffChoice+"2.wav"; if (FileExists(Chuff.c_str())){BeginPlay(0,Chuff.c_str(),CV[110]);ChuffCycle=2;}
                                                                   else{Chuff=ChuffChoice+"1.wav";BeginPlay(0,Chuff.c_str(),CV[110]);ChuffCycle=1;} break;
                              case 2:Chuff=ChuffChoice+"3.wav";if (FileExists(Chuff.c_str())){BeginPlay(0,Chuff.c_str(),CV[110]);ChuffCycle=3;}
                                                                   else{Chuff=ChuffChoice+"1.wav";BeginPlay(0,Chuff.c_str(),CV[110]);ChuffCycle=1;} break;
                              case 3:Chuff=ChuffChoice+"4.wav";if (FileExists(Chuff.c_str())){BeginPlay(0,Chuff.c_str(),CV[110]);ChuffCycle=0;}
                                                                   else{Chuff=ChuffChoice+"1.wav";BeginPlay(0,Chuff.c_str(),CV[110]);ChuffCycle=1;} break;
                        }
                }else{
                  Wavs_Per_Revolution=1;
                  if(Last_Wavs_Per_Revolution != Wavs_Per_Revolution){
                           SetChuffPeriod(Last_DCC_Speed_Demand,Wavs_Per_Revolution);  delay(1); // revise chuff timing
                                        Last_Wavs_Per_Revolution=Wavs_Per_Revolution; }
                  switch (ChuffCycle){ 
                              case 0:Chuff=ChuffChoiceFast+"1.wav";BeginPlay(0,Chuff.c_str(),CV[110]);ChuffCycle=0; // select which wav phase sound best when running fast, or use a completely separate fast wav file
                              //Stuff here only for strobe use, one per rev to help set chuff rate
                                  //SteamOnStarted=millis(); digitalWrite(NodeMCUPinD[SteamOutputPin],HIGH);
                              break;
                              case 1:Chuff=ChuffChoiceFast+"1.wav";BeginPlay(0,Chuff.c_str(),CV[110]);ChuffCycle=0;break;
                              case 2:Chuff=ChuffChoiceFast+"1.wav";BeginPlay(0,Chuff.c_str(),CV[110]);ChuffCycle=0;break;
                              case 3:Chuff=ChuffChoiceFast+"1.wav";BeginPlay(0,Chuff.c_str(),CV[110]);ChuffCycle=0;break;
                                          }
                      }
                      } // chuffs on

#endif   
}//chuff void

 
