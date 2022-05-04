#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "FS.h"   // spiffs


#include "Chuff.h"

// NOTES:
/*
 * REMEMBER to turn off serial monitor BEFORE trying to upload SPIFFs files! 
 * 
 * 
 * NodeMCUPinD[0] = D0;  //the pins that relate to my D0-10 are set in ESP nodemcu etc directives 
  NodeMCUPinD[1] = D1;
  NodeMCUPinD[2] = D2;
  NodeMCUPinD[3] = D3;
  NodeMCUPinD[4] = D4;
  NodeMCUPinD[5] = D5;
  NodeMCUPinD[6] = D6;
  NodeMCUPinD[7] = D7;
  NodeMCUPinD[8] = D8; // 
  NodeMCUPinD[9] = D9; //RX
  NodeMCUPinD[10] = D10;// TX 
 * 
 * 
 */




const int SteamOutputPin = 14;  //D5   on node mcu Used here for Steam output

const int WhistleDemandPin = 12;  //D6 on node mcu Used here for whistle demand
const int DemandD2 =  4  ;     //D2
const int DemandD4 =  2  ;     //D4
const int DemandD3 =  0  ;     //D3
const int WheelSensor_Pin = 5; //D1 on nodemcu
// #define UseWheelSensor  // uncomment to use this sensor!
bool Last_sensor_State;


long x;
long TimeNow;
long CPM;
long Period;
long SteamOnStarted;
long SteamPulseDuration;
bool Audio_Setup_Problem;

bool SoundPlaying1;
bool SoundPlaying0;

uint8_t CV[200];   // CVs for speed etc!
extern int Last_Speed_Demand;
  
void setup(){
 CV[1]=3;
 CV[100]=100;
 CV[2]=4;  // start "voltage"
 CV[110]=70; // sound volume
  
  Serial.begin(115200);
  delay(100);
  Audio_Setup_Problem = false;
  
  delay(100);
  x=0;
  pinMode(WhistleDemandPin, INPUT_PULLUP);
  pinMode(DemandD2, INPUT_PULLUP);
  pinMode(DemandD3, INPUT_PULLUP);
  pinMode(DemandD4, INPUT_PULLUP);
  
  pinMode(SteamOutputPin, OUTPUT);
  CPM=60; // 121= 5mph start ChuffsPerMinute  1216 cpm or  304 rpm = 50 mph for 1.4m dia wheels
  Period=500;
  SteamPulseDuration=50;
  SteamOnStarted=millis();
  pinMode(WheelSensor_Pin, INPUT_PULLUP);
  Last_sensor_State=digitalRead(WheelSensor_Pin);
  Last_Speed_Demand= 0;
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




extern long Chuff_wav_period;
extern int Last_Speed_Demand;

void loop(){ 
bool WheelSensor;
 
 TimeNow=millis();
 AudioLoop(millis());

 #ifdef UseWheelSensor
 WheelSensor=digitalRead(WheelSensor_Pin);
  if (WheelSensor){
    Last_sensor_State=1;
    }  // set at 1 if input is high (rest ready for next pulse) 
 
  if (!WheelSensor&& Last_sensor_State )  // Sensor is LOW, but last_state  was high, .. 
     {
      Last_sensor_State=0;  // set this low so it will not immediately retrigger this loop!! 
      Chuff("/BBCH");
      //  NOt Working NEWChuff("/BBCH","/Fast",35);
      SteamOnStarted=millis();
      digitalWrite(SteamOutputPin,HIGH);
     }

 #else  (// !UseWheelSensor
  if (TimeToChuff(Period)){
    Chuff("/Fenchurch");
    // Not working just for demonstration! NEWChuff("/Fenchurch","/Fench",35); //~35 mph rough trigger speed .."chuff(" has formulae with number of wavcyclesperrpm  chuff selects sound samples (slow and Fast) and per wav switch period interval t sect slow or fast"/BBCH" is my best sounding set.. or try "/ivor_" or "/Fenchurch"  
     SteamOnStarted=millis();
     digitalWrite(SteamOutputPin,HIGH);
     }
 #endif  //if def UseWheelSensor
 
 if ((SteamOnStarted+SteamPulseDuration)<=TimeNow){digitalWrite(SteamOutputPin,LOW);}
 //Increment the "CPM" speeds for test...
 if (x>=5000 ){x=0;CPM=CPM+48;
              Last_Speed_Demand = 10;
              if (CPM>=1){ Period=60000/CPM;}else {Period=500;}      //half sec min if CPM =0 
              Serial.print("Speed(mph):");Serial.print(CPM/24);      //increase CPM
              Serial.print("  Period:");Serial.print(Period);Serial.print("ms");
              Serial.println();
           } 
   if(CPM>=600){ CPM=60;Period=1000;BeginPlay(0,"/brakes.wav",128);
              }
   // FORCE a single speed for tests
  //Period=400;            
  Chuff_wav_period = Period;


 
 // play Sound effect on Channel 1 by pressing button..
    if (!digitalRead(WhistleDemandPin) && !SoundPlaying1){ 
      Serial.println("SOUND EFFECT DEMAND 1");
       BeginPlay(1,"/F1.wav",100);//F1 Ivor F6 Bell 
       }
if (!digitalRead(DemandD4) && !SoundPlaying1){ 
      Serial.println("SOUND EFFECT DEMAND 4");
       BeginPlay(1,"/F6.wav",100);//F1 Ivor F6 Bell 
       }
       
  if (!digitalRead(DemandD3) && !SoundPlaying1){  
      Serial.println("SOUND EFFECT DEMAND 2");
       BeginPlay(1,"/F3.wav",100);//F1 Ivor F6 Bell 
       }
  if (!digitalRead(DemandD2) && !SoundPlaying1){ 
      Serial.println("SOUND EFFECT DEMAND 3");
       BeginPlay(1,"/F2.wav",100);//F1 Ivor F6 Bell 
       }

   
  x=x+1;  //loop count
 delay(1);
}
                          
