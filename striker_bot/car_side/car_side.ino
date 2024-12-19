// ESP-NOW  receiver + motor

#include <esp_now.h>
#include <WiFi.h>

//motor
#define A1 21
#define A2 23
#define B1 18
#define B2 19
#define SPA 13
#define SPB 14

const unsigned int delaytime = 30;


typedef struct struct_message {
  short RState;
  short LState;
  bool SpeedState;
} struct_message;
struct_message myData;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  commands();
}

void setup() {
  Serial.begin(115200);
  //motor
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);
  pinMode(SPA, OUTPUT);
  pinMode(SPB, OUTPUT);

  delay(200);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}


void loop() {
}

void commands() {
  if (myData.RState>0) {
    MRF();
  }
  else if (myData.RState<0){
    MRB();
  }
  else{
    MRS();
  }
  
  if (myData.LState>0) {
    MLF();
  }
  else if (myData.LState<0){
    MLB();
  }
  else{
    MLS();
  }

  if (myData.SpeedState){
    analogWrite(SPA,255);
    analogWrite(SPB,255);
  }
  else{
    analogWrite(SPA,126);
    analogWrite(SPB,126);
  }
}

void MRF() {
  digitalWrite(A1, HIGH);
  digitalWrite(A2, LOW);
  delay(delaytime);
}

void MRB() {
  digitalWrite(A1, LOW);
  digitalWrite(A2, HIGH);
  delay(delaytime);
}

void MRS() {
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  delay(delaytime);
}

void MLF() {
  digitalWrite(B1, HIGH);
  digitalWrite(B2, LOW);
  delay(delaytime);
}

void MLB() {
  digitalWrite(B1, LOW);
  digitalWrite(B2, HIGH);
  delay(delaytime);
}

void MLS() {
  digitalWrite(B1, LOW);
  digitalWrite(B2, LOW);
  delay(delaytime);
}
