

int led_ready = 3;
int ventil = 12;
int shot = 8;
int shotswitch = 4;
int ventilswitch = 5;
bool bserial = false;

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
    delay(100);
    openVentil();
    ventilready = digitalRead(ventilswitch);
  }
  
  int shotready = digitalRead(shotswitch);

  while(1 == shotready) {
    mylog("open ventil");
    digitalWrite(ventil,HIGH);
    delay(50);
    digitalWrite(ventil,LOW);
    delay(80);
    digitalWrite(ventil,HIGH);
    delay(50);
    digitalWrite(ventil,LOW);
    
    delay(160);
    mylog("shot");
    
    digitalWrite(shot,HIGH);
    delay(500);
    digitalWrite(shot,LOW);
      
    shotready = digitalRead(shotswitch);
  }
  mylog("wait");
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

    
    int ventilOpen1 = Serial.parseInt(); 
    
    int ventilDelay = Serial.parseInt(); 
    
    int ventilOpen2 = Serial.parseInt(); 

    int delayShot = Serial.parseInt(); 

    if (Serial.read() == '\n') {
      // print the three numbers in one string as hexadecimal:
      Serial.print("Ventil open 1 : " + ventilOpen1 + " # ");
      Serial.print("Ventil open 2 : " + ventilOpen2 + " # ");      
      Serial.print("delay " + ventilDelay + " # ");
      Serial.println("delayShot " + ventilShot + "");
    }
  }

}