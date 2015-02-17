
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <TFT.h>// Arduino LCD library

#include <Adafruit_BMP085.h>

#define SIZEOFRECORD 8

 // pin definition for the Uno
#define tft_cs 10
#define sd_cs 7
#define dc   9
#define rst  8
 // #define BUZ 4

#define PREF_PRESSURE_MIN 98000
#define PREF_PRESSURE_MAX 103500

//Nbre de pixel de l'Ã©cran
#define W_SCR 160
#define H_SCR 128

//Hauteur des caractÃ¨re en pixel
#define HEIGHT_CHAR 7

#define UPDATE_SCREEN_1_TIME 10000
#define UPDATE_SCREEN_2_TIME 10000
#define MEASURE_TIME 1000

//Ecran
TFT TFTscreen = TFT(tft_cs, dc, rst);

//Capteur de pression et de temperature
Adafruit_BMP085 sensor;

unsigned long lastMeasurementTime = 0;
unsigned long lastRefreshDisplay = 0;
byte heightsToDisplay[W_SCR];

long lastRefreshDisplay_2 = 0;

//Tendance de la pression sur 1 et 3 heures
int32_t pressure = 0;
int32_t min_pressure = PREF_PRESSURE_MIN;
int32_t max_pressure = PREF_PRESSURE_MAX;
byte norm_pressure_i = 0;

float temperature = 0;

//Liste des Ã©chelles en nombre de seconde par pixel horizontal
prog_uint8_t scales[] PROGMEM = {1, 3, 6, 12, 24, 48, 72, 96, 120}; //Correspond Ã  la visualisation de 3H, 6H, 12H, 24H, 48H, 72H}
volatile byte iscale = 0;

volatile unsigned long lastButtonAction = 0;
volatile boolean isScaleChanged = false;

//screens
long last_screen_change   = 0;
byte screen_number = 1;
boolean is_screen_changed = false;

//colors
byte blue[] = {72, 203, 240};

void setup () {
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  Serial.begin(9600);


  Serial.println("SETUP");


  if(!SD.begin(sd_cs)){
    Serial.println("Error SD card");
    return;
  }

  sensor.begin();
  // initialize the display
  TFTscreen.begin();

  measureValue();

  // clear the screen with a pretty color
  TFTscreen.background(0,0,0);

  Serial.println("SETUP screen_number = " + String(screen_number));

  if(screen_number == 1) {
    updateDataToDisplay();
    displayDisplayedPeriod();
  }
  else if(screen_number == 2) {

  }

  // define buttons
  //attachInterrupt(2, buttonActionPerformed, FALLING);
  attachInterrupt(3, changeDisplay, LOW);
}


void loop () {

  unsigned long gap = millis() - lastMeasurementTime;

  if(gap >= MEASURE_TIME){
    lastMeasurementTime += gap;
    measureValue();
  }


  if(screen_number == 1){



    // refresh display
    gap = millis() - lastRefreshDisplay;
    if(gap > UPDATE_SCREEN_1_TIME || is_screen_changed){
      Serial.println("LOOP screen_number 1");
      lastRefreshDisplay += gap;
      TFTscreen.background(0,0,0);
      displayDisplayedPeriod();
      updateDataToDisplay();
    }

    // change scale on display
    if(isScaleChanged || is_screen_changed){
      isScaleChanged = false;
      updateDataToDisplay();
      displayDisplayedPeriod();
    }
  }
  else if(screen_number == 2){

    gap = millis() - lastRefreshDisplay_2;
    if(gap > UPDATE_SCREEN_2_TIME || is_screen_changed){

      Serial.println("LOOP screen_number 2");

      lastRefreshDisplay_2 += gap;

      TFTscreen.background(0,0,0);
      display_screen_2();
    }
  }

  if(is_screen_changed) {
    is_screen_changed = false;
  }
}

void measureValue(){
  updatePressure();
  updateTemperature();
  //compute_display_PressureTrend();
  appendPressureInHistoric();
}

void display_screen_2(){


  TFTscreen.stroke(blue[0], blue[1], blue[2]);



    float xtemp = sensor.readTemperature();
    float xpressure = sensor.readPressure();
    displayValue(xpressure,0,20);
    displayValue(xtemp,0,80);
}


void displayValue(float value, int x, int y) {
  char valueStr[10];
  String(value).toCharArray(valueStr, 5);
  TFTscreen.setTextSize(4);
  TFTscreen.text(valueStr,x,y);
  TFTscreen.setTextSize(1);

}

