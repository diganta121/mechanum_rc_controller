//pins
const int flP = 16;  // p side
const int frP = 17;
const int blP = 18;
const int brP = 19;

const int flN = 16;  // N side
const int frN = 17;
const int blN = 18;
const int brN = 19;

const int enfl = 16;  //enable pins
const int enfr = 17;
const int enbl = 18;
const int enbr = 19;

const int ch_speed = 5;  //rc pins
const int ch_strafe = 6;
const int ch_rotate = 7;

const int deadzone = 2;

int speed = 0;
int strafe = 0;
int rotate = 0;

//Speed varialbles for each wheel
int flss = 0;
int frs = 0;
int bls = 0;
int brs = 0;

bool KILLSWITCH = false;

void setup() {
  // pinMode(flP, OUTPUT);   //d8-Forward left
  // pinMode(frP, OUTPUT);   //Forward right
  // pinMode(blP, OUTPUT);   //Back left
  // pinMode(brP, OUTPUT);   //Back right
  // pinMode(flN, OUTPUT);   //d8-Forward left
  // pinMode(frN, OUTPUT);   //Forward right
  // pinMode(blN, OUTPUT);   //Back left
  // pinMode(brN, OUTPUT);   //Back right
  // pinMode(enfl, OUTPUT);  //d8-Forward left
  // pinMode(enfr, OUTPUT);  //Forward right
  // pinMode(enbl, OUTPUT);  //Back left
  // pinMode(enbr, OUTPUT);  //Back right

  pinMode(ch_speed, INPUT);  // input
  pinMode(ch_strafe, INPUT);
  pinMode(ch_rotate, INPUT);


  Serial.begin(115200);
  Serial.println("bot started: ");
}

void loop() {
  int speed_inp = pulseIn(ch_speed, HIGH);
  speed = map(speed_inp, 1000, 2000, -255, 255);

  int strafe_inp = pulseIn(ch_strafe, HIGH);
  strafe = map(strafe_inp, 1000, 2000, -255, 255);

  int rotate_inp = pulseIn(ch_rotate, HIGH);
  rotate = map(rotate_inp, 1000, 2000, -255, 255);

  Serial.print(speed);
  Serial.print(strafe);
  Serial.print(rotate);

  // Loop continuously if kill switch pressed
  // while (KILL_SWITCH) {
  //   delay(100);
  //   Serial.print(".");
  // }

  flss = climt(speed + strafe - rotate);  //map(sParams[0].toInt(), -99, 99, 1000, 2000);  //Forward left wheel speed
  frs = climt(speed - strafe - rotate);   //map(sParams[1].toInt(), -99, 99, 1000, 2000);   //Forward right wheel speed
  bls = climt(speed - strafe + rotate);   // map(sParams[2].toInt(), -99, 99, 1000, 2000);   //Back left speed
  brs = climt(speed + strafe - rotate);   // map(sParams[3].toInt(), -99, 99, 1000, 2000);   //Back right speed

  Serial.print(flss);
  Serial.print(frs);
  Serial.print(bls);
  Serial.print(brs); //ln

  Serial.print(speed_inp);
  Serial.print(strafe_inp);
  Serial.print(rotate_inp);


  // motor_move(flss,flP,flN,enfl);
  // motor_move(frs,frP,frN,enfr);

  // motor_move(bls,blP,blN,enbl);
  // motor_move(brs,brP,brN,enbr);
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
  if (sp < -deadzone) {
    digitalWrite(p, LOW);
    digitalWrite(n, HIGH);
    analogWrite(enpin, -sp);
  }
  else if (sp > deadzone) {
    digitalWrite(p, HIGH);
    digitalWrite(n, LOW);
    analogWrite(enpin, sp);
  }
  // elif else
}

// void stop() {
//   digitalWrite(enfl, LOW);
//   digitalWrite(enfr, LOW);
//   digitalWrite(enbl, LOW);
//   digitalWrite(enbr, LOW);
// }
