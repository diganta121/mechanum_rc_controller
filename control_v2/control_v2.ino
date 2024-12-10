double Direction = 0;

int cc_speed = 1300;  //Clockwise speed
int cw_speed = 1700;  //Counter clockwise speed

//pins
const int fl = 16; // p side
const int fr = 17;
const int bl = 18;
const int br = 19;

const int fl = 16; // N side
const int fr = 17;
const int bl = 18;
const int br = 19;

const int enfl = 16;//enable pins
const int enfr = 17;
const int enbl = 18;
const int enbr = 19;

const int ch_Speed = 5;
const int ch_strafe = 6;
const int ch_rotate = 7;

int speed = 0;
int strafe = 0;
int rotation = 0;

//Speed varialbles for each wheel
int flss;
int frs;
int bls;
int brs;

bool KILLSWITCH = false;

void setup() {
  pinMode(fl, OUTPUT);  //d8-Forward left
  pinMode(fr, OUTPUT);  //Forward right
  pinMode(bl, OUTPUT);  //Back left
  pinMode(br, OUTPUT);  //Back right

  Serial.begin(115200);
  Serial.println();


  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  // Begin listening to UDP port
  UDP.begin(UDP_PORT);
  Serial.print("Listening on UDP port ");
  Serial.println(UDP_PORT);
}

void loop() {
  // int packetSize = UDP.parsePacket();
  int speed_inp = pulsein(ch_Speed);
  speed = map(speed_inp, 1000, 2000, -255, 255);

  int strafe_inp = pulsein(ch_Speed);
  strafe = map(strafe_inp, 1000, 2000, -255, 255);
  
  int rotation_inp = pulsein(ch_Speed);
  rotation = map(rotation_inp, 1000, 2000, -255, 255);

  // Loop continuously if kill switch pressed
  while (KILL_SWITCH) {
    delay(100);
    Serial.print(".");
  }

  //Serial.print("Received packet! Size: ");
  //Serial.println(packetSize);

  //Direction = atan2(sParams[1].toDouble(),sParams[0].toDouble());
  //Serial.println((Direction));


  flss = climt(speed + strafe - rotate); //map(sParams[0].toInt(), -99, 99, 1000, 2000);  //Forward left wheel speed
  frs = climt(speed - strafe - rotate); //map(sParams[1].toInt(), -99, 99, 1000, 2000);   //Forward right wheel speed
  bls = climt(speed - strafe + rotate ); // map(sParams[2].toInt(), -99, 99, 1000, 2000);   //Back left speed
  brs = climt(speed + strafe - rotate ); // map(sParams[3].toInt(), -99, 99, 1000, 2000);   //Back right speed

  char buffer[40];
  sprintf(buffer, " %d %d", flss, frs);
  Serial.println(buffer);
  sprintf(buffer, " %d %d", bls, brs);
  Serial.println(buffer);


  //Serial.println(flss +"\t"+frs);
  // Serial.println(bls+"\t"+brs);
  motor_forward(flss, frs, bls, brs);
  //Serial.println("Mapped");

  //Serial.println(bls);
  // Serial.println(brs);
}

int climt(int n){
  if (n>255){
    return (255);
  }
  else if(n<0){
    return (0);
  }
  else {
    return (n);
  }
}

void motor(int sp, int p, int n,int enpin,int en){
  if (sp<0){
    analogWrite(enpin,sp);
    digitalWrite(p,LOW);
  }
  // elif else
}

void stop() {
  digitalWrite(en,LOW);
}
