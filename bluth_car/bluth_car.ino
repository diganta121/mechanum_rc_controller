#include "BluetoothSerial.h"


BluetoothSerial SerialBT;


const int C1 = 18;
const int C2 = 21;
const int A1 = 19;
const int A2 = 22;
const int sp_pin = 13;

String SBT;

int speed = 255;


void setup() {
  Serial.begin(115200);
  SerialBT.begin("Cheese");
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(C1, OUTPUT);
  pinMode(C2, OUTPUT);
}


void loop() {
  if (SerialBT.available()) {

    delay(5);
    SBT = SerialBT.readString();
    Serial.println(SBT);
    analogWrite(sp_pin,speed);

    if (SBT == "RF") {
      digitalWrite(A1, HIGH);
      digitalWrite(A2, LOW);
    } else if (SBT == "RB") {
      digitalWrite(A1, LOW);
      digitalWrite(A2, HIGH);

    } else if (SBT == "RST") {
      digitalWrite(A1, LOW);
      digitalWrite(A2, LOW);
    }


    if (SBT == "LF") {
      digitalWrite(C1, HIGH);
      digitalWrite(C2, LOW);
    } else if (SBT == "LB") {
      digitalWrite(C1, LOW);
      digitalWrite(C2, HIGH);

    } else if (SBT == "LST") {
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
}