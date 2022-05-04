# LocoSoundEffects
ESP8266 based sound effects test for Steam Locos, based on EarlePhilHower's "ESP8266 audio" 
Video of it working https://youtu.be/Vd0HbV_MXVI

This test originally used the Adafruit I2SDAC, plus a single button on pin d6 to ground to initiate a whistle.

This new version (2022) includes mixer to show two channel use.
also has 4 possible sound effects. 

The Chuffs are on Channel 0
Soundeffects are on Channel 1


A steam generator can be controlled (synchronous to the chuff) on pin D5.

If the period is allowed to change (see Line 131 on ino) The code will gradually increases speed, until a max is reached, when it applies the brakes and starts again. 

The sound samples are contained in a spiffs file, and can be created using audacity or similar, or downloaded from free sample sets from DCC sound suppliers

2022  the wav files are now all the same bitrate. So make sure you upload these!! 

I have included the NEWChuff  function that changes from multiple phase sounds to a single sound at a switch speed. - 
BUT NOTE THIS IS NOT WORKING! it is included for example purposes only! 

The code is not "TIDY".. its just to show how to use the audio and the mixer. 


