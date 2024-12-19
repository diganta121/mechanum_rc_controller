// remote side dig
#include <esp_now.h>
#include <WiFi.h>
#include <ezButton.h>

#define RFButton 14
#define RBButton 27

#define LFButton 26
#define LBButton 25

#define SPButton 33
// & 3.3V    & GND

#define LED_PIN 19
bool RFB = false;
bool RBB = false;

bool LFB = false;
bool LBB = false;

bool SPB = false;

int Rm = 0;
int Lm = 0;

int bValue = 0;

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = { 0x34, 0x85, 0x18, 0x26, 0xD7, 0x34 };  //34:85:18:26:D7:34   ESP32-C3 Device1
//uint8_t broadcastAddress[] = {0x34,0x85,0x18,0x26,0xC6,0x88};  //34:85:18:26:C6:88   ESP32-C3 Device2

//uint8_t broadcastAddress[] = {0xC8,0xC9,0xA3,0xC9,0x14,0xEC};  //C8:C9:A3:C9:14:EC   ESP32-Wroom


// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  short RState;// -1 , 0 ,1
  short LState;
  bool SpeedState;//0 , 1
} struct_message;

// Create a struct_message called myData & peer
struct_message myData;
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\n Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
  status == ESP_NOW_SEND_SUCCESS ? digitalWrite(LED_PIN, HIGH) : digitalWrite(LED_PIN, LOW);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  pinMode(RFButton, INPUT);
  pinMode(LFButton, INPUT);
  pinMode(SPButton, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
delay(100);
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

void loop() {
  RFB = read_button(RFButton);
  LFB = read_button(LFButton);
  
  RBB = read_button(RBButton);
  LBB = read_button(LBButton);

  if(!(RFB && RBB)){
    if(RFB){
      myData.RState = 1;
    }
    else if(RBB) {
      myData.RState = -1;
    }
    else{
      myData.RState = 0;
    }
  }
  else{
    myData.RState = 0;
  }

  if(!(LFB && LBB)){
    if(LFB){
      myData.LState = 1;
    }
    else if(LBB){
      myData.LState = -1;
    }
    else{
      myData.LState = 0;
  }
  else{
    myData.LState = 0;
  }
  //myData.RState = Rm;
  //myData.LState = Lm;
  //myData.RState = analogRead(RFButton);
  //myData.LState = analogRead(LFButton);


  // Read the button value
  bValue = read_button(SPButton);
  delay(2);
  myData.SpeedState = bValue;
  //myData.SpeedState = button.getState();


  Serial.print(myData.RState);
  Serial.print("  ");
  Serial.print(myData.LState);
  Serial.print("  ");
  Serial.println(myData.SpeedState);
  //Serial.println("");


  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent");
  } else {
    Serial.println("Error");
  }
  delay(50);
}


int read_button(int a){
  int s = 0;
  for(int i=0;i<4;i++){
    s += digitalRead(a);
    delay(2);
  }
  return (s>2)? true : false ;
}
