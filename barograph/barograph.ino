#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <TFT.h>  // Arduino LCD library
#include <SPI.h>
#include <SD.h>

Adafruit_BMP085 sensor;

#define cs 12
#define sd_cs 11
#define dc 10
#define rst 9

#define MEASURE_INTERVAL 10000

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
byte light_on = 0;

int display_screen = 1;

long last_light_on = 0;

long last_button_pressed_time = 0;
long light_last_button_pressed_time = 0;
byte last_displayed_screen = 2;

void setup() {
  Serial.begin(9600);
  Serial.println("START");
  if(!sensor.begin()) {
    Serial.println("ERROR sensor");
  }
  screen.begin();
  // clear the screen with a black background
  screen.background(0, 0, 0);
  if(!SD.begin(sd_cs)) {
    Serial.println("ERROR when open SD");
  }

  pinMode(4,OUTPUT);
  digitalWrite(4,LOW);

  displayTitleText();
  //draw_pressure_graph_lines();
  //draw_pressure_graph();

  attachInterrupt(7, change_display, RISING);
  attachInterrupt(6, turn_on_display, RISING);
}

void turn_on_display() {
  long button_pressed = millis() - light_last_button_pressed_time;

  if(button_pressed > 100) {
    light_on = 1;
  }

  light_last_button_pressed_time = millis();
}

void loop(){

  if (light_on == 0){

  }
  else if( light_on == 1){
    digitalWrite(4,HIGH);
    light_on = 2;
    last_light_on = millis();
  }
  else {
    if (millis() - last_light_on > 10000) {
      digitalWrite(4,LOW);
      light_on = 0;
    }
  }

  if(display_screen == 1 && last_displayed_screen == 2) {
    displayTitleText();
    last_displayed_screen = 1;
  }
  else if (display_screen == 2 && last_displayed_screen == 1){
    draw_pressure_graph_lines();
    last_displayed_screen = 2;
  }
  else {
    // nothing changed
  }

  unsigned long gap = millis() - last_measure_time;

  if(gap >= MEASURE_INTERVAL){
    last_measure_time += gap;
    pressure = readPressure();
    temperature = readTemp();
    write_data_to_file();
  }

  if(last_displayed_screen == 1) {

    if(gap >= MEASURE_INTERVAL){
      display_pressure_on_screen();
      display_temperature_on_screen();
      display_pressure_trend();
      display_day_pressure_trend();
    }

  }
  else if(last_displayed_screen == 2) {
    if(gap >= MEASURE_INTERVAL){
      draw_pressure_graph();
    }
  }



}

void change_display() {

  long button_pressed = millis() - last_button_pressed_time;

  if(button_pressed > 100) {

      if(display_screen == 1) {
        display_screen = 2;
      }
      else if(display_screen == 2) {
        display_screen = 1;
      }

      Serial.println("display = " + String(display_screen));

  }

  last_button_pressed_time = millis();

}

void draw_pressure_graph_lines() {

  screen.background(0,0,0);
  screen.stroke(text_color[0], text_color[1], text_color[2]);


      for (int i = 0; i < screen_width; i ++) {
        if(i % 4 == 0) {
          screen.point(i, 100);
        }
      }
      displayStringValue("980", 0, 97, 1);


      for (int i = 0; i < screen_width; i ++) {
        if(i % 4 == 0) {
          screen.point(i, 80);
        }
      }
      displayStringValue("990", 0, 77, 1);


      for (int i = 0; i < screen_width; i ++) {
        if(i % 4 == 0) {
          screen.point(i, 60);
        }
      }
      displayStringValue("1000", 0, 57, 1);

      for (int i = 0; i < screen_width; i ++) {
        if(i % 4 == 0) {
          screen.point(i, 40);
        }
      }
      displayStringValue("1010", 0, 37, 1);


      for (int i = 0; i < screen_width; i ++) {
        if(i % 4 == 0) {
          screen.point(i, 20);
        }
      }
      displayStringValue("1020", 0, 17, 1);

}

void draw_pressure_graph() {

  screen.stroke(more_pressure_color[0], more_pressure_color[1], more_pressure_color[2]);


  File data_file = SD.open("DATA.CSV", FILE_WRITE);
  if(!data_file) {
    Serial.println("ERROR file not found read_pressure_from_file");
  }
  long file_size = data_file.size();

  byte b;
  char x_pressure[6];
  long hours = 48;
  long size_of_record = 13;

  Serial.println("file size = " + String(file_size));


  long pos = file_size - (60 * 60 * hours * 1000 / MEASURE_INTERVAL * size_of_record);

  Serial.println("file pos = " + String(pos));

  if (pos <= 0 ) {
    pos = 0;
  }

  data_file.seek(pos);

  int x = 0;

  while(data_file.available() > size_of_record) {
    for (int i = 0; i < size_of_record; i++) {

      b = data_file.read();

      if(i <= 5) {
        x_pressure[i] = char(b);
        Serial.print(char(b));
      }


    }

    float f_pos = 0;

    pos = data_file.position() + (3 * hours * size_of_record);
    data_file.seek(pos);

    float f_pressure;
    sscanf(x_pressure, "%f", &f_pressure);

    Serial.println("");
    Serial.println(f_pressure);

    int y = (screen_height / 2) + ((1000 - f_pressure) * 2);
    screen.point(x, y);

    Serial.println("");

    x = x + 1;

  }

  Serial.println("number of records = " + String(x));

}

void xloop() {
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

  int sekunden = 60 * 60 * time * 1000 / MEASURE_INTERVAL;
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
  String value = pressure + "," + temperature;
  File data_file = SD.open("DATA.CSV", FILE_WRITE);

  if(!data_file) {
    Serial.println("ERROR file not found write_data_to_file");
  }
  data_file.println(value);
  Serial.println("write values : " + String(value));
  data_file.close();
}

void display_temperature_on_screen() {

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
