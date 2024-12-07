
WiFiUDP UDP;
char packet[255];
char reply[] = "Packet received!";

String sParams[4];
int iCount, i;
String sLine;
double Direction = 0;

int cc_speed = 1300; //Clockwise speed
int cw_speed = 1700; //Counter clockwise speed

//Servo pins refer to circuit diagram
Servo fl;
Servo fr;
Servo bl;
Servo br;
//Speed varialbles for each wheel
int flss;
int frs;
int bls;
int brs;

void setup() {
  fl.attach(15);//d8-Forward left
  fr.attach(D7);//Forward right
  bl.attach(D6);//Back left
  br.attach(D5);//Back right 
  // Setup serial port
  Serial.begin(115200);
  Serial.println();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.clearDisplay();

  eyeMoves();
  // Begin WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS); 
  // Connecting to WiFi...
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  // Loop continuously while WiFi is not connected
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  // Connected to WiFi
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  // Begin listening to UDP port
  UDP.begin(UDP_PORT);
  Serial.print("Listening on UDP port ");
  Serial.println(UDP_PORT);
}

  int len2;
void loop() {
  // If packet received...
  int packetSize = UDP.parsePacket();
  if (packetSize) {
    //Serial.print("Received packet! Size: ");
    //Serial.println(packetSize); 
    int len = UDP.read(packet, 255);
    len2 = len;
    if (len > 0)
    {
      packet[len] = '\0';
    }
    Serial.print("Packet received: ");
    Serial.println(packet);
   sLine = packet;
    // parse only if exists
    if (sLine.length() > 0) {
    // parse the line
    iCount = StringSplit(sLine,',',sParams,4);
    // print the extracted paramters
    for(i=0;i<iCount;i++) {
       // Serial.println(sParams[i]);
    }
    //Direction = atan2(sParams[1].toDouble(),sParams[0].toDouble());
    //Serial.println((Direction));
    
    Serial.println("");
}  

    flss=map(sParams[0].toInt() ,-99,99,1000,2000);//Forward left wheel speed
    frs=map(sParams[1].toInt() ,-99,99,1000,2000);//Forward right wheel speed 
    bls=map(sParams[2].toInt() ,-99,99,1000,2000);//Back left speed
    brs=map(sParams[3].toInt() ,-99,99,1000,2000);//Back right speed

   char buffer[40];
   sprintf(buffer," %d %d",flss,frs);
   Serial.println(buffer);
   sprintf(buffer," %d %d",bls,brs);
   Serial.println(buffer);
    
   
   //Serial.println(flss +"\t"+frs);
  // Serial.prin
  // Serial.println(bls+"\t"+brs);
    motor_forward(flss,frs,bls, brs);
    //Serial.println("Mapped");
   
    //Serial.println(bls);
   // Serial.println(brs);
  }
  //blink_eye();
  //motor_stop();
}



void motor_forward(int flss,int frs,int bls,int brs)
{
   fl.writeMicroseconds(flss);
   fr.writeMicroseconds(frs);
   bl.writeMicroseconds(bls);
   br.writeMicroseconds(brs);
  }
void motor_stop()
{
   fl.writeMicroseconds(1500);
   fr.writeMicroseconds(1500);
   bl.writeMicroseconds(1500);
   br.writeMicroseconds(1500);
   eyeMoves();  
    }
