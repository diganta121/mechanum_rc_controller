#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Pins
const int flP = 33;
const int flN = 25;
const int frN = 26;
const int frP = 27;

const int enfl = 19; // Enable pins
const int enfr = 21;

const int ch_acc = 22;  // RC pins
const int ch_side = 23;
const int ch_throttle = 4;

const int deadzonea = 100;
const int deadzones = 100;
const int timeout = 25000; // Timeout in milliseconds

int acc = 0;
int side = 0;
int throttle = 0;

// Speed variables for each wheel
int Lmotor = 0;
int Rmotor = 0;

unsigned long last_signal_time = 0;

const unsigned int commandInterval = 25;  // Equivalent to delaytime
unsigned long lastCommandTime = 0;

unsigned long lastCommandTime2 = 0; // for lost contact
unsigned long currMilis =0;


uint8_t newMACAddress[] = { 0x36, 0x69, 0x2B, 0xFC, 0x4D, 0x3F };

typedef struct struct_message {
  int Throttle;
  int acc;
  int side;
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
  pinMode(flP, OUTPUT);
  pinMode(flN, OUTPUT);
  pinMode(frP, OUTPUT);
  pinMode(frN, OUTPUT);
  pinMode(enfl, OUTPUT);
  pinMode(enfr, OUTPUT);

  pinMode(ch_acc, INPUT);
  pinMode(ch_side, INPUT);
  pinMode(ch_throttle, INPUT);

  Serial.begin(115200);

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
  Serial.println("Bot started");

}
void loop() {
  currMilis = millis();
  if (currMilis - lastCommandTime > commandInterval) {
    commands();
    lastCommandTime = millis();
  }
  if (currMilis - lastCommandTime2 > 2000) {
    myData.RState = 0;
    myData.LState = 0;
    lastCommandTime2 = millis();
  }
}

void commands() {
  last_signal_time = millis();

  throttle = myData.Throttle //map(throttle_inp, 1160, 1900, 0, 255);
  acc = myData.acc //map(acc_inp, 1160, 1900, -255, 255);
  side = myData.side //map(side_inp, 1930, 1050, -255, 255);


  // Map throttle to low-resolution values
  int throttle_res = map_to_low_res(throttle);
  analogWrite(enfl, throttle_res);
  analogWrite(enfr, throttle_res);

  // Determine motor speeds based on inputs
  if (abs(acc) < deadzonea && abs(side) > deadzones) {
    // Tank turn
    Lmotor = climt(+side);
    Rmotor = climt(-side);
  } else if (abs(acc) > deadzonea && abs(side) > deadzones) {
    // Moving while turning
    Lmotor = climt(acc + side / 2);
    Rmotor = climt(acc - side / 2);
    analogWrite(enfl, climt(throttle_res-side/2));
    analogWrite(enfr, climt(throttle_res+side/2));
    
  } else if (abs(acc) > deadzonea && abs(side) < deadzones) {
    // Straight movement
    Lmotor = climt(acc);
    Rmotor = climt(acc);
  } else {
    // Stop
    Lmotor = 0;
    Rmotor = 0;
  }


  // Debugging
  Serial.print("Throttle: ");
  Serial.print(throttle_res);
  Serial.print(" ACC: ");
  Serial.print(acc);
  Serial.print(" SIDE: ");
  Serial.print(side);
  Serial.print("L: ");
  Serial.print(Lmotor);
  Serial.print("R: ");
  Serial.println(Rmotor);


  // Move the motors
  motor_move(Lmotor, flP, flN);
  motor_move(Rmotor, frP, frN);

  delay(5);
  // Timeout check
  if (millis() - last_signal_time > timeout) {
    stop_motors();
  }
}

// Limit motor speed with proper handling of reverse motion
int climt(int n) {
  if (n > 255) return 255;
  if (n < -255) return -255;
  return n;
}

void motor_move(int sp, int p, int n) {
  if (abs(sp) < deadzonea) {
    // Stop the motor
    digitalWrite(p, LOW);
    digitalWrite(n, LOW);
  } else if (sp < 0) {
    digitalWrite(p, LOW);
    digitalWrite(n, HIGH);
  } else if (sp > 0) {
    digitalWrite(p, HIGH);
    digitalWrite(n, LOW);
  }
}

int map_to_low_res(int value) {
  if (value <= 50) return 0;
  else if (value <= 220) {
    return map(value,50,200,90,240);
  }
  else return 255;
}

void stop_motors() {
  digitalWrite(flP, LOW);
  digitalWrite(flN, LOW);
  digitalWrite(frP, LOW);
  digitalWrite(frN, LOW);
  analogWrite(enfl, 0);
  analogWrite(enfr, 0);
  Serial.println("Motors stopped due to no signal.");
  delay(10);
}
