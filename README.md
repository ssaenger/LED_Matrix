# LED_Matrix
My personal project with a 47x60 LED matrix.

My LED matrix is wired differently than the OctoWS2811 library expects.
Because of this, I had to modify the library. While I was at it, I made
other optimizations to the audio library. All the changed files that
need to be updated are:

C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3\Arduino.h
C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio\analyze_fft1024.cpp
C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio\analyze_fft1024.h
C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\OctoWS2811\OctoWS2811.cpp

Arduino.h defines the preprocessor constant. The other files include changes around
this preprocessor directive. The .ino files look it as well. 
