#include <TFT.h>  // Arduino LCD library
#include <SPI.h>

// pin definition for the Uno
#define cs 12
#define sd_cs 11
#define dc   10
#define rst  9
// pin definition for the Leonardo
// #define cs   7
// #define dc   0
// #define rst  1

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);

// char array to print to the screen

void setup() {

  // Put this line at the beginning of every sketch that uses the GLCD:
  TFTscreen.begin();

  pinMode(4,OUTPUT);
  digitalWrite(4,LOW);

  // clear the screen with a black background
  TFTscreen.background(100, 100, 100);

  // write the static text to the screen
  // set the font color to white
  TFTscreen.stroke(255, 255, 255);
  // set the font size
  TFTscreen.setTextSize(2);
  // write the text to the top left corner of the screen
  TFTscreen.text("Hello World", 0, 0);
  // ste the font size very large for the loop
  TFTscreen.setTextSize(5);
}

void loop() {
  digitalWrite(4,HIGH);
  delay(5000);
  digitalWrite(4,LOW);
  delay(5000);
}
