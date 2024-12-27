// ESP-NOW  receiver + motor

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>


//motor
#define A1 21
#define A2 23
#define B1 18
#define B2 19
#define SPA 13
#define SPB 14

const unsigned int delaytime = 25;

uint8_t newMACAddress[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };


typedef struct struct_message {
  int RState;
  int LState;
} struct_message;
struct_message myData;

void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  const uint8_t *mac = recv_info->src_addr;
  memcpy(&myData, incomingData, sizeof(myData));

  commands();
}


void readMacAddress() {
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  } else {
    Serial.println("Failed to read MAC address");
  }
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
  Serial.println("started");

  // Change ESP32 Mac Address
  esp_err_t err = esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);
  if (err == ESP_OK) {
    Serial.println("Success changing Mac Address");
  }
  delay(100);
  readMacAddress();
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
  //right side
  int R = myData.RState;
  int L = myData.LState;
  Serial.print(L);
  Serial.print(" ");
  Serial.println(R);



  if (R > 10) {
    MRF();
  } 
  else if (R < -10) {
    MRB();
  } 
  else {
    MRS();
  }
  
  if(R<0){
  analogWrite(-SPA, R);
  }
  else{
  analogWrite(SPA, R);
  }


  //left side
  if (L > 10 ) {
    MLF();
  } 
  else if (L < -10) {
    MLB();
  }
   else {
    MLS();
  }

  if(L<0){
  analogWrite(-SPB, L);
  }
  else{
  analogWrite(SPB, L);
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