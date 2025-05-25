// The Spectacular Clock
// By: Ryan Jardina - rjardina gmail.com
// Last update 05/24/2025

#include "RTClib.h"                           // Understand RTC (Real Time Clock)
#include <Adafruit_NeoPixel.h>                // Addressable LEDs (control LEDs)
#include <EEPROM.h>                           // Read & write EEPROM (memory)
RTC_DS1307 rtc;                               // Tell RTC library what IC used
#define Pi 3.1415                             // Defines Pi, Duh?!
byte J;
long CAT;                                     // Current Added Time
byte Direction = EEPROM.read(0);              // Read memory & set Direction
byte Brightness = EEPROM.read(1);             //      "      "     Brightness
byte Switch = EEPROM.read(2);                 //      "      "     Switch
byte Shift = EEPROM.read(3);                  //      "      "     Shift
byte Wheel = EEPROM.read(4);                  //      "      "     Wheel
byte Demo = EEPROM.read(5);                   //      "      "     Demo

Adafruit_NeoPixel pixels(3, 6, NEO_RGB + NEO_KHZ800); // number of pixels, pin output, RGB order, bitstream

void motor(byte one, byte two, byte three) {
  byte Digit[] = {one, two, three};
  float Angle[] = {0, .6666, 1.3333};
  if (!Direction) { Angle[1] = 1.3333; Angle[2] = .6666; } // Change Direction
  long RGB[3];
  for (byte Q = 0; Q <= 2; Q++) {
    long X = Digit[Q] + Shift;
    for (byte Phase = 0; Phase <= 2; Phase++) {    // Run loop till Phase > 2
      if (Wheel != 1) {
        RGB[Phase] = 125 + (2 * 400 / Pi) * asin(cos(2 * (Pi / 60) * X + Angle[Phase] * Pi)); }
      if (!Demo && Q == 1) {
        if (Direction) {RGB[Phase] = 22 + (2 * 70 / Pi) * atan(tan(2 * (Pi / 30) * -X));}
         else {RGB[Phase] = 22 + (2 * 70 / Pi) * atan(tan(2 * (Pi / 30) * X));} }
      RGB[Phase] = min(255, max(RGB[Phase], 0)); }      // Make negative number zero & End for
  if (!Wheel && Demo || !Demo && Q == 2) { // Turn 3 phase into RYB output
    long RYB[3];
    X = min(RGB[1], RGB[2]);
    RYB[0] = RGB[0] + RGB[1] - X;
    RYB[1] = RGB[1] + (2 * X);
    RYB[2] = 2 * (RGB[2] - X);
    float I = 255 / float(RYB[0] + RYB[1] + RYB[2]);
    for (X = 0; X <= 2; X++) { RGB[X] = I * RYB[X]; } }
  pixels.setBrightness(Brightness);           // Set brightness to LED
  pixels.setPixelColor(Q, RGB[0], RGB[1], RGB[2]); } // Update color
  pixels.show(); }                            // Update LEDs & End motor

void setup() {
  pixels.begin();                             //   "   neopixel
  rtc.begin();                                //   "   RTC
  for (byte V = 7; V <= 9; V++) { pinMode(V, INPUT_PULLUP); } // Set button X as input w/pullup
  !Demo ? demo() : animation(); }             // if NOT Demo run demo/reset else run animation

void loop() {
  DateTime now = rtc.now();                   // For time updating
  if (now.second() != CAT) {                  // Update clock every second
    if (!Switch) {motor(now.hour() * 5, now.minute(), now.second());}
            else {motor(now.second(), now.minute(), now.hour() * 5);}
    CAT = now.second(); }                     // Add second delay to next update & End if
  if (buttons()) {
    delay(100);
    switch (buttons()) {
      case 1: rtc.adjust(now.unixtime() + 3600); break;                     // Add 1 hour
      case 2: rtc.adjust(now.unixtime() + 60); break;                       // Add 1 min
      case 3: Brightness < 254 ? Brightness += 85 : Brightness = 85; break; // Add 1/3 Brightness
      case 4: rtc.adjust(now.unixtime() + 1);  break;                       // Add 1 second
      case 5: Switch ? Switch = 0 : Switch = 1; break;                      // Switches hour & second LEDs
      case 6: Shift ? Shift = 0 : Shift = now.second(); break;              // Move second's position to top or undo
      case 7: Demo = 0; EEPROM.write(5, Demo); wait(); demo(); break; }     // Put into init
    memory();
    wait(); } }

void animation() {
  top:                                        // Goto label for startup animation
  if (millis() > CAT) {                       //
    J <= 59 ? J++ : J = 0;                    // Start animation & X now Q
    motor(J, J, J);
    CAT = millis() + 166; }                   // Adds .166 millsecond till next color
  if (Demo && J <= 59) {goto top;} }          // Used for startup

void demo() {
  Direction = 0;                              // Default value for Direction
  Brightness = 255;                           //     "       "     Brightness
  Switch = 0;                                 //     "       "     Switch
  Shift = 0;                                  //     "       "     Shift
  Wheel = 0;                                  //     "       "     Wheel
  while (!Demo) {                             // While Demo is not 1
    animation();                              //
    if (buttons()) {                          // if Input NOT 0
      delay(100);                             // For button debouncing
      switch (buttons()) {
        case 1: Wheel = 2; Demo = 1; break;   // Input is 1 Choose RGB color wheel & turn off demo mode
        case 2: J = 0; Direction ? Direction = 0 : Direction = 1; break;   // Change direction
        case 4: Wheel = 0; Demo = 1; break; } //   "      4    "    Y       "          "         "
     wait(); } }                              // Wait for input to stop
  rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0));  // Reset time
  memory(); }                                 // Writes all changes to memory

byte buttons() {
  byte Pressed;
  for (byte V = 0; V <= 2; V++) {bitWrite(Pressed, V, !digitalRead(V + 7));}
  return Pressed; }

void wait() {
  while (buttons()) {                         // Run loop till Input 0
    pixels.clear();                           // Turn LEDs off
    pixels.show();                            // Update LEDs
    CAT = 100; } }                            // Clock update ASAP & End while & wait

void memory() {
  EEPROM.write(0, Direction);                 // Writes Direction to memory
  EEPROM.write(1, Brightness);                //    "   Brightness    "
  EEPROM.write(2, Switch);                    //    "   Switch        "
  EEPROM.write(3, Shift);                     //    "   Shift         "
  EEPROM.write(4, Wheel);                     //    "   Color Wheel   "
  EEPROM.write(5, Demo); }                    //    "   Demo          "