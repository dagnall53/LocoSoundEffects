#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "FS.h"   // spiffs
#include "Chuff.h"
// NOTES:
/*
 * REMEMBER to turn off serial monitor BEFORE trying to upload SPIFFs files! 
 * 
 * 
 * 
 * 
 * 
 */





const int WhistleDemandPin = 12;  //D6 on node mcu Used here for whistle demand
const int SteamOutputPin = 14;  //D5   on node mcu Used here for Steam output

const int WheelSensor_Pin = 5; //D1 on nodemcu
// #define UseWheelSensor  // to use this sensor!
bool Last_sensor_State;


long x;
long TimeNow;
long CPM;
long Period;
long SteamOnStarted;
long SteamPulseDuration;
bool Audio_Setup_Problem;


  
void setup()
{
  Serial.begin(115200);
  delay(100);
  Audio_Setup_Problem = false;
  
  delay(100);
  x=0;
  pinMode(WhistleDemandPin, INPUT_PULLUP);
  pinMode(SteamOutputPin, OUTPUT);
  CPM=60; // 121= 5mph start ChuffsPerMinute  1216 cpm or  304 rpm = 50 mph for 1.4m dia wheels
  Period=500;
  SteamPulseDuration=50;
  SteamOnStarted=millis();
  pinMode(WheelSensor_Pin, INPUT_PULLUP);
  Last_sensor_State=digitalRead(WheelSensor_Pin);

  //***********CHECK SPIFFS directory
  if (!SPIFFS.begin ()) {
    Serial.println ("An Error has occurred while mounting SPIFFS");
    return;
  }

  Dir dir = SPIFFS.openDir ("");
  while (dir.next ()) {
    Serial.print (dir.fileName ());
    Serial.print (" size (");
    Serial.println (dir.fileSize ());
    Serial.println(")");
  }
  //*************************
  SetUPAudio();
}


void loop(){ 
bool WheelSensor;
 
 TimeNow=millis();
 AudioLoop(millis());

 #ifdef UseWheelSensor
 WheelSensor=digitalRead(WheelSensor_Pin);
  if (WheelSensor){
    Last_sensor_State=1;
    }  // set at 1 if input is high (rest ready for next pulse) 
 
  if (!WheelSensor&& Last_sensor_State   // Sensor is LOW, but last_state  was high, .. 
     &&!SoundEffectPlaying()){
      Last_sensor_State=0;  // set this low so it will not immediately retrigger this loop!! 
      Chuff();SteamOnStarted=millis();digitalWrite(SteamOutputPin,HIGH);
     }

 #else  (// !UseWheelSensor
 //if (TimeToChuff(Period)&&!SoundEffectPlaying()){ old 
  if (TimeToChuff(Period)){
     Chuff();
     SteamOnStarted=millis();
     digitalWrite(SteamOutputPin,HIGH);
     }
 #endif  //if def UseWheelSensor
 
 if ((SteamOnStarted+SteamPulseDuration)<=TimeNow){digitalWrite(SteamOutputPin,LOW);}
 //set speeds...
 if (x>=5000 ){x=0;CPM=CPM+48;
              if (CPM>=1){ Period=60000/CPM;}else {Period=500;} //half sec if CPM =0 
              Serial.print("Speed(mph):");Serial.print(CPM/24);//increase CPM
              Serial.print("  Period:");Serial.print(Period);Serial.print("ms");
              Serial.println();
           } 
   if(CPM>=600){ CPM=60;Period=1000;BeginPlay(0,"/brakes.wav",128);
              }
   // FORCE a single speed for test
  Period=238;            



 
 // play Sound effect on Channel 1 by pressing button..
    if (!digitalRead(WhistleDemandPin) && !SoundEffectPlaying()){ 
      //Serial.print("SOUND EFFECT DEMAND");
       BeginPlay(1,"/F1.wav",100);//F1 Ivor F6 Bell 
       }
  

   
  x=x+1;  //loop count
 delay(1);
}
                          
