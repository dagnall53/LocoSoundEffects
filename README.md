# LocoSoundEffects
ESP8266 based sound effects test for Steam Locos, based on EarlePhilHower's "ESP8266 audio" 

This test uses the Adafruit I2SDAC, plus a single button on pin d6 to ground to initiate a whistle.
When not playing the whistle (or other effects), the code sequences through a four phase "chuff" generator, playing "Chuff samples.
I hope to later add a wav mixer so that the chuff can continue whilst other effects are played.
A steam generator can be controlled (synchronous to the chuff) on pin D5.

The code gradually increases speed, until a max is reched, when it applies the brakes and starts again. 
