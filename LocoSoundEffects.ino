#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "Chuff.h"
const int WhistleDemandPin = 12;  //D6 on node mcu Used here for whistle demand
const int SteamOutputPin = 14;  //D5   on node mcu Used here for Steam output

const int WheelSensor_Pin = 5; //D1 on nodemcu
#define UseWheelSensor  // to use this sensor!
bool Last_sensor_State;


long x;
long TimeNow;
long CPM;
long Period;
long SteamOnStarted;
long SteamPulseDuration;



  
void setup()
{
  Serial.begin(115200);
  delay(100);
  SetUpChuff();
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
}


void loop(){ 
bool WheelSensor;
WheelSensor=digitalRead(WheelSensor_Pin);
 TimeNow=millis();
 AudioLoop(1);
if (WheelSensor){Last_sensor_State=1;}  // set at 1 if input is high (rest ready for next pulse) 
 #ifdef UseWheelSensor
 if (!WheelSensor&& Last_sensor_State   // Sensor is LOW, but last_state  was high, .. 
     &&!SoundEffectPlaying()){
      Last_sensor_State=0;  // set this low so it will not immediately retrigger this loop!! 
      Chuff();SteamOnStarted=millis();digitalWrite(SteamOutputPin,HIGH);
     }

 #else
 if (TimeToChuff(Period)&&!SoundEffectPlaying()){Chuff();SteamOnStarted=millis();digitalWrite(SteamOutputPin,HIGH);}
 #endif
 
 if ((SteamOnStarted+SteamPulseDuration)<=TimeNow){digitalWrite(SteamOutputPin,LOW);}
 //set speeds...
 if (x>=5000 ){x=0;CPM=CPM+48;
              if (CPM>=1){ Period=60000/CPM;}else {Period=500;} //half sec if CPM =0 
              Serial.print("Speed(mph):");Serial.print(CPM/24);//increase CPM
              Serial.print("  Period:");Serial.print(Period);Serial.print("ms");
              Serial.println();
           } 
   if(CPM>=600){ CPM=60;Period=1000;BeginPlay("/brakes.wav");
              }



 
 // play whistle by pressing button..
    if (!digitalRead(WhistleDemandPin) && !SoundEffectPlaying()){ BeginPlay("/Class 4 med with reverb 1.wav");}
  

   
  x=x+1;  //loop count
 delay(1);
}
                          
