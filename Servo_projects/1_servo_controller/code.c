#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// ====== WiFi Config ======
const char* ssid     = "ServoTeach";
const char* password = "12345678";

// ====== Servo Config ======
Servo myServo;
const int SERVO_PIN = 13;      // change if needed
const int SERVO_MIN_US = 500;  // typical for SG90
const int SERVO_MAX_US = 2400; // typical for SG90

// ====== Sequence Storage ======
const int MAX_STEPS = 50;
int sequenceAngles[MAX_STEPS];
int stepCount = 0;

WebServer server(80);

// ====== Helper: Move Servo Safely ======
void moveServoTo(int angle) {
  angle = constrain(angle, 0, 180);
  myServo.write(angle);
  Serial.print("Moving servo to: ");
  Serial.println(angle);
}

// ====== HTTP Handlers ======

// Root page: simple control UI
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Servo Teach & Play</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background: #111827;
      color: #E5E7EB;
      text-align: center;
      padding: 20px;
    }
    .card {
      max-width: 400px;
      margin: 0 auto;
      background: #1F2937;
      padding: 20px;
      border-radius: 16px;
      box-shadow: 0 10px 25px rgba(0,0,0,0.4);
    }
    h1 { font-size: 22px; margin-bottom: 10px; }
    h2 { font-size: 16px; margin-top: 20px; }
    input[type=range] {
      width: 100%;
    }
    button {
      margin: 6px;
      padding: 10px 16px;
      border-radius: 999px;
      border: none;
      cursor: pointer;
      font-weight: bold;
    }
    .primary { background: #3B82F6; color: white; }
    .secondary { background: #6B7280; color: white; }
    .danger { background: #EF4444; color: white; }
    #status {
      margin-top: 15px;
      font-size: 14px;
      color: #A5B4FC;
      min-height: 20px;
    }
    #stepCount {
      font-weight: bold;
      color: #22C55E;
    }
  </style>
</head>
<body>
  <div class="card">
    <h1>Servo Teach & Play</h1>
    <p>Move with slider, save steps, then play.</p>

    <h2>Angle: <span id="angleVal">90</span>°</h2>
    <input type="range" id="angleSlider" min="0" max="180" value="90" />

    <div style="margin-top: 15px;">
      <button class="primary" onclick="moveServo()">Move</button>
      <button class="secondary" onclick="saveStep()">Save Step</button>
    </div>

    <div style="margin-top: 10px;">
      <button class="primary" onclick="playSeq()">Play</button>
      <button class="danger" onclick="clearSeq()">Clear</button>
    </div>

    <p>Saved steps: <span id="stepCount">0</span></p>

    <div id="status"></div>
  </div>

  <script>
    const angleSlider = document.getElementById('angleSlider');
    const angleVal = document.getElementById('angleVal');
    const statusDiv = document.getElementById('status');
    const stepCountSpan = document.getElementById('stepCount');

    angleSlider.addEventListener('input', function() {
      angleVal.textContent = angleSlider.value;
    });

    function setStatus(msg) {
      statusDiv.textContent = msg;
    }

    function moveServo() {
      const angle = angleSlider.value;
      fetch('/set?angle=' + angle)
        .then(r => r.text())
        .then(t => setStatus(t))
        .catch(err => setStatus('Error: ' + err));
    }

    function saveStep() {
      const angle = angleSlider.value;
      fetch('/save?angle=' + angle)
        .then(r => r.text())
        .then(t => {
          setStatus(t);
          // server response includes "Steps: X"
          const parts = t.split('Steps: ');
          if (parts.length > 1) {
            const num = parts[1].trim();
            stepCountSpan.textContent = num;
          }
        })
        .catch(err => setStatus('Error: ' + err));
    }

    function playSeq() {
      setStatus('Playing sequence...');
      fetch('/play')
        .then(r => r.text())
        .then(t => setStatus(t))
        .catch(err => setStatus('Error: ' + err));
    }

    function clearSeq() {
      fetch('/clear')
        .then(r => r.text())
        .then(t => {
          setStatus(t);
          stepCountSpan.textContent = '0';
        })
        .catch(err => setStatus('Error: ' + err));
    }
  </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

// Move servo to angle
void handleSet() {
  if (!server.hasArg("angle")) {
    server.send(400, "text/plain", "Missing angle");
    return;
  }
  int angle = server.arg("angle").toInt();
  moveServoTo(angle);
  server.send(200, "text/plain", "Moved to " + String(angle) + " degrees");
}

// Save current angle into sequence
void handleSave() {
  if (!server.hasArg("angle")) {
    server.send(400, "text/plain", "Missing angle");
    return;
  }
  if (stepCount >= MAX_STEPS) {
    server.send(200, "text/plain", "Sequence full! Max steps = " + String(MAX_STEPS));
    return;
  }

  int angle = server.arg("angle").toInt();
  angle = constrain(angle, 0, 180);
  sequenceAngles[stepCount] = angle;
  stepCount++;

  String msg = "Saved angle ";
  msg += angle;
  msg += " at step ";
  msg += stepCount;
  msg += " / ";
  msg += MAX_STEPS;

  Serial.println(msg);
  server.send(200, "text/plain", msg + " | Steps: " + String(stepCount));
}

// Play the saved sequence
void handlePlay() {
  if (stepCount == 0) {
    server.send(200, "text/plain", "No steps saved!");
    return;
  }

  Serial.println("Playing sequence...");
  for (int i = 0; i < stepCount; i++) {
    int angle = sequenceAngles[i];
    Serial.print("Step ");
    Serial.print(i + 1);
    Serial.print(" -> ");
    Serial.println(angle);
    moveServoTo(angle);
    delay(700); // delay between steps (ms) – adjust
  }

  server.send(200, "text/plain", "Done playing " + String(stepCount) + " steps.");
}

// Clear sequence
void handleClear() {
  stepCount = 0;
  server.send(200, "text/plain", "Sequence cleared.");
  Serial.println("Sequence cleared.");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Servo setup
  myServo.setPeriodHertz(50); // 50 Hz
  myServo.attach(SERVO_PIN, SERVO_MIN_US, SERVO_MAX_US);
  moveServoTo(90); // initial center position

  // WiFi AP Mode
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Routes
  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.on("/save", handleSave);
  server.on("/play", handlePlay);
  server.on("/clear", handleClear);
  server.onNotFound([]() {
    server.send(404, "text/plain", "Not found");
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
