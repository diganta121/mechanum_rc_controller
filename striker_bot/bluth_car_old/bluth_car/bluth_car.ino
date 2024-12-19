#include "BluetoothSerial.h"


BluetoothSerial SerialBT;


const int C1 = 18;//right motor
const int C2 = 19;
const int A1 = 21;//left motor
const int A2 = 23;
const int spA = 13;
const int spC = 14;

String SBT;
 
int speed = 255;


void setup() {
  Serial.begin(115200);
  SerialBT.begin("Cheese");
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(C1, OUTPUT);
  pinMode(C2, OUTPUT);
  pinMode(spA,OUTPUT);
  pinMode(spC,OUTPUT);
}


void loop() {
  if (SerialBT.available()) {
    SBT = SerialBT.readString();
    Serial.println(SBT);

    analogWrite(spA,speed);
    analogWrite(spC,speed);

    if (SBT == "RF") {
      digitalWrite(A1, HIGH);
      digitalWrite(A2, LOW);
      
    } else if (SBT == "RB") {
      digitalWrite(A1, LOW);
      digitalWrite(A2, HIGH);

    }

    if (SBT == "LF") {
      digitalWrite(C1, HIGH);
      digitalWrite(C2, LOW);
    } else if (SBT == "LB") {
      digitalWrite(C1, LOW);
      digitalWrite(C2, HIGH);

    } 


     if (SBT == "RST") {
      digitalWrite(A1, LOW);
      digitalWrite(A2, LOW);
    }
    if (SBT == "LST") {
      digitalWrite(C1, LOW);
      digitalWrite(C2, LOW);
    }

    if (SBT == "ST"){
      stop();
    }

    if (SBT == "SPF"){
      speed = 255;
    }else if (SBT == "SPS"){
      speed = 120;
    }
  }
}


void stop() {
  digitalWrite(C1, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(C2, LOW);
  digitalWrite(A2, LOW);

  digitalWrite(spA,LOW);
  digitalWrite(spC,LOW);

}