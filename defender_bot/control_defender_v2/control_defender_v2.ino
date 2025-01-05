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
const int timeout = 1000; // Timeout in milliseconds (1 second)

int acc = 0;
int side = 0;
int throttle = 0;

// Speed variables for each wheel
int Lmotor = 0;
int Rmotor = 0;

unsigned long last_signal_time = 0;

const unsigned int commandInterval = 25;  // Equivalent to delaytime
unsigned long lastCommandTime = 0;

uint8_t newMACAddress[] = { 0x36, 0x69, 0x2B, 0xFC, 0x4D, 0x3F };

typedef struct struct_message {
  int acc;
  int side;
  int Throttle;
} struct_message;

struct_message myData = { 0, 0, 0 };

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
  
  if (esp_wifi_set_mac(WIFI_IF_STA, newMACAddress) != ESP_OK) {
    Serial.println("Failed to set MAC address");
  } else {
    Serial.println("MAC address changed successfully");
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Bot started");
}

void loop() {
  unsigned long currMilis = millis();
  
  if (currMilis - lastCommandTime > commandInterval) {
    if (myData.Throttle != 0) {
      commands();
    } else {
      stop_motors(); // Stop motors if throttle is 0
    }
    lastCommandTime = currMilis;
  }

  // Stop motors if no signal for 1 second
  if (currMilis - last_signal_time > timeout) {
    stop_motors();
  }
}

void commands() {
  last_signal_time = millis(); // Reset the last signal time when receiving new commands

  throttle = myData.Throttle;
  acc = myData.acc;
  side = myData.side;

  int throttle_res = map_to_low_res(throttle);
  analogWrite(enfl, throttle_res);
  analogWrite(enfr, throttle_res);

  if (abs(acc) < deadzonea && abs(side) > deadzones) {
    Lmotor = climt(+side);
    Rmotor = climt(-side);
  } else if (abs(acc) > deadzonea && abs(side) > deadzones) {
    Lmotor = climt(acc + side / 2);
    Rmotor = climt(acc - side / 2);
    analogWrite(enfl, climt(throttle_res - side / 2));
    analogWrite(enfr, climt(throttle_res + side / 2));
  } else if (abs(acc) > deadzonea && abs(side) < deadzones) {
    Lmotor = climt(acc);
    Rmotor = climt(acc);
  } else {
    Lmotor = 0;
    Rmotor = 0;
  }

  Serial.print("Throttle: ");
  Serial.print(throttle_res);
  Serial.print(" ACC: ");
  Serial.print(acc);
  Serial.print(" SIDE: ");
  Serial.print(side);
  Serial.print(" L: ");
  Serial.print(Lmotor);
  Serial.print(" R: ");
  Serial.println(Rmotor);

  motor_move(Lmotor, flP, flN);
  motor_move(Rmotor, frP, frN);
  delay(2);
}

int climt(int n) {
  if (n > 255) return 255;
  if (n < -255) return -255;
  return n;
}

void motor_move(int sp, int p, int n) {
  if (abs(sp) < deadzonea) {
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
    return map(value, 50, 200, 90, 240);
  } else return 255;
}

void stop_motors() {
  digitalWrite(flP, LOW);
  digitalWrite(flN, LOW);
  digitalWrite(frP, LOW);
  digitalWrite(frN, LOW);
  analogWrite(enfl, 0);
  analogWrite(enfr, 0);
  Serial.println("Motors stopped.");
}