void displayTitleText() {
  // clear the screen with a black background
  //screen.background(0, 0, 0);

  // write the static text to the screen
  // set the font color to white
  //screen.stroke(255, 0, 255);
  // set the font size
  //screen.setTextSize(2);
  // write the text to the top left corner of the screen
  //screen.text("Luftdruck :\n ", 0, 0);
  //screen.text("Temperature :\n ", 0, 60);

  // ste the font size very large for the loop
//  screen.setTextSize(4);

}


void compute_display_PressureTrend(){
  File historicFile = SD.open("historic.txt", FILE_WRITE);
  if(!historicFile){
    //Serial.println("1");
    return;
  }

  long fileSize = historicFile.size();
  long pos = fileSize - (long)10800* (long)SIZEOFRECORD;

  historicFile.seek(pos);

  char pressureArray[7];
  for(int j = 0; j < 6; j++){
    pressureArray[j] = historicFile.read();
    if(pressureArray[j] == ' '){
      pressureArray[j] = 0;
    }
  }
  pressureArray[6] = 0;
  long pressure3HValue = atol(pressureArray);

  pos = fileSize - (long)1800 * (long)SIZEOFRECORD;
  historicFile.seek(pos);

  pressureArray[7];
  for(int j = 0; j < 6; j++){
    pressureArray[j] = historicFile.read();
    if(pressureArray[j] == ' '){
      pressureArray[j] = 0;
    }
  }
  pressureArray[6] = 0;
  long pressure30MinValue = atol(pressureArray);
  historicFile.close();

  int oneHourTrend = (int)((pressure - pressure30MinValue)*(long)2);
  int threeHourTrend = (int)(pressure - pressure3HValue);

  String oneHTrendStr = String(oneHourTrend);
  oneHTrendStr += String("Pa/h");
  displayText(oneHTrendStr, 54, 50, 0, 255, 0, 0);

  String threeHTrendStr = String(threeHourTrend);
  threeHTrendStr += String("Pa/3h");
  displayText(threeHTrendStr, 54, 105, 0, 255, 0, 0);

}

void buttonActionPerformed(){
  if(millis() - lastButtonAction > 300){
    lastButtonAction = millis();
    iscale++;
    if(iscale == 8){
      iscale = 0;
    }
    isScaleChanged = true;
  }
}


void changeDisplay(){
  if(millis() - last_screen_change > 300){
    last_screen_change = millis();
    screen_number++;
    if(screen_number == 3){
      screen_number = 1;
    }
    is_screen_changed = true;
  }
}

void displayDisplayedPeriod(){
  byte period = (byte)pgm_read_word_near(scales + iscale);
  String periodTxt = String(period);
  periodTxt += "h";
  displayText(periodTxt, 24, 45, 120, 127, 127, 127);
}

void updatePressure(){
  //Mise Ã  jour de la pression
  long pressureTemp = sensor.readPressure();
  Serial.println(pressureTemp);
  long var;
  if(pressureTemp > pressure){
    var = pressureTemp - pressure;
  }
  else{
    var = pressure - pressureTemp;
  }

  //Cela permet de limiter du bruit observÃ© lors du fonctionnement
  if(var < (long)20){
    pressure = pressureTemp;
  }

//  displayPressure(pressure, 1, 75, 120, 255, 0, 0);
}

void updateTemperature(){
  //Mise Ã  jour de la pression
  float temperatureTemp = sensor.readTemperature();
  temperature = temperatureTemp;
//  displayTemperature(temperature, 5, 120, 255, 0, 0);
}

void displayText(String msg, int pxlg, int i, int j, byte r, byte g, byte b){
  TFTscreen.stroke(0,0,0);
  for(int k = i; k < i + pxlg; k++){
    TFTscreen.line(k, j, k, j + HEIGHT_CHAR);
  }

  TFTscreen.stroke(r,g,b);

  char txtmsg[11];
  msg.toCharArray(txtmsg, 11);

  TFTscreen.text(txtmsg, i, j);
}

void displayPressure(int32_t pressureval, byte res, byte i, byte j, byte r, byte g, byte b){
  TFTscreen.stroke(0,0,0);
  byte lgth;
  if(res == 0 || res == 1){
    lgth = 48;
  }
  else if(res == 2){
    lgth = 42;
  }
  else{
    lgth = 48;
    res = 0;
  }

  for(int k = i; k < i + lgth; k++){
    TFTscreen.line(k, j, k, j + HEIGHT_CHAR);
  }

  int32_t resAbs = 1;
  for(int i = 0; i < res; i++){
    resAbs = resAbs * 10;
  }

  pressureval = (int32_t)(pressureval + resAbs/2)/resAbs;
  String myString = String(pressureval);


  String unit = String("Pa");;
  if(res == 1){
    unit = String("dPa");
  }
  else if(res == 2){
    unit = String("hPa");
  }

  //myString += String(unit);
  myString += unit;
  char pressureStr[10];
  myString.toCharArray(pressureStr, 10);
  TFTscreen.stroke(r,g,b);
  TFTscreen.text(pressureStr, i, j);
}

