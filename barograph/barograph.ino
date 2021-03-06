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

#define MEASURE_INTERVAL 1000

TFT screen = TFT(cs, dc, rst);

#define ONE_HOUR_AGO 1
#define ONE_DAY_AGO 24

int screen_width = 160;
int screen_height = 128;

byte text_color[] = {170,170,170};
byte more_pressure_color[] = {70,230,60};
byte less_pressure_color[] = {250,20,20};

unsigned long last_measure_time = 0;
unsigned long last_read_time = 0;

String last_pressure = "";
String last_temperature = "";
float last_pressure_diff = 0;
float last_day_pressure_diff = 0;

String temperature = "";
String pressure = "";

void setup() {
  Serial.begin(9600);
  Serial.println("START");
  sensor.begin();
  screen.begin();
  // clear the screen with a black background
  screen.background(0, 0, 0);
  if(!SD.begin(sd_cs)) {
    Serial.println("ERROR when open SD");
  }
  displayTitleText();
}

void loop() {
  unsigned long gap = millis() - last_measure_time;
  // capture pressure
  if(gap >= MEASURE_INTERVAL){
    last_measure_time += gap;
    display_pressure_on_screen();
    display_temperature_on_screen();
    write_data_to_file();
    display_pressure_trend();
    display_day_pressure_trend();
  }
}

void read_pressure_from_file() {

  byte num_of_values = 9;
  byte counter = 0;

  File data_file = SD.open("DATA.CSV", FILE_WRITE);
  if(!data_file) {
    Serial.println("ERROR file not found read_pressure_from_file");
  }
  long file_size = data_file.size();

  byte values[num_of_values];
  byte b;

  long sekunden = 60;
  long size_of_record = 13;
  long pos = file_size - (sekunden * size_of_record);

  data_file.seek(pos);

  char file_pressure[7];
  char file_temperature[5];

  for(int i = 0; i <= size_of_record - 3; i++){
    b = data_file.read();
    if(i <= 5) {
      file_pressure[i] = char(b);
    }

    if(i > 6) {
      file_temperature[i - 7] = char(b);
      Serial.print(b);
      Serial.print(',');
    }

  }
  file_pressure[6] = 0;
  file_temperature[4] = 0;

  float f_pressure;
  sscanf(file_pressure, "%f", &f_pressure);

  float f_temperature;
  sscanf(file_temperature, "%f", &f_temperature);

  data_file.close();
}

float read_f_pressure_from_file(int time) {
  byte values[9];
  byte b;
  char file_pressure[6];

  File data_file = SD.open("DATA.CSV", FILE_WRITE);
  if(!data_file) {
    Serial.println("ERROR file not found read_pressure_from_file");
    return 0;
  }
  long file_size = data_file.size();

  int sekunden = 60 * 60 * time * (MEASURE_INTERVAL / 1000);
  long size_of_record = 13;
  long pos = file_size - (sekunden * size_of_record);
  data_file.seek(pos);

  for(int i = 0; i <= size_of_record - 3; i++){
    b = data_file.read();
    if(i <= 5) {
      file_pressure[i] = char(b);
    }
  }

  float f_pressure;
  sscanf(file_pressure, "%f", &f_pressure);

  data_file.close();
  return f_pressure;
}


void display_pressure_trend() {
  String s = "";
  float pressure_diff =  pressure.toFloat() - read_f_pressure_from_file(ONE_HOUR_AGO);

  if(pressure_diff != last_pressure_diff) {
    screen.stroke(0, 0, 0);
    screen.fill(0, 0, 0);
    screen.rect(10,60,screen_width,20);
    screen.noFill();
  }

  if(pressure_diff >= 0){
    s = "+";
    screen.stroke(more_pressure_color[0], more_pressure_color[1], more_pressure_color[2]);
  }
  else {
    screen.stroke(less_pressure_color[0], less_pressure_color[1], less_pressure_color[2]);
  }

  s = s + String(pressure_diff);
  if(pressure_diff >= 10) {
    s.remove(5);
  }
  else {
    s.remove(4);
  }
  s = s + "hPa/h";

  displayStringValue(s,10,60,2);
  last_pressure_diff = pressure_diff;
}

void display_day_pressure_trend() {
  String s = "";
  float pressure_diff =  pressure.toFloat() - read_f_pressure_from_file(ONE_DAY_AGO);;

  if(pressure_diff != last_day_pressure_diff) {
    screen.stroke(0, 0, 0);
    screen.fill(0, 0, 0);
    screen.rect(10,82,screen_width,20);
    screen.noFill();
  }

  if(pressure_diff >= 0){
    s = "+";
    screen.stroke(more_pressure_color[0], more_pressure_color[1], more_pressure_color[2]);
  }
  else {
    screen.stroke(less_pressure_color[0], less_pressure_color[1], less_pressure_color[2]);
  }

  s = s + String(pressure_diff);
  if(pressure_diff >= 10) {
    s.remove(5);
  }
  else {
    s.remove(4);
  }
  s = s + "hPa/d";

  displayStringValue(s,10,82,2);
  last_day_pressure_diff = pressure_diff;
}


void write_data_to_file() {
  String value = last_pressure + "," + last_temperature;
  File data_file = SD.open("DATA.CSV", FILE_WRITE);

  if(!data_file) {
    Serial.println("ERROR file not found write_data_to_file");
  }
  data_file.println(value);
  data_file.close();
}

void display_temperature_on_screen() {
  temperature = readTemp();

  byte pos_x = screen_width - 50;
  byte pos_y = screen_height - 20;

  if (last_temperature != temperature) {
    screen.stroke(0, 0, 0);
    screen.fill(0, 0, 0);
    screen.rect(pos_x,pos_y,screen_width,20);
    screen.noFill();
  }

  screen.stroke(text_color[0], text_color[1], text_color[2]);
  displayStringValue(temperature,pos_x,pos_y,2);
  last_temperature = temperature;
}

void display_pressure_on_screen() {

  pressure = readPressure();
  if (last_pressure != pressure){
    screen.stroke(0,0,0);
    screen.fill(0, 0, 0);
    screen.rect(0,20,screen_width,40);
    screen.noFill();
  }

  screen.stroke(text_color[0], text_color[1], text_color[2]);
  displayStringValue(pressure,0,20,4);
  last_pressure = pressure;
}

String readTemp() {

  String s;
  float f_temperature = sensor.readTemperature();

  // add a space if temp below 10 to have always the same lenght
  if (f_temperature < 10) {
    s = " ";
  }

  char result[10];
  sprintf(result, "%.1f", f_temperature);

  return s + result;
}

String readPressure() {

  String s = "";
  int i_pressure = sensor.readPressure();

  if (i_pressure < 100000) {
    s = " ";
  }

  float f_pressure = float(i_pressure / 10);

  char result[6];
  sprintf(result, "%.1f", f_pressure / 10);

  return s + String(result);
}

void displayStringValue(String value, int x, int y, byte txs) {
  char valueStr[value.length() + 1];
  value.toCharArray(valueStr, value.length() + 1);
  screen.setTextSize(txs);
  screen.text(valueStr,x,y);
}

void displayTitleText() {
  // clear the screen with a black background
  screen.background(0, 0, 0);

  // write the static text to the screen
  // set the font color to white
  screen.stroke(text_color[0], text_color[1], text_color[2]);
  // set the font size
  screen.setTextSize(2);
  // write the text to the top left corner of the screen
  screen.text("Luftdruck:\n ", 0, 0);
  screen.text("Temp:\n ", 0, screen_height - 20);

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
