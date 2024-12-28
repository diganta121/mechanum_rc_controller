//pins
const int flP = 1;
const int flN = 2;

const int frN = 4;
const int frP = 3;

const int enfl = 10;  //enable pins
const int enfr = 11;

const int ch_acc = 28;  //rc pins
const int ch_side = 27;

const int deadzonea =  30;
const int deadzones =  30;

int acc = 0;
int side = 0;

int acc_inp = 0;
int side_inp = 0;
//acc varialbles for each wheel
int Lmotor = 0;
int Rmotor = 0;

void setup() {
  pinMode(flP, OUTPUT);   //d8-Forward left
  pinMode(flN, OUTPUT);   //d8-Forward left
  pinMode(frP, OUTPUT);   //Forward right
  pinMode(frN, OUTPUT);   //Forward right
  pinMode(enfl, OUTPUT);  //d8-Forward left
  pinMode(enfr, OUTPUT);  //Forward right

  pinMode(ch_acc, INPUT);  // input
  pinMode(ch_side, INPUT);

  Serial.begin(115200);
  // Serial.println("bot started: ");
}
void loop() {
  int acc_inp = pulseIn(ch_acc, HIGH);
  acc = map(acc_inp, 1160, 1900, -255, 255);

  int side_inp = pulseIn(ch_side, HIGH);
  side = map(side_inp, 1930, 1050, -255, 255);

  Serial.print(acc);
  Serial.print(" ");
  Serial.print(side);
  Serial.print(" ");
  int accabs = abs(acc);
  int sideabs = abs(side);
  if (accabs < deadzonea && sideabs > deadzones) {
    // Tank turn: opposite speeds for left and right motors
    Lmotor = climt(side);
    Rmotor = -climt(side);
  } 
  else if (accabs > deadzonea && sideabs > deadzones) {
    // Moving while turning: adjust speeds
    Lmotor = climt(acc + side / 2); // Add side for left
    Rmotor = climt(acc - side / 2); // Subtract side for right
  } 
  else if (accabs > deadzonea && sideabs < deadzones) {
    // Straight movement: both motors same speed
    Lmotor = climt(acc);
    Rmotor = climt(acc);
  } 
  else {
    // Within deadzone, stop the motors
    Lmotor = 0;
    Rmotor = 0;
  }

  // Move the motors
  motor_move(Lmotor, flP, flN, enfl);
  motor_move(Rmotor, frP, frN, enfr);

  delay(10); // Small delay for stability
}


int climt(int n) {
  if (n >= 255) {
    return (255);
  } 
  else if (n <= 0) {
    return (0);
  } 
  else {
    return (n);
  }
}

void motor_move(int sp, int p, int n, int enpin) {
  int abs_sp = abs(sp);
  int output = 0;

  if (abs_sp <= 30) {  // Deadzone
    output = 0;
  }
  else if (abs_sp <= 200) {  // analog range
    output = map(abs_sp, 31, 200, 70, 200); // Scale to 1-127
  }
  else {  // High acc range
    output = 255;
  }

  if (sp < 10) {
    digitalWrite(p, LOW);
    digitalWrite(n, HIGH);
    analogWrite(enpin, output);
  }
  else if (sp > 10) {
    digitalWrite(p, HIGH);
    digitalWrite(n, LOW);
    analogWrite(enpin, output);
  }
  else {  // Within deadzone, stop the motor
    digitalWrite(p, LOW);
    digitalWrite(n, LOW);
    analogWrite(enpin, 0);
  }
}

// void stop() {
//   digitalWrite(enfl, LOW);
//   digitalWrite(enfr, LOW);
//   digitalWrite(enbl, LOW);
//   digitalWrite(enbr, LOW);
// }