void displayTemperature(float temperatureval, byte x, byte y, byte r, byte g, byte b){

  //clean scren where display temperature
  TFTscreen.stroke(0,0,0);
  TFTscreen.fill(0,0,0);
  TFTscreen.rect(x,y,30,7);

  String myString = String(temperatureval);
  char tempStr[7];
  myString.toCharArray(tempStr, 5);
  tempStr[4] = 67;
  TFTscreen.stroke(r,g,b);
  TFTscreen.text(tempStr, x, y);
}


void appendPressureInHistoric(){
  char pressureArray[7];
  String pressureStr = String(pressure);
  pressureStr.toCharArray(pressureArray, 7);
  if(pressure < 100000){
    pressureArray[5]=' ';
  }
  pressureArray[6]=0;

  File historicFile = SD.open("historic.txt", FILE_WRITE);
  if(!historicFile){
    return;
  }

  Serial.println(pressureArray);

  historicFile.println(pressureArray);
  historicFile.close();
}

void displayEmptyGraph(){
  /*{
    byte norm_pressure_i_temp = (byte)(((long)NORM_PRESSURE - (long)min_pressure) * (long)(H_SCR - 10 - 10) /((long)max_pressure - (long)min_pressure));
    norm_pressure_i_temp = H_SCR - 10 - 10 - norm_pressure_i_temp;
    if(norm_pressure_i_temp != norm_pressure_i){
      drawHDotLine(10 + norm_pressure_i, 0, 0, 0);
      norm_pressure_i = norm_pressure_i_temp;
    }
    drawHDotLine(10 + norm_pressure_i, 0, 0, 255);
  }*/

  for(int i = 0; i < 4; i++){
    drawHDotLine(10 + i * 36, 127, 127, 127);
    displayPressure(max_pressure - i * (max_pressure - min_pressure)/(long)3, 2, 0, 1 + i * 36, 127, 127, 127);
  }
}

void updateDataToDisplay(){

  displayEmptyGraph();
  //Nombre de seconde entre 2 mesures Ã  afficher
  byte scale = (byte)pgm_read_word_near(scales + iscale);

  File historicFile = SD.open("historic.txt", FILE_WRITE);
  if(!historicFile){
    return;
  }

  uint32_t fileSize = historicFile.size();
  long pos = (long)fileSize - (long)scale * (long)3600 * (long)SIZEOFRECORD;
  long increment = ((long)scale * (long)3600 + (long)W_SCR/(long)2) / (long)W_SCR;
  increment = increment * (long)SIZEOFRECORD;

  char pressureArray[7];
  int i = 0;
  int32_t temp_min_pressure = PREF_PRESSURE_MIN;
  int32_t temp_max_pressure = PREF_PRESSURE_MAX;

  while(i < W_SCR && pos < (long)fileSize){
    //Si il y a de l'historique
    if(pos >= 0){
      historicFile.seek(pos);

      for(int j = 0; j < 6; j++){
        pressureArray[j] = historicFile.read();
        if(pressureArray[j] == ' '){
          pressureArray[j] = 0;
        }
      }
      pressureArray[6] = 0;

      long pressureValue = atol(pressureArray);
      if(temp_min_pressure > pressureValue){
        temp_min_pressure = pressureValue;
      }
      if(temp_max_pressure < pressureValue){
        temp_max_pressure = pressureValue;
      }

      TFTscreen.stroke(0,0,0);
      TFTscreen.point(i, H_SCR-heightsToDisplay[i]);

      heightsToDisplay[i] = map(pressureValue, min_pressure, max_pressure, 10, H_SCR - 10);

      TFTscreen.stroke(0,255,0);
      TFTscreen.point(i, H_SCR-heightsToDisplay[i]);

      i++;
    }
    //Autrement
    else{
      if(heightsToDisplay[i] != 0){
        TFTscreen.stroke(0,0,0);
        TFTscreen.point(i, H_SCR - heightsToDisplay[i]);
      }

      heightsToDisplay[i++] = 0;
    }
    pos += increment;
  }
  historicFile.close();

  //Les nouvelles valeurs maximum seront prises en compte lors du prochain rafraichissement.
  max_pressure = temp_max_pressure;
  min_pressure = temp_min_pressure;
}

void drawHDotLine(byte j, byte r, byte g, byte b){
  TFTscreen.stroke(r, g, b);
  for(int i = 0; i < W_SCR; i++){
    if(i%3 == 0){
      TFTscreen.point(i, j);
    }
  }
}
