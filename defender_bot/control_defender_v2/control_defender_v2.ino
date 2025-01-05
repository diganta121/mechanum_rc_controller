#include <WiFi.h>
#include <WebServer.h>

// Motor pins
#define A1 21
#define A2 23
#define B1 32
#define B2 33
#define SPA 13
#define SPB 14

// Network credentials
const char* ssid = "Cheese_bot";
const char* password = "soccerrobo";

const unsigned int commandInterval = 25;
unsigned long lastCommandTime = 0;
unsigned long currMilis = 0;

WebServer server(80);

// Structure to hold motor states
struct MotorStates {
  int RState;
  int LState;
} myData = {0, 0};

void setup() {
  Serial.begin(115200);
  
  // Motor pin setup
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);
  pinMode(SPA, OUTPUT);
  pinMode(SPB, OUTPUT);

  // Create AP
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/control", HTTP_POST, handleControl);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  
  currMilis = millis();
  if (currMilis - lastCommandTime > commandInterval) {
    commands();
    lastCommandTime = millis();
  }
}

void handleRoot() {
  String html = R"(
    <!DOCTYPE html>
    <html>
    <head>
      <meta name='viewport' content='width=device-width, initial-scale=1'>
      <style>
        body {
          margin: 0;
          padding: 20px;
          display: flex;
          justify-content: center;
          align-items: center;
          min-height: 100vh;
          background: #f0f0f0;
          font-family: Arial, sans-serif;
        }
        .container {
          display: flex;
          gap: 20px;
        }
        .column {
          display: flex;
          flex-direction: column;
          gap: 10px;
        }
        button {
          width: 120px;
          height: 80px;
          border: none;
          border-radius: 10px;
          background: #007bff;
          color: white;
          font-size: 16px;
          cursor: pointer;
          transition: background 0.3s;
        }
        button:active {
          background: #0056b3;
        }
      </style>
    </head>
    <body>
      <div class='container'>
        <div class='column'>
          <button onmousedown='control("L", 255)' onmouseup='control("L", 0)' ontouchstart='control("L", 255)' ontouchend='control("L", 0)'>Left Full Forward</button>
          <button onmousedown='control("L", 128)' onmouseup='control("L", 0)' ontouchstart='control("L", 128)' ontouchend='control("L", 0)'>Left Half Forward</button>
          <button onmousedown='control("L", -128)' onmouseup='control("L", 0)' ontouchstart='control("L", -128)' ontouchend='control("L", 0)'>Left Half Back</button>
          <button onmousedown='control("L", -255)' onmouseup='control("L", 0)' ontouchstart='control("L", -255)' ontouchend='control("L", 0)'>Left Full Back</button>
        </div>
        <div class='column'>
          <button onmousedown='control("R", 255)' onmouseup='control("R", 0)' ontouchstart='control("R", 255)' ontouchend='control("R", 0)'>Right Full Forward</button>
          <button onmousedown='control("R", 128)' onmouseup='control("R", 0)' ontouchstart='control("R", 128)' ontouchend='control("R", 0)'>Right Half Forward</button>
          <button onmousedown='control("R", -128)' onmouseup='control("R", 0)' ontouchstart='control("R", -128)' ontouchend='control("R", 0)'>Right Half Back</button>
          <button onmousedown='control("R", -255)' onmouseup='control("R", 0)' ontouchstart='control("R", -255)' ontouchend='control("R", 0)'>Right Full Back</button>
        </div>
      </div>
      <script>
        function control(side, value) {
          fetch('/control', {
            method: 'POST',
            headers: {
              'Content-Type': 'application/x-www-form-urlencoded',
            },
            body: `side=${side}&value=${value}`
          });
        }
      </script>
    </body>
    </html>
  )";
  server.send(200, "text/html", html);
}

void handleControl() {
  if (server.hasArg("side") && server.hasArg("value")) {
    String side = server.arg("side");
    int value = server.arg("value").toInt();
    
    if (side == "L") {
      myData.LState = value;
    } else if (side == "R") {
      myData.RState = value;
    }
    
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void commands() {
  int R = constrain(myData.RState, -255, 255);
  int L = constrain(myData.LState, -255, 255);

  // Right side motor control
  if (R > 10) {
    MRF();
    analogWrite(SPA, R);
  } else if (R < -10) {
    MRB();
    analogWrite(SPA, -R);
  } else {
    MRS();
    analogWrite(SPA, 0);
  }

  // Left side motor control
  if (L > 10) {
    MLF();
    analogWrite(SPB, L);
  } else if (L < -10) {
    MLB();
    analogWrite(SPB, -L);
  } else {
    MLS();
    analogWrite(SPB, 0);
  }

  Serial.print("L: ");
  Serial.print(L);
  Serial.print(" R: ");
  Serial.println(R);
}

void MRF() {
  digitalWrite(A1, HIGH);
  digitalWrite(A2, LOW);
}

void MRB() {
  digitalWrite(A1, LOW);
  digitalWrite(A2, HIGH);
}

void MRS() {
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
}

void MLF() {
  digitalWrite(B1, HIGH);
  digitalWrite(B2, LOW);
}

void MLB() {
  digitalWrite(B1, LOW);
  digitalWrite(B2, HIGH);
}

void MLS() {
  digitalWrite(B1, LOW);
  digitalWrite(B2, LOW);
}