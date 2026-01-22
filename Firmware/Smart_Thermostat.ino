#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <ESP32Servo.h>

// ================= WIFI =================
const char* ssid     = "Izwaniphone";
const char* password = "takingat123";

// ================= NODE-RED =================
const char* serverUrl = "http://172.20.10.6:1880/esp32/data";

// ================= PINS =================
#define DHTPIN 4
#define DHTTYPE DHT11

// Relay (Active LOW)
#define RELAY_ON  LOW
#define RELAY_OFF HIGH
#define HEATER_PIN 26

// Fan Motor Driver
#define FAN_INA 27
#define FAN_INB 25

// Servo
#define SERVO_PIN 14

// ================= PWM SETTINGS (NEW V3.0 SYNTAX) =================
// In v3.0+, we don't need to define channels manually.
const int fanPwmFreq = 5000;
const int fanPwmResolution = 8; // 8-bit resolution (0-255)

// Speed Settings (0 - 255)
#define MAX_FAN_SPEED 255
#define ECO_FAN_SPEED 100 

// ================= OBJECTS =================
DHT dht(DHTPIN, DHTTYPE);
Servo ventServo;

// ================= SETTINGS =================
float TEMP_THRESHOLD = 25.0;
bool ecoMode = false;
bool manualTempMode = false;
float manualTemp = 0.0;

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  // Initialize Pins
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(FAN_INA, OUTPUT);
  pinMode(FAN_INB, OUTPUT);

  // Default States
  digitalWrite(HEATER_PIN, RELAY_OFF); 
  digitalWrite(FAN_INB, LOW); 

  // --- NEW PWM SETUP (Fix for Error) ---
  // Replaces ledcSetup and ledcAttachPin
  ledcAttach(FAN_INA, fanPwmFreq, fanPwmResolution);
  ledcWrite(FAN_INA, 0); // Start Fan OFF

  // --- SETUP SERVO ---
  ventServo.setPeriodHertz(50); 
  ventServo.attach(SERVO_PIN, 500, 2400);
  ventServo.write(90); // Start Closed

  dht.begin();

  // WiFi Connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// ================= LOOP =================
void loop() {
  
  // 1. SERIAL LISTENER
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.startsWith("T=")) {
      manualTemp = cmd.substring(2).toFloat();
      manualTempMode = true;
    }
    if (cmd.equalsIgnoreCase("AUTO")) {
      manualTempMode = false;
    }
  }

  // 2. READ SENSORS
  float dhtTemp = dht.readTemperature();
  float hum = dht.readHumidity();
  float activeTemp = manualTempMode ? manualTemp : dhtTemp;

  if (isnan(activeTemp) || isnan(hum)) {
    Serial.println("DHT Read Failed!");
    activeTemp = 0.0; hum = 0.0;
  }

  // 3. NODE-RED COMM
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String payload = "{\"temperature\":" + String(activeTemp) + 
                     ",\"humidity\":" + String(hum) + 
                     ",\"ecoMode\":" + String(ecoMode ? "true" : "false") + 
                     ",\"manualMode\":" + String(manualTempMode ? "true" : "false") + "}";

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(payload);
    
    if (httpCode == 200) {
      String response = http.getString();
      Serial.println("Node-RED Response: " + response);
      if (response.indexOf("ECO_ON") >= 0) ecoMode = true;
      else if (response.indexOf("ECO_OFF") >= 0) ecoMode = false;
    } else {
      Serial.print("HTTP Error: "); Serial.println(httpCode);
    }
    http.end();
  }

  // 4. LOGIC
  Serial.println("\n--- STATUS ---");
  Serial.print("Temp: "); Serial.println(activeTemp);
  
  if (ecoMode) {
    Serial.println("MODE: ECO (Low Fan + Open Vent)");
    digitalWrite(HEATER_PIN, RELAY_OFF);
    digitalWrite(FAN_INB, LOW);
    
    // NEW SYNTAX: Write to PIN, not Channel
    ledcWrite(FAN_INA, ECO_FAN_SPEED); 
    
    ventServo.write(45);

  } else {
    Serial.println("MODE: BASIC");
    if (activeTemp > TEMP_THRESHOLD) {
      Serial.println("State: HOT -> Max Cooling");
      digitalWrite(HEATER_PIN, RELAY_OFF);
      digitalWrite(FAN_INB, LOW);
      
      // NEW SYNTAX
      ledcWrite(FAN_INA, MAX_FAN_SPEED); 
      
      ventServo.write(45);
    } 
    else {
      Serial.println("State: COLD -> Heating Up");
      digitalWrite(HEATER_PIN, RELAY_ON);
      digitalWrite(FAN_INB, LOW);
      
      // NEW SYNTAX
      ledcWrite(FAN_INA, 0); 
      
      ventServo.write(90);
    }
  }

  Serial.println("--------------");
  delay(5000); 
}
