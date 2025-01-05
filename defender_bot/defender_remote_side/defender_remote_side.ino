// remote side dig
#include <esp_now.h>
#include <WiFi.h>

const int deadzone = 120;

const int accAxisPin = 35;
const int sideAxisPin = 34;
const int throtAxisPin = 32;

const int LSPButton = 33;

#define LED_PIN 19

int acc = 0;
int side = 0;
int throt = 0;

int Lsp = 1;

int lthrot = 0;
int lacc = 0;
int lside = 0;

int mthrot = 0;
int macc = 0;
int mside = 0;

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] =  { 0x36, 0x69, 0x2B, 0xFC, 0x4D, 0x3F };
const int send_rate = 30;
int sendvar = 0;

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int acc;  // -2, -1 , 0 ,1 , 2
  int side;
  int throt;
} struct_message;

// Create a struct_message called Data & peer
struct_message Data;
struct_message PrevData;
esp_now_peer_info_t peerInfo;

int defaultacc = 0;
// Variables to store default joystick positions (calibration offsets)
int defaultside = 0;
int defaultThrot = 0;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
  status == ESP_NOW_SEND_SUCCESS ? digitalWrite(LED_PIN, HIGH) : errBlink();
}

void errBlink(){
  for (int i = 0; i < 3; i++) {  // Blink LED for failure
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void calibrateMinJoystick(){
  digitalWrite(LED_PIN, HIGH);
  delay(2000);
  lthrot = analogRead(throtAxisPin);
  lacc = analogRead(throtAxisPin);
  lside = analogRead(throtAxisPin);
  errBlink();
  delay(2000);
  digitalWrite(LED_PIN, HIGH);
  mthrot = analogRead(throtAxisPin);
  macc = analogRead(throtAxisPin);
  mside = analogRead(throtAxisPin);
  digitalWrite(LED_PIN, LOW);
}

// Function to calibrate joystick
void calibrateJoystick() {
    int totalAcc = 0, totalSide = 0,totalThrot = 0;
    int samples = 7; // Number of samples to average for calibration

    Serial.println("Calibrating joystick...");
    for (int i = 0; i < samples; i++) {
        totalAcc += analogReadSmooth(accAxisPin);
        totalSide += analogReadSmooth(sideAxisPin);
        totalThrot += analogReadSmooth(throtAxisPin);

        delay(20); // Small delay between samples
    }

    // Calculate average default position
    defaultacc = climit(map(totalAcc / samples,lacc,macc,255,-255));
    defaultside = climit(map(totalSide / samples,lside,mside,-255,255));
    defaultThrot = climit(map(totalThrot / samples,lthrot,mside,-255,255));

    delay(50);
    Serial.print("Default accAxis: ");
    Serial.println(defaultacc);
    Serial.print("Default sideAxis: ");
    Serial.println(defaultside);
    Serial.print("Default ThrotAxis: ");
    Serial.println(defaultThrot);
}
int binStickVal(int n){

}

int climit(int n) {
  if (n >= 255) {
    return 255;
  } 
  else if (n <= -255) {
    return -255;
  } 
  else {
    return n;
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  pinMode(accAxisPin, INPUT);
  pinMode(sideAxisPin, INPUT);

  // pinMode(RBButton, INPUT);
  // pinMode(LBButton, INPUT);

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

  // Calibrate joystick
  calibrateMinJoystick();
  delay(5);
  calibrateJoystick();
}


bool button_state(int a) {
  int s = 0;
  for (int i = 0; i < 4; i++) {
    s += digitalRead(a);
    delay(3);
  }
  return (s > 2) ? true : false;
}

int analogReadSmooth(int pin) {
    int total = 0;
    for (int i = 0; i < 3; i++) {
        total += analogRead(pin);
        delay(10);
    }
    return total / 3;
}


int stick_value(int sp) {
  int abs_sp = abs(sp);
  int output = 0;

  if (abs_sp <= deadzone) {  // Deadzone
    output = 0;
  } else if (abs_sp <= 250) {                // analog range
    output = map(abs_sp, 31, 255, 70, 255);  // Scale to 70-280
  }
  else{
    output = 255;
  }

  if (sp <0){
    output *= -1;
  }
  return output;
}


void loop() {
  // Read analog values for throttle only
  throt = analogReadSmooth(throtAxisPin);

  // Check if the values of acc and side are above the limit (e.g., deadzone)
  int accRaw = analogRead(accAxisPin);
  int sideRaw = analogRead(sideAxisPin);
  
  // Define a threshold limit for acc and side to switch them to digital
  int accThreshold = 500; // Adjust this threshold as needed
  int sideThreshold = 500; // Adjust this threshold as needed

  // Set acc and side to 255 if they exceed the threshold, otherwise set to 0
  acc = (accRaw > accThreshold) ? 255 : 0;
  side = (sideRaw > sideThreshold) ? 255 : 0;

  // Process throttle value with analog mapping
  throt = climit(stick_value(map(throt, lthrot, mthrot, 0, 255)));

  // Apply logic to halve acc if LSPButton is pressed
  Lsp = !button_state(LSPButton);
  if (Lsp) {
    acc = acc / 2;
  }

  // Prepare data to send
  Data.acc = acc;
  Data.side = side;
  Data.throt = throt;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Data, sizeof(Data));

  // Output the result
  Serial.print("Acc: ");
  Serial.print(acc);
  Serial.print(" Side: ");
  Serial.print(side);
  Serial.print(" Throt: ");
  Serial.println(throt);

  if (result == ESP_OK) {
    Serial.println("S");
  } else {
    Serial.println("E");
  }

  delay(10);
}
