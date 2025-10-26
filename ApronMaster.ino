#include <Adafruit_NeoPixel.h>
#include <esp_now.h>
#include <WiFi.h>

// LED Strip settings
#define LED_PIN     1       // Fixed pin for Master
#define NUM_LEDS    14      // Fixed number of LEDs for Master
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Pressure sensor settings
#define PRESSURE_PIN 2      // Fixed pressure sensor pin for Master
#define PRESSURE_THRESHOLD 2000
#define WORN_DURATION 3600000  // 1 hour

// Variables
int apprenticeGreenLEDs = 0;
bool beingWorn = false;
unsigned long wornCheckTime = 0;

// Structure to receive data
typedef struct struct_message {
  int greenLEDCount;
} struct_message;

struct_message receivedData;

// Function declarations
void updateLEDs();
void celebrationShow();

// Callback when data is received
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  int previousCount = apprenticeGreenLEDs;
  apprenticeGreenLEDs = receivedData.greenLEDCount;
  
  Serial.print("Received from Apprentice: ");
  Serial.print(apprenticeGreenLEDs);
  Serial.println(" green LEDs");
  
  // Check if we just completed the cycle (went from max to 0)
  if (previousCount == 14 && apprenticeGreenLEDs == 0) {
    celebrationShow();
  }
  
  updateLEDs();
}

void setup() {
  Serial.begin(115200);
  
  // Initialize LED strip
  strip.begin();
  strip.setBrightness(30);  // Lower brightness for 14 LEDs
  strip.clear();
  strip.show();
  
  // Initialize WiFi in Station mode
  WiFi.mode(WIFI_STA);
  
  Serial.print("Master MAC Address: ");
  Serial.println(WiFi.macAddress());
  
  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }
  
  // Register receive callback
  esp_now_register_recv_cb(OnDataRecv);
  
  Serial.println("ApronMaster Ready");
  Serial.println("Waiting for Apprentice data...");
}

void loop() {
  unsigned long currentTime = millis();
  
  // Check if Master apron is being worn
  if (currentTime - wornCheckTime > WORN_DURATION) {
    int pressureValue = analogRead(PRESSURE_PIN);
    
    if (pressureValue > PRESSURE_THRESHOLD) {
      beingWorn = true;
      Serial.println("Master apron is being worn");
    } else {
      beingWorn = false;
      Serial.println("Master apron not being worn");
    }
    
    wornCheckTime = currentTime;
  }
  
  delay(100);
}

void updateLEDs() {
  strip.clear();
  
  // Mirror the Apprentice's LED pattern
  for (int i = 0; i < apprenticeGreenLEDs && i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 255, 0));  // Green
  }
  
  for (int i = apprenticeGreenLEDs; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));  // Red
  }
  
  strip.show();
}

void celebrationShow() {
  // Blink all green LEDs 3 times
  for (int blink = 0; blink < 3; blink++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.Color(0, 255, 0));  // Green
    }
    strip.show();
    delay(300);
    
    strip.clear();
    strip.show();
    delay(300);
  }
  
  // Running light effect - up and down twice
  for (int run = 0; run < 2; run++) {
    // Run up
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.clear();
      strip.setPixelColor(i, strip.Color(0, 255, 0));
      strip.show();
      delay(100);
    }
    
    // Run down
    for (int i = NUM_LEDS - 1; i >= 0; i--) {
      strip.clear();
      strip.setPixelColor(i, strip.Color(0, 255, 0));
      strip.show();
      delay(100);
    }
  }
  
  // Clear all
  strip.clear();
  strip.show();
  delay(500);
}