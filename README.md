# LED_Matrix
My personal project with a 47x60 LED matrix.

My LED matrix is wired differently than the OctoWS2811 library expects.
Because of this, I had to modify the library. While I was at it, I made
other optimizations to the audio library. All the changed files that
need to be updated are:

C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3\Arduino.h (need to verify this, if changed)
C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio\analyze_fft1024.cpp
C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio\analyze_fft1024.h
C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\OctoWS2811\OctoWS2811.cpp

Arduino.h defines the preprocessor constant. The other files include changes around
this preprocessor directive. The .ino files look it as well. Edit: Actually its not there.
Arduino looks at the header files first before looking at any .ino files. I can't define
the preprocessor in my ino files. I need to make a new header file that defines the
constant.
