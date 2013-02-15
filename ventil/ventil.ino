

int led_ready = 3;
int ventil = 12;
int shot = 8;
int shotswitch = 4;
int ventilswitch = 5;
bool bserial = true;

int ventilOpen1 = 100;
int ventilDelay = 100;
int ventilOpen2 = 100;
int delayShot = 100; 

void setup() {                

  pinMode(led_ready, OUTPUT);
  pinMode(shot, OUTPUT);
  pinMode(ventil, OUTPUT);  
  pinMode(shotswitch, INPUT);
  pinMode(ventilswitch, INPUT);
  
  digitalWrite(led_ready, LOW);
  digitalWrite(ventil, LOW); 
  digitalWrite(shot, LOW);
  
  Serial.begin(9600);
  mylog("START") ;
}
 

void loop() {
 
  readSerialData();

  int ventilready = digitalRead(ventilswitch);
  while(1 == ventilready) {
    mylog("ventil spuelen");
    delay(100);
    openVentil();
    ventilready = digitalRead(ventilswitch);
  }
  
  int shotready = digitalRead(shotswitch);

  while(1 == shotready) {
    digitalWrite(ventil,HIGH);
    mylog("open ventil 1");
    delay(ventilOpen1);
    digitalWrite(ventil,LOW);
    delay(ventilDelay);
    if(ventilOpen2 > 0) {
      digitalWrite(ventil,HIGH);
      mylog("open ventil 2");
      delay(ventilOpen2);
      digitalWrite(ventil,LOW);
    }
    
    delay(delayShot);
    
    
    digitalWrite(shot,HIGH);
    mylog("shot");
    delay(500);
    digitalWrite(shot,LOW);
      
    shotready = digitalRead(shotswitch);
  }
  delay(1000);
  mylog("" + String(ventilOpen1) + "," + String(ventilDelay) + "," + String(ventilOpen2) + "," + String(delayShot));

}

void mylog(String value) {
  if(bserial == true) {
    Serial.println(value);
  }
}

void openVentil() {
  
  int ventilready = digitalRead(ventilswitch);
  while(1 == ventilready) {
    delay(100);
    mylog("ventil spuelen");
    digitalWrite(ventil,HIGH);
    ventilready = digitalRead(ventilswitch);
  }
  delay(100);
  digitalWrite(ventil,LOW);
}

void readSerialData() {
  
  while (Serial.available() > 0) {
    mylog("readSerialData");
    
    ventilOpen1 = Serial.parseInt(); 
    
    ventilDelay = Serial.parseInt(); 
    
    ventilOpen2 = Serial.parseInt(); 

    delayShot = Serial.parseInt(); 

    //if (Serial.read() == '\n') {
      // print the three numbers in one string as hexadecimal:
      Serial.println("Ventil open 1 : " + String(ventilOpen1) + " # ");
      Serial.println("Ventil open 2 : " + String(ventilOpen2) + " # ");      
      Serial.println("delay " + String(ventilDelay) + " # ");
      Serial.println("delayShot " + String(delayShot) + "");
    //}
  }

}
