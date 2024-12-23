// remote side dig
#include <esp_now.h>
#include <WiFi.h>

const int RFButton = 33;
const int RBButton = 14;
const int LFButton = 25;
const int LBButton = 27;

const int LSPButton = 35;
const int RSPButton = 34;

#define LED_PIN 19

bool RFB = false;
bool RBB = false;

bool LFB = false;
bool LBB = false;

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

  pinMode(RFButton, INPUT);
  pinMode(LFButton, INPUT);
  pinMode(RBButton, INPUT);
  pinMode(LBButton, INPUT);

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

bool DataDiff(){
  // to check if Data and dataprev is different
  if(Data.LState == PrevData.LState && Data.RState == PrevData.RState){
    return false;
  }
  PrevData.LState = Data.LState;
  PrevData.RState = Data.RState;
  return true;
}

void loop() {
  RFB = button_state(RFButton);
  LFB = button_state(LFButton);

  RBB = button_state(RBButton);
  LBB = button_state(LBButton);

  Rsp = button_state(RSPButton)? 2 : 1;
  Lsp = button_state(LSPButton)? 2 : 1;

  if (!(RFB && RBB)) {
    if (RFB) {
      Data.RState = Rsp;
      //Serial.println(" rf ");
    } else if (RBB) {
      Data.RState = -Rsp;
      //Serial.println(" rb ");
    } else {
      Data.RState = 0;
    }
  } else {
    Data.RState = 0;
    //Serial.println(" rboth ");
  }

  if (!(LFB && LBB)) {
    if (LFB) {
      Data.LState = Lsp;
      //Serial.println(" lf ");
    } else if (LBB) {
      Data.LState = -Lsp;
      // Serial.println(" lb ");
    } else {
      Data.LState = 0;
    }
  } 
  else {
    Data.LState = 0;
    // Serial.println(" lboth ");
  }

  // Read the button value
  //bValue = button_state(SPButton);
  //delay(2);

  Serial.print(Data.RState);
  Serial.print(" ");
  Serial.println(Data.LState);


  // === Send message via ESP-NOW only if data changed ===
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Data, sizeof(Data));

  if (result == ESP_OK) {
    Serial.println("Sent");
  } else {
    Serial.println("Error");
  }

  delay(20); 
}
