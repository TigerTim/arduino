#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <TFT.h>  // Arduino LCD library
#include <SPI.h>
#include <SD.h>

Adafruit_BMP085 sensor;

#define cs 10
#define sd_cs 7
#define dc   9
#define rst  8

TFT screen = TFT(cs, dc, rst);

int dW = 160;
int dH = 128;

File data_file;

void setup() {
  // put your setup code here, to run once:

  // Serial.begin(9600);

  sensor.begin();

  // Put this line at the beginning of every sketch that uses the GLCD:
  screen.begin();

  // clear the screen with a black background
  screen.background(0, 0, 0);

  SD.begin();

  //drawLine(0,0,50,50);
  displayTitleText();

}

void loop() {
  // put your main code here, to run repeatedly:

    //Serial.print(bmp.readTemperature());
    //Serial.println(" *C");

    //Serial.print("Pressure = ");
    //Serial.print(bmp.readPressure());
    //Serial.println(" Pa");

    temp_pressure_screen();
}

void read_data_to_file(String value) {

  data_file = SD.open("pressure_temp.csv", FILE_WRITE);

  data_file.println(value);

  data_file.close();
}

void write_data_to_file(String value) {

  data_file = SD.open("pressure_temp.csv", FILE_WRITE);

  data_file.println(value);

  data_file.close();
}

void temp_pressure_screen() {

  screen.stroke(0, 0, 255);

  float temp = readTemp();
  float pressure = readPressure();
  displayValue(pressure,0,20);
  displayValue(temp,0,80);

  String s = String(pressure) + "," + String(temp);
  write_data_to_file(s);


  delay(5000);

  screen.stroke(0, 0, 0);
  displayValue(temp,0,80);
  displayValue(pressure,0,20);

}

float readTemp() {
  return sensor.readTemperature();
}

float readPressure() {
  return sensor.readPressure();
}

void displayValue(float value, int x, int y) {
  char valueStr[10];
  String(value).toCharArray(valueStr, 5);
  screen.text(valueStr,x,y);
}

void displayTitleText() {
  // clear the screen with a black background
  screen.background(0, 0, 0);

  // write the static text to the screen
  // set the font color to white
  screen.stroke(255, 0, 255);
  // set the font size
  screen.setTextSize(2);
  // write the text to the top left corner of the screen
  screen.text("Luftdruck :\n ", 0, 0);
  screen.text("Temperature :\n ", 0, 60);

  // ste the font size very large for the loop
  screen.setTextSize(4);

}
