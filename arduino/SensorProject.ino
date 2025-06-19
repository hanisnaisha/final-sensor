#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "UUMWiFi_Guest";
const char* password = "";

// Your PHP endpoint URL
const char* serverURL = "https://humancc.site/hanisnaisha/finalproject/backend/sensor_receiver.php";

// ESP32 Pin Definitions
#define DHT_PIN 4
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

#define FLAME_SENSOR_PIN 16
#define PIR_SENSOR_PIN 17
#define RELAY_PIN 25

void setup() {
  // Start serial communication
  Serial.begin(115200);
  delay(2000);  // Wait for Serial Monitor
  Serial.println("\nStarting sensor system...");

  // Connect to WiFi FIRST!
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // THEN send the test HTTP request (optional — you can remove this block later)
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "temperature=23&humidity=60&flame=0&motion=1&relay=1";
    int httpResponseCode = http.POST(postData);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String response = http.getString();
    Serial.println(response);

    http.end();
  }

  // Initialize I2C
  Wire.begin();

  // Initialize display with retry
  int displayInitAttempts = 0;
  while (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C) && displayInitAttempts < 3) {
    Serial.println("SSD1306 allocation failed, retrying...");
    delay(1000);
    displayInitAttempts++;
  }

  if (displayInitAttempts >= 3) {
    Serial.println("Failed to initialize display after 3 attempts");
  } else {
    Serial.println("Display initialized successfully");
  }

  // Initialize DHT sensor
  dht.begin();
  Serial.println("DHT sensor initialized");

  // Configure sensor pins
  pinMode(FLAME_SENSOR_PIN, INPUT_PULLUP);
  pinMode(PIR_SENSOR_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Clear the display
  display.clearDisplay();
  Serial.println("Setup complete!");
}

void loop() {
  // Read sensors
  int flameRaw = digitalRead(FLAME_SENSOR_PIN);
  int pirRaw = digitalRead(PIR_SENSOR_PIN);


  bool flameDetected = flameRaw == LOW; 
  bool motionDetected = pirRaw == HIGH;

  Serial.print("Flame raw value: ");
  Serial.println(flameRaw); // Should be 0 when flame is present
  Serial.print("Flame detected? ");
  Serial.println(flameDetected ? "YES" : "NO");

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    temp = 0;
    hum = 0;
  }

  bool relayState = flameDetected;
  digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);

  // Display on OLED (optional, not present in your loop yet)
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.printf("Temp: %.1fC\nHum: %.1f%%\nFlame: %s\nMotion: %s\nRelay: %s",
                 temp, hum,
                 flameDetected ? "YES" : "NO",
                 motionDetected ? "YES" : "NO",
                 relayState ? "ON" : "OFF");
  display.display();

  // Send to PHP server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "temperature=" + String(temp) + "&humidity=" + String(hum) + "&flame=" + String(flameDetected ? "1" : "0") + "&motion=" + String(motionDetected ? "1" : "0") + "&relay=" + String(relayState ? "1" : "0");

    int httpResponseCode = http.POST(postData);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(5000);  // Wait before sending next data
}