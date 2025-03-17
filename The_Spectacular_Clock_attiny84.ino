// The Spectacular Clock
// By: Ryan Jardina - rjardina gmail.com
// Last update 10/06/2019
// Using Attinycore to program Attiny84

#include <TinyRTClib.h>                       // Understand RTC (Real Time Clock)
#include <TinyWireM.h>                        // I2C Communication (Allow talk to RTC)                   
#include <Adafruit_NeoPixel.h>                // Addressable LEDs (control LEDs)
#include <EEPROM.h>                           // Read & write EEPROM (memory)
RTC_DS1307 rtc;                               // Tell RTC library what IC used
#define Pi 3.1415                             // Defines Pi, Duh?!
long X; byte Q; float Angle[3]; byte Phase; byte Digit[3]; // Variables
byte Input; float I; long RYB[3]; byte J; long RGB[3];     // More variables
byte Brightness; byte Demo; byte Direction; byte Switch; byte Wheel; byte Shift; // Memory Variables
byte Pins[] = {0, 1, 2, 3, 5, 7, 9, 10, 11};  // Buttons & unused pins #
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(3, 8, NEO_GRB + NEO_KHZ800);
                                              // number of pixels, pin output, RGB order, bitstream
void setup() {                                // setup function
  TinyWireM.begin();                          // Start Wire communicate
  pixels.begin();                             //   "   neopixel
  rtc.begin();                                //   "   RTC
  for (X = 0; X <= 9; ++X) {pinMode(Pins[X], INPUT_PULLUP);} // Set pins as input w/pullup
  Direction = EEPROM.read(0);                 // Read memory & set Direction
  Brightness = EEPROM.read(1);                //      "      "     Brightness
  Switch = EEPROM.read(2);                    //      "      "     Switch
  Shift = EEPROM.read(3);                     //      "      "     Shift
  Wheel = EEPROM.read(4);                     //      "      "     Wheel
  Demo = EEPROM.read(5);                      //      "      "     Demo
  if (Demo != 1) {demo();}                    // if Demo NOT 1 run demo/reset
    else {animation();}                       // Else run animation
}                                             // End setup function
void loop() {                                 // Main function run over & over
  DateTime now = rtc.now();                   // For time updating
  if (millis() > X) {                         // Update clock every second
    Digit[0] = now.hour() * 5;                // Digit 0 equals hour times five
    Digit[1] = now.minute();                  //   "   1    "   minutes
    Digit[2] = now.second();                  //   "   2    "   seconds
    if (Switch == 1) {X = Digit[0]; Digit[0] = Digit[2]; Digit[2] = X;} // Switch Hr & Sec Leds
    motor();                                  // Run sub-function
    X = millis() + 1000; }                    // Add second delay to next update & End if
  buttons();                                  // Run sub-function
  if (Input != 0) {                           // if Input NOT 0
    delay(100); buttons();                    // Delay for button debounce & combos
    if (Input == 1) {rtc.adjust(now.unixtime() + 3600);}  // Add 1 hour
    if (Input == 2) {rtc.adjust(now.unixtime() + 60);}    // Add 1 min
    if (Input == 4) {rtc.adjust(now.unixtime() + 3);}     // Add 3 seconds
    if (Input == 3) {Brightness = Brightness + 85;        // Add 1/4 Brightness
      if (Brightness > 255) {Brightness = 85;} }          // if Brightness above 100 set to 1/4
    if (Input == 5) {if (Switch == 0) {Switch = 1;}       // Switches hour & second LEDs
      else {Switch = 0;} }                    // Switch already 1 make 0
    if (Input == 6) {                         // Move face of clock
      if (Shift == 0) {Shift = now.second();} // Move second's position to top
       else {Shift = 0;} }                    // Red back at top
    if (Input == 7) {Demo = 0; EEPROM.write(5, Demo); wait(); demo();} // Put into init
      memory();                               // Run sub-function
      wait(); }                               // Run sub-function & End; if & loop
}                                             // End main loop
void motor() {                                // motor sub-function
  if (Direction == 0) {Angle[1] = (1.3333); Angle[2] = (.6666);} // Change Direction
    else {Angle[1] = (.6666); Angle[2] = (1.3333);}       // Change back
  for (Q = 0; Q <= 2; ++Q) {                  //
    X = Digit[Q] + Shift;                     //
    for (Phase = 0; Phase <= 2; ++Phase) {    // Run loop till Phase > 2
      if (Wheel != 1) {                       // 
        RGB[Phase] = 63.5 + (2 * 192.2 / Pi) * asin(cos(2 * (Pi / 60) * X + Angle[Phase] * Pi)); }
      if (Demo != 1 && Q == 1) {              //
        if (Direction == 1) {RGB[Phase] = 22 + (2 * 70 / Pi) * atan(tan(2 * (Pi / 30) * -X));}
         else {RGB[Phase] = 22 + (2 * 70 / Pi) * atan(tan(2 * (Pi / 30) * X));} }
      RGB[Phase] = max(RGB[Phase], 0); }      // Make negative number zero & End for
  if (Wheel == 0 && Demo == 1 || Demo != 1 && Q == 2) { // Turn 3 phase into RYB output
    X = min(RGB[1], RGB[2]);                  //
    RYB[0] = RGB[0] + RGB[1] - X;             //
    RYB[1] = RGB[1] + (2 * X);                //
    RYB[2] = 2 * (RGB[2] - X);                //
    I = RYB[0] + RYB[1] + RYB[2];             // Add all three colors together
    I = 255 / I;                              // Take all three colors & gives % between 0-255
    for (X = 0; X <= 2; ++X) {                //
      RGB[X] = I * RYB[X]; } }                //
  pixels.setBrightness(Brightness);           // Set brightness to LED
  pixels.setPixelColor(Q, RGB[0], RGB[1], RGB[2]); } // Update color
  pixels.show();                              // Update LEDs & End motor
}                                             // End motor sub-function
void animation() {                            //
  top:                                        // Goto label for startup animation
  if (millis() > X) {                         //
    if (J <= 59) {J++;}                       // Start animation & X now Q
      else {J = 0;}                           //
    Digit[0] = J;                             // Set color  hour   LED
    Digit[1] = J;                             //   "   "   minute   "
    Digit[2] = J;                             //   "   "   second   "
    motor();                                  // Run motor sub-function
    X = millis() + 166; }                     // Adds .166 millsecond for till next color
    if (Demo == 1 && J <= 59) {goto top;}     // Used for startup
}                                             // animation sub-function
void demo() {                                 //
  Direction = 0;                              // Default value for Direction
  Brightness = 255;                           //     "       "     Brightness
  Switch = 0;                                 //     "       "     Switch
  Shift = 0;                                  //     "       "     Shift
  Wheel = 0;                                  //     "       "     Wheel
  while (Demo != 1) {                         // While Demo is not 1
      animation();                            //
      buttons();                              // Runs sub-function & read Input
    if (Input != 0) {                         // if Input NOT 0
     delay(100); buttons();                   // For button debouncing
     if (Input == 2) { J = 0;                 // if Input is 2
       if (Direction == 0) {Direction = 1;}   // Change direction/order
        else {Direction = 0;} }               // Change back
     if (Input == 1) {Wheel = 2; Demo = 1;}   // if Input is 1 Choose RGB color wheel & turn off demo mode
     if (Input == 4) {Wheel = 0; Demo = 1;}   //      "      4    "    Y       "          "         "
     wait(); } }                              // Wait for input to stop
  rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0));  // Reset time
  memory();                                   // Writes all changes to memory
}                                             // End demo sub-function
void buttons() {                              // buttons sub-function
  bitWrite(Input, 0, !digitalRead(0));        // Button 1 pressed add 1 to INPUT
  bitWrite(Input, 1, !digitalRead(1));        //    "   2      "      2    "
  bitWrite(Input, 2, !digitalRead(2));        //    "   4      "      4    "     & End buttons
}                                             // End sub-function
void wait() {                                 // wait sub-function
  while (Input != 0) {                        // Run loop till Input 0
    buttons();                                // Read Input again.
    pixels.clear();                           // Turn LEDs off
    pixels.show();                            // Update LEDs
    X = 0; }                                  // Clock update ASAP & End while & wait
}                                             // End sub-function
void memory() {                               // memory sub-function
  EEPROM.write(0, Direction);                 // Writes Direction to memory
  EEPROM.write(1, Brightness);                //    "   Brightness    "
  EEPROM.write(2, Switch);                    //    "   Switch        "
  EEPROM.write(3, Shift);                     //    "   Shift         "
  EEPROM.write(4, Wheel);                     //    "   Color Wheel   "
  EEPROM.write(5, Demo);                      //    "   Demo          "
}                                             // Ends memory sub-function
