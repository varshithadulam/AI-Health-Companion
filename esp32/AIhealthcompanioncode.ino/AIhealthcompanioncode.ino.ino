#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "MAX30100_PulseOximeter.h"

/* ---------- WiFi & Firebase ---------- */
#define WIFI_SSID     "Varshitha"
#define WIFI_PASSWORD "varshithagoud"

#define DATABASE_HOST "health-ccfd6-default-rtdb.firebaseio.com"
#define DATABASE_PATH "/health_data.json"

/* ---------- Objects ---------- */
Adafruit_BMP085 bmp;
PulseOximeter pox;

/* ---------- Timing ---------- */
#define HR_INTERVAL_MS        1000
#define FIREBASE_INTERVAL_MS  5000

uint32_t lastHRRead = 0;
uint32_t lastFirebaseSend = 0;

/* ---------- Sensor values ---------- */
float heartRate = 0;
float spo2 = 0;

void setup() {
  Serial.begin(115200);
  delay(3000);

  Serial.println("ESP32 starting...");

  /* ---------- I2C ---------- */
  Wire.begin(21, 22);

  /* ---------- MAX30100 ---------- */
  Serial.println("Initializing MAX30100...");
  if (!pox.begin()) {
    Serial.println("ERROR: MAX30100 not detected");
    while (1);
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  Serial.println("MAX30100 ready");

  /* ---------- BMP180 ---------- */
  if (!bmp.begin()) {
    Serial.println("ERROR: BMP180 not detected");
    while (1);
  }
  Serial.println("BMP180 ready");

  /* ---------- WiFi ---------- */
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());
}

void loop() {
  /* ---------- MAX30100 MUST RUN CONTINUOUSLY ---------- */
  pox.update();

  /* ---------- Read HR & SpO2 ---------- */
  if (millis() - lastHRRead >= HR_INTERVAL_MS) {
    lastHRRead = millis();
    heartRate = pox.getHeartRate();
    spo2 = pox.getSpO2();

    Serial.print("HR: ");
    Serial.print(heartRate);
    Serial.print(" bpm | SpO2: ");
    Serial.print(spo2);
    Serial.println(" %");
  }

  /* ---------- Send to Firebase ---------- */
  if (millis() - lastFirebaseSend >= FIREBASE_INTERVAL_MS) {
    lastFirebaseSend = millis();

    float temperature = bmp.readTemperature();
    int32_t pressure = bmp.readPressure();
    float altitude = bmp.readAltitude(101325);
    long timestamp = millis();

    String json =
      "{"
      "\"heart_rate\":" + String(heartRate) + ","
      "\"spo2\":" + String(spo2) + ","
      "\"temperature\":" + String(temperature) + ","
      "\"pressure\":" + String(pressure) + ","
      "\"altitude\":" + String(altitude) + ","
      "\"timestamp\":" + String(timestamp) +
      "}";

    Serial.println("Sending to Firebase:");
    Serial.println(json);

    WiFiClientSecure client;
    client.setInsecure();   // hotspot-safe TLS bypass

    HTTPClient https;
    String url = String("https://") + DATABASE_HOST + DATABASE_PATH;

    if (https.begin(client, url)) {
      https.addHeader("Content-Type", "application/json");
      int httpCode = https.POST(json);

      if (httpCode > 0) {
        Serial.print("Firebase POST success: ");
        Serial.println(httpCode);
      } else {
        Serial.print("Firebase POST error: ");
        Serial.println(https.errorToString(httpCode));
      }

      https.end();
    }
  }
}