#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "secrets.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include "RTClib.h"

// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

#define rainSensor 4
#define ldr 5
#define DHTPIN 2       // Pin for DHT sensor
#define DHTTYPE DHT11  // DHT 11 sensor type

DHT dht(DHTPIN, DHTTYPE);
RTC_DS3231 rtc;

void setup() {
  Serial.begin(115200);
  pinMode(rainSensor, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  dht.begin();
  delay(2000);  // Allow DHT to stabilize

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Set to compile time
  }
}

void loop() {
  // Read DHT sensor data
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT sensor");
    delay(1000);
  } else {
    Serial.print("SUHU = ");
    Serial.print(t);
    Serial.println(" *C");
    Serial.print("KELEMBAPAN = ");
    Serial.print(h);
    Serial.println(" %");

    writeTemp(t);
    writeHum(h);
  }

  // Rain sensor logic
  if (digitalRead(rainSensor) == HIGH) {
    Serial.println("It's raining");
    writeRain(true);

    while (digitalRead(rainSensor) == HIGH) {
      delay(100); // Debounce delay
    }

    if (digitalRead(rainSensor) == LOW) {
      Serial.println("It's not raining anymore");
      writeRain(false);
    }
  }

  // Weather Logic using LDR and RTC
  int lightValue = analogRead(ldr);
  DateTime now = rtc.now();

  Serial.print("Cahaya: ");
  Serial.println(lightValue);

  int hour = now.hour();
  bool isDaytime = (hour >= 6 && hour < 18);
  bool isDark = lightValue < 500;
  bool isCool = t < 25;

  if (isDark && isDaytime) {
    Serial.println("Cuaca mendung");
    writeWeather("Mendung");
  } else if (isDark && !isDaytime && isCool) {
    Serial.println("Kemungkinan malam");
    writeWeather("Malam");
  } else {
    Serial.println("Cerah");
    writeWeather("Cerah");
  }

  delay(3000);
}

void writeTemp(float value) {
  if (Firebase.ready() && Firebase.RTDB.setFloat(&fbdo, "MWS/Temp", value)) {
    Serial.print("Written temperature: ");
    Serial.println(value);
  } else {
    Serial.println("Failed to write temperature");
    Serial.print("Reason: ");
    Serial.println(fbdo.errorReason());
  }
}

void writeHum(float value) {
  if (Firebase.ready() && Firebase.RTDB.setFloat(&fbdo, "MWS/Hum", value)) {
    Serial.print("Written humidity: ");
    Serial.println(value);
  } else {
    Serial.println("Failed to write humidity");
    Serial.print("Reason: ");
    Serial.println(fbdo.errorReason());
  }
}

void writeRain(bool value) {
  if (Firebase.ready() && Firebase.RTDB.setBool(&fbdo, "MWS/Rain", value)) {
    Serial.print("Written rain status: ");
    Serial.println(value);
  } else {
    Serial.println("Failed to write rain status");
    Serial.print("Reason: ");
    Serial.println(fbdo.errorReason());
  }
}

void writeWeather(String value) {
  if (Firebase.ready() && Firebase.RTDB.setString(&fbdo, "MWS/Weather", value)) {
    Serial.print("Written weather: ");
    Serial.println(value);
  } else {
    Serial.println("Failed to write weather");
    Serial.print("Reason: ");
    Serial.println(fbdo.errorReason());
  }
}
