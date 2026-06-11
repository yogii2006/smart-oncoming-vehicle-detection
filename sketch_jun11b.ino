#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WiFi Credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// ThingSpeak API Key
String apiKey = "YOUR_THINGSPEAK_API_KEY";

// HC-SR04 Pins
#define TRIG_PIN 5
#define ECHO_PIN 18

// LED Indicator
#define LED_PIN 2

// LCD Address
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Connecting...");

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WiFi Connected");
  delay(2000);
}

void loop()
{
  long duration;
  float distance;

  // Trigger Ultrasonic Sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);

  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.println(distance);

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Dist:");
  lcd.print(distance);
  lcd.print("cm");

  String status;

  if(distance < 200)
  {
    digitalWrite(LED_PIN, HIGH);

    lcd.setCursor(0,1);
    lcd.print("Vehicle Detect");

    status = "Detected";
  }
  else
  {
    digitalWrite(LED_PIN, LOW);

    lcd.setCursor(0,1);
    lcd.print("Road Clear");

    status = "Clear";
  }

  // Telemetry Upload
  if(WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    String url =
    "https://api.thingspeak.com/update?api_key=" +
    apiKey +
    "&field1=" + String(distance) +
    "&field2=" + status;

    http.begin(url);

    int httpCode = http.GET();

    Serial.print("HTTP Code: ");
    Serial.println(httpCode);

    http.end();
  }

  delay(5000);
}