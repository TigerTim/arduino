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

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  Serial.println("START");

  sensor.begin();

  // Put this line at the beginning of every sketch that uses the GLCD:
  screen.begin();

  // clear the screen with a black background
  screen.background(0, 0, 0);

  if(!SD.begin(sd_cs)) {
      Serial.println("ERROR when open SD");
  }

  Serial.println("____2___");

  //write_data_to_file("test1,test2");


  //read_data_from_file();

  drawLine(0,0,50,50);
  //displayTitleText();

}

void loop() {
  // put your main code here, to run repeatedly:

    //Serial.print(bmp.readTemperature());
    //Serial.println(" *C");

    //Serial.print("Pressure = ");
    //Serial.print(bmp.readPressure());
    //Serial.println(" Pa");

    //temp_pressure_screen();
}

void read_pressure_from_file() {

  byte num_of_values = 5;
  byte counter = 0;

  File data_file = SD.open("data.csv", FILE_READ);
  int values[num_of_values];
  byte b;

  while (data_file.available() && counter <= num_of_values) {
    b = data_file.read();

    if(b == 44) {

    }
    counter += 1;
  }

  data_file.close();
}


void read_data_from_file() {

  File data_file = SD.open("data.csv", FILE_READ);

  while (data_file.available()) {
       Serial.print(data_file.read());
  }

  data_file.close();
}

void write_data_to_file(String value) {

  File data_file = SD.open("data.csv", FILE_WRITE);

  if(!data_file) {
    Serial.println("ERROR file not found");
  }

  byte xx = data_file.println(value);

  Serial.println(xx);

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

void drawLine(byte x1, byte y1,byte x2, byte y2) {
  // clear the screen with a black background
  screen.background(0, 0, 0);

  // write the static text to the screen
  // set the font color to white
  screen.stroke(255, 255, 255);

  // ste the font size very large for the loop
  screen.line(x1,y1,x2,y2);

}
