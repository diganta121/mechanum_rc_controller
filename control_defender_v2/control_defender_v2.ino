//pins
const int flP = 1;  // p side
const int frP = 3;


const int flN = 2;  // N side
const int frN = 4;


const int enfl = 10;  //enable pins
const int enfr = 11;


const int ch_speed = 28;  //rc pins
const int ch_strafe = 27;

const int deadzone =  10;

int speed = 0;
int strafe = 0;

int speed_inp = 0;
int strafe_inp = 0;
//Speed varialbles for each wheel
int flss = 0;
int frs = 0;


bool KILLSWITCH = false;

void setup() {
  pinMode(flP, OUTPUT);   //d8-Forward left
  pinMode(frP, OUTPUT);   //Forward right
  pinMode(flN, OUTPUT);   //d8-Forward left
  pinMode(frN, OUTPUT);   //Forward right
  pinMode(enfl, OUTPUT);  //d8-Forward left
  pinMode(enfr, OUTPUT);  //Forward right

  pinMode(ch_speed, INPUT);  // input
  pinMode(ch_strafe, INPUT);


  Serial.begin(115200);
  // Serial.println("bot started: ");
}

void loop() {
  int speed_inp = pulseIn(ch_speed, HIGH);
  speed = map(speed_inp, 1160, 1900, -255, 255);

  int strafe_inp = pulseIn(ch_strafe, HIGH);
  strafe = map(strafe_inp, 1930, 1050, -255, 255);
  // Serial.print(speed_inp);
  // Serial.print(" ");
  // Serial.print(strafe_inp); 
  // Serial.print(" ");

  // Serial.print(rotate_inp);
  // Serial.print("  ");

  Serial.print(speed);
  Serial.print(" ");

  Serial.print(strafe);  


  // Loop continuously if kill switch pressed
  // while (KILL_SWITCH) {
  //   delay(100);
  //   Serial.print(".");
  // }

  flss = climt(speed); //Forward left wheel speed
  frs = climt(speed);   //map(sParams[1].toInt(), -99, 99, 1000, 2000);   //Forward right wheel speed

  Serial.print(" ");
  Serial.print(flss);
  Serial.print("  ");

  Serial.print(frs);
  Serial.print(" ");

  Serial.print(bls);  
  Serial.print(" ");

  Serial.println(brs);  

  motor_move(flss,flP,flN,enfl);
  motor_move(frs,frP,frN,enfr);
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
  else {  // High speed range
    output = 255;
  }

  if (sp < -deadzone) {
    digitalWrite(p, LOW);
    digitalWrite(n, HIGH);
    analogWrite(enpin, output);
  }
  else if (sp > deadzone) {
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
