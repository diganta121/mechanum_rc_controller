#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Motor pins
#define A1 21
#define A2 23
#define B1 32
#define B2 33
#define SPA 13
#define SPB 14

const unsigned int commandInterval = 25;  // Equivalent to delaytime
unsigned long lastCommandTime = 0;

unsigned long lastCommandTime2 = 0; // for lost contact
unsigned long currMilis =0;


uint8_t newMACAddress[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };

typedef struct struct_message {
  int RState;
  int LState;
} struct_message;

struct_message myData = { 0, 0 };

void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  if (len == sizeof(myData)) {
    memcpy(&myData, incomingData, sizeof(myData));
  } else {
    Serial.println("Received data size mismatch!");
  }
}

void setup() {
  Serial.begin(115200);

  // Motor pin setup
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);
  pinMode(SPA, OUTPUT);
  pinMode(SPB, OUTPUT);

  WiFi.mode(WIFI_STA);
  Serial.println("Started");

  // Change ESP32 Mac Address
  if (esp_wifi_set_mac(WIFI_IF_STA, newMACAddress) != ESP_OK) {
    Serial.println("Failed to set MAC address");
  } else {
    Serial.println("MAC address changed successfully");
  }

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  currMilis = millis();
  if (currMilis - lastCommandTime > commandInterval) {
    commands();
    lastCommandTime = millis();
  }
 else if (currMilis - lastCommandTime2 > 2000) {
    myData.RState = 0;
    myData.LState = 0;
    lastCommandTime2 = millis();
  }
}

void commands() {
  int R = constrain(myData.RState, -255, 255);
  int L = constrain(myData.LState, -255, 255);

  // Right side motor control
  if (R > 10) {
    MRF();
    analogWrite(SPA, R);
  } else if (R < -10) {
    MRB();
    analogWrite(SPA, -R);
  } else {
    MRS();
    analogWrite(SPA, 0);
  }

  // Left side motor control
  if (L > 10) {
    MLF();
    analogWrite(SPB, L);
  } else if (L < -10) {
    MLB();
    analogWrite(SPB, -L);
  } else {
    MLS();
    analogWrite(SPB, 0);
  }

  Serial.print("L: ");
  Serial.print(L);
  Serial.print(" R: ");
  Serial.println(R);
  lastCommandTime2 = millis();
}

void MRF() {
  digitalWrite(A1, HIGH);
  digitalWrite(A2, LOW);
}

void MRB() {
  digitalWrite(A1, LOW);
  digitalWrite(A2, HIGH);
}

void MRS() {
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
}

void MLF() {
  digitalWrite(B1, HIGH);
  digitalWrite(B2, LOW);
}

void MLB() {
  digitalWrite(B1, LOW);
  digitalWrite(B2, HIGH);
}

void MLS() {
  digitalWrite(B1, LOW);
  digitalWrite(B2, LOW);
}
