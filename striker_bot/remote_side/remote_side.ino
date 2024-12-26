// remote side dig
#include <esp_now.h>
#include <WiFi.h>

const int deadzone = 30;

const int RAxisPin = 33;
const int LAxisPin = 25;

const int LSPButton = 35;
const int RSPButton = 34;

#define LED_PIN 19

int Rvalue = 125;
int Lvalue = 125;

// bool RFB = false;
// bool RBB = false;

// bool LFB = false;
// bool LBB = false;

int Rsp = 1;
int Lsp = 1;



// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };  // car

const int send_rate = 30;
int sendvar = 0;

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int RState;  // -2, -1 , 0 ,1 , 2
  int LState;
} struct_message;

// Create a struct_message called Data & peer
struct_message Data;
struct_message PrevData;
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
  status == ESP_NOW_SEND_SUCCESS ? digitalWrite(LED_PIN, HIGH) : digitalWrite(LED_PIN, LOW);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  pinMode(RAxisPin, INPUT);
  pinMode(LAxisPin, INPUT);

  // pinMode(RBButton, INPUT);
  // pinMode(LBButton, INPUT);

  pinMode(RSPButton, INPUT);
  pinMode(LSPButton, INPUT);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  delay(200);
  digitalWrite(LED_PIN, LOW);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}


bool button_state(int a) {
  int s = 0;
  for (int i = 0; i < 4; i++) {
    s += digitalRead(a);
    delay(3);
  }
  return (s > 2) ? true : false;
}

// bool DataDiff(){
//   // to check if Data and dataprev is different
//   if(Data.LState == PrevData.LState && Data.RState == PrevData.RState){
//     return false;
//   }
//   PrevData.LState = Data.LState;
//   PrevData.RState = Data.RState;
//   return true;
// }

int stick_value(int sp) {
  int abs_sp = abs(sp);
  int output = 0;

  if (abs_sp <= deadzone) {  // Deadzone
    output = 0;
  } else if (abs_sp <= 200) {                // analog range
    output = map(abs_sp, 31, 200, 70, 200);  // Scale to 1-127
  } else {                                   // High speed range
    output = 255;
  }
  return output;
}

void loop() {
  Rvalue = analogRead(RAxisPin);
  Lvalue = analogRead(LAxisPin);

  Rsp = button_state(RSPButton);
  Lsp = button_state(LSPButton);

  Serial.print(Rvalue);
  Serial.print(" ");
  Serial.print(Lvalue);
  Serial.print(" ");

  Rvalue = stick_value(Rvalue);
  Lvalue = stick_value(Lvalue);

  if (Rsp && Lsp) {
    Rvalue = 0;
    Lvalue = 0;
  } else if (Rsp) {
    Rvalue = 255;
    Lvalue = 255;
  } else if (Lsp) {
    Rvalue = -255;
    Lvalue = -255;
  }

  Data.RState = Rvalue;
  Data.LState = Lvalue;
  
  Serial.print(Data.RState);
  Serial.print(" ");
  Serial.println(Data.LState);

  // === Send message via ESP-NOW ===
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Data, sizeof(Data));

  if (result == ESP_OK) {
    Serial.println("Sent");
  } else {
    Serial.println("Error");
  }

  delay(20);
}
