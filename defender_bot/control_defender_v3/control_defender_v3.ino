#include <WiFi.h>
#include <WebServer.h>

// Motor pins
#define FLP 33  // Front Left Positive
#define FLN 25  // Front Left Negative
#define FRP 27  // Front Right Positive
#define FRN 26  // Front Right Negative

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
  pinMode(FLP, OUTPUT);
  pinMode(FLN, OUTPUT);
  pinMode(FRP, OUTPUT);
  pinMode(FRN, OUTPUT);
  
  // Set initial motor states
  digitalWrite(FLP, LOW);
  digitalWrite(FLN, LOW);
  digitalWrite(FRP, LOW);
  digitalWrite(FRN, LOW);

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
          justify-content: space-between;
          align-items: center;
          min-height: 100vh;
          background: #f0f0f0;
          font-family: Arial, sans-serif;
          touch-action: none;
        }
        .slider-container {
          height: 400px;
          display: flex;
          flex-direction: column;
          align-items: center;
          margin: 0 20px;
        }
        .slider {
          width: 100px;
          height: 400px;
          background: #ddd;
          border-radius: 15px;
          position: relative;
          touch-action: none;
        }
        .slider-thumb {
          width: 80px;
          height: 60px;
          background: #007bff;
          border-radius: 10px;
          position: absolute;
          left: 10px;
          top: 170px;
          cursor: pointer;
          touch-action: none;
          user-select: none;
        }
        .label {
          margin-top: 10px;
          font-size: 18px;
          font-weight: bold;
        }
        .value-display {
          margin-top: 5px;
          font-size: 16px;
        }
      </style>
    </head>
    <body>
      <div class="slider-container">
        <div class="slider" id="leftSlider">
          <div class="slider-thumb" id="leftThumb"></div>
        </div>
        <div class="label">Left Motor</div>
        <div class="value-display" id="leftValue">0</div>
      </div>
      <div class="slider-container">
        <div class="slider" id="rightSlider">
          <div class="slider-thumb" id="rightThumb"></div>
        </div>
        <div class="label">Right Motor</div>
        <div class="value-display" id="rightValue">0</div>
      </div>
      <script>
        let lastSentTime = 0;
        const MIN_SEND_INTERVAL = 50;
        let leftValue = 0;
        let rightValue = 0;

        class Slider {
          constructor(sliderId, thumbId, valueId, side) {
            this.slider = document.getElementById(sliderId);
            this.thumb = document.getElementById(thumbId);
            this.valueDisplay = document.getElementById(valueId);
            this.side = side;
            this.isDragging = false;
            this.startY = 0;
            this.startThumbY = 0;
            this.value = 0;
            
            this.setupEventListeners();
          }

          setupEventListeners() {
            this.thumb.addEventListener('mousedown', (e) => this.startDragging(e));
            document.addEventListener('mousemove', (e) => this.drag(e));
            document.addEventListener('mouseup', () => this.stopDragging());

            this.thumb.addEventListener('touchstart', (e) => this.startDragging(e));
            document.addEventListener('touchmove', (e) => this.drag(e));
            document.addEventListener('touchend', () => this.stopDragging());
          }

          startDragging(e) {
            this.isDragging = true;
            if (e.type === 'mousedown') {
              this.startY = e.clientY;
            } else {
              this.startY = e.touches[0].clientY;
            }
            this.startThumbY = this.thumb.offsetTop;
          }

          drag(e) {
            if (!this.isDragging) return;
            
            e.preventDefault();
            let currentY;
            if (e.type === 'mousemove') {
              currentY = e.clientY;
            } else {
              currentY = e.touches[0].clientY;
            }

            let deltaY = currentY - this.startY;
            let newTop = this.startThumbY + deltaY;
            
            newTop = Math.max(0, Math.min(newTop, this.slider.offsetHeight - this.thumb.offsetHeight));
            this.thumb.style.top = newTop + 'px';

            let center = (this.slider.offsetHeight - this.thumb.offsetHeight) / 2;
            let range = 255;
            this.value = Math.round((center - newTop) * (range / center));
            this.valueDisplay.textContent = this.value;

            if (this.side === 'L') {
              leftValue = this.value;
            } else {
              rightValue = this.value;
            }

            const now = Date.now();
            if (now - lastSentTime >= MIN_SEND_INTERVAL) {
              sendControlValues();
              lastSentTime = now;
            }
          }

          stopDragging() {
            if (!this.isDragging) return;
            this.isDragging = false;
            
            this.thumb.style.transition = 'top 0.3s';
            this.thumb.style.top = '170px';
            this.value = 0;
            this.valueDisplay.textContent = '0';
            
            if (this.side === 'L') {
              leftValue = 0;
            } else {
              rightValue = 0;
            }
            
            sendControlValues();
            
            setTimeout(() => {
              this.thumb.style.transition = 'none';
            }, 300);
          }
        }

        function sendControlValues() {
          fetch('/control', {
            method: 'POST',
            headers: {
              'Content-Type': 'application/x-www-form-urlencoded',
            },
            body: `side=B&left=${leftValue}&right=${rightValue}`
          });
        }

        new Slider('leftSlider', 'leftThumb', 'leftValue', 'L');
        new Slider('rightSlider', 'rightThumb', 'rightValue', 'R');
      </script>
    </body>
    </html>
  )";
  server.send(200, "text/html", html);
}

void handleControl() {
  if (server.hasArg("side") && server.arg("side").equals("B")) {  // Fixed string comparison
    int leftValue = server.hasArg("left") ? server.arg("left").toInt() : 0;
    int rightValue = server.hasArg("right") ? server.arg("right").toInt() : 0;
    
    myData.LState = leftValue;
    myData.RState = rightValue;
    
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}


void commands() {
  int R = constrain(myData.RState, -255, 255);
  int L = constrain(myData.LState, -255, 255);

  // Right motor control
  if (R > 10) {
    // Forward
    digitalWrite(FRP, HIGH);
    digitalWrite(FRN, LOW);
  } else if (R < -10) {
    // Backward
    digitalWrite(FRP, LOW);
    digitalWrite(FRN, HIGH);
  } else {
    // Stop
    digitalWrite(FRP, LOW);
    digitalWrite(FRN, LOW);
  }

  // Left motor control
  if (L > 10) {
    // Forward
    digitalWrite(FLP, HIGH);
    digitalWrite(FLN, LOW);
  } else if (L < -10) {
    // Backward
    digitalWrite(FLP, LOW);
    digitalWrite(FLN, HIGH);
  } else {
    // Stop
    digitalWrite(FLP, LOW);
    digitalWrite(FLN, LOW);
  }

  Serial.print("L: ");
  Serial.print(L);
  Serial.print(" R: ");
  Serial.println(R);
}