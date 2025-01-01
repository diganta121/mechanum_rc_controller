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

const int deadzonea = 30;
const int deadzones = 30;
const int timeout = 2000; // Timeout in milliseconds

int acc = 0;
int side = 0;
int throttle = 0;

// Speed variables for each wheel
int Lmotor = 0;
int Rmotor = 0;

unsigned long last_signal_time = 0;

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
  Serial.println("Bot started");
}

void loop() {
  // Read inputs
  int acc_inp = pulseIn(ch_acc, HIGH);
  int side_inp = pulseIn(ch_side, HIGH);
  int throttle_inp = pulseIn(ch_throttle, HIGH);

  if (acc_inp == 0 || side_inp == 0 || throttle_inp == 0) {
    // No valid signal received, stop the motors
      // Debugging
    Serial.print("no sig- Thr: ");
    Serial.print(throttle_inp);
    Serial.print(" ACC: ");
    Serial.print(acc_inp);
    Serial.print(" SIDE: ");
    Serial.println(side_inp);
    Serial.print(" ");

    stop_motors();
    return;
  }

  last_signal_time = millis();

  throttle = map(throttle_inp, 1160, 1900, 0, 255);
  acc = map(acc_inp, 1160, 1900, -255, 255);
  side = map(side_inp, 1930, 1050, -255, 255);

  // Debugging
  Serial.print("Throttle: ");
  Serial.print(throttle);
  Serial.print(" ACC: ");
  Serial.print(acc);
  Serial.print(" SIDE: ");
  Serial.println(side);

  // Map throttle to low-resolution values
  int throttle_res = map_to_low_res(throttle);
  analogWrite(enfl, throttle_res);
  analogWrite(enfr, throttle_res);

  // Determine motor speeds based on inputs
  if (abs(acc) < deadzonea && abs(side) > deadzones) {
    // Tank turn
    Lmotor = climt(throttle + side);
    Rmotor = climt(throttle - side);
  } else if (abs(acc) > deadzonea && abs(side) > deadzones) {
    // Moving while turning
    Lmotor = climt(throttle + acc + side / 2);
    Rmotor = climt(throttle + acc - side / 2);
  } else if (abs(acc) > deadzonea && abs(side) < deadzones) {
    // Straight movement
    Lmotor = climt(throttle + acc);
    Rmotor = climt(throttle + acc);
  } else {
    // Stop
    Lmotor = 0;
    Rmotor = 0;
  }

  // Move the motors
  motor_move(Lmotor, flP, flN);
  motor_move(Rmotor, frP, frN);

  delay(10); // Small delay for stability

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
  else if (value <= 100) return 70;
  else if (value <= 150) return 120;
  else if (value <= 200) return 190;
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
}
