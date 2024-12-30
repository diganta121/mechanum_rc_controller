// Pins
const int flP = 33;
const int flN = 25;
const int frN = 26;
const int frP = 27;

const int enfl = 19; // Enable pins
const int enfr = 21;

const int ch_acc = 22;  // RC pins
const int ch_side = 23;

const int deadzonea = 100;
const int deadzone2 = 100;

int acc = 0;
int side = 0;

// Speed variables for each wheel
int Lmotor = 0;
int Rmotor = 0;

void setup() {
  pinMode(flP, OUTPUT);
  pinMode(flN, OUTPUT);
  pinMode(frP, OUTPUT);
  pinMode(frN, OUTPUT);
  pinMode(enfl, OUTPUT);
  pinMode(enfr, OUTPUT);

  pinMode(ch_acc, INPUT);
  pinMode(ch_side, INPUT);

  Serial.begin(115200);
  Serial.println("Bot started");
}

void loop() {
  // Read inputs
  int acc_inp = pulseIn(ch_acc, HIGH);
  acc = map(acc_inp, 1175, 1900, -255, 255);

  int side_inp = pulseIn(ch_side, HIGH);
  side = map(side_inp, 1930, 1050, -255, 255);

  // Debugging
  Serial.print("ACC: ");
  Serial.print(acc);
  Serial.print(" SIDE: ");
  Serial.print(side);

  // Determine motor speeds based on inputs
  if (abs(acc) < deadzonea && abs(side) > deadzonea) {
    // Tank turn
    Lmotor = climt(side);
    Rmotor = -climt(side);
  } else if (abs(acc) > deadzonea && abs(side) > deadzonea) {
    // Moving while turning
    Lmotor = climt(acc + side / 2);
    Rmotor = climt(acc - side / 2);
  } else if (abs(acc) > deadzonea && abs(side) < deadzonea) {
    // Straight movement
    Lmotor = climt(acc);
    Rmotor = climt(acc);
  } else {
    // Stop
    Lmotor = 0;
    Rmotor = 0;
  }
  Serial.print(" C3: ");
  Serial.print(acc_inp);
  
  Serial.print(" C4: ");
  Serial.print(side_inp);

  Serial.print(" L: ");
  Serial.print(Lmotor);
  
  Serial.print(" R: ");
  Serial.println(Rmotor);
  
  // Move the motors
  motor_move(Lmotor, flP, flN, enfl);
  motor_move(Rmotor, frP, frN, enfr);

  delay(20); // Small delay for stability
}

// Limit motor speed with proper handling of reverse motion
int climt(int n) {
  if (n > 255) return 255;
  if (n < -255) return -255;
  return n;
}

void motor_move(int sp, int p, int n, int enpin) {
  if (abs(sp) < deadzone2) {
    // Stop the motor
    digitalWrite(p, LOW);
    digitalWrite(n, LOW);
    digitalWrite(enpin, LOW); // Disable the motor
  } else if (sp < 0) {
    digitalWrite(p, LOW);
    digitalWrite(n, HIGH);
    digitalWrite(enpin, HIGH); // Enable the motor
  } else if (sp > 0) {
    digitalWrite(p, HIGH);
    digitalWrite(n, LOW);
    digitalWrite(enpin, HIGH); // Enable the motor
  }
}
