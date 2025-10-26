#include <Adafruit_NeoPixel.h>
#include <Preferences.h>
#include <esp_now.h>
#include <WiFi.h>

// LED Strip settings
#define LED_PIN     1       // Fixed pin for Apprentice
#define NUM_LEDS    14      // Fixed number of LEDs for Apprentice
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Pressure sensor settings
#define PRESSURE_PIN 2      // Fixed pressure sensor pin for Apprentice
#define PRESSURE_THRESHOLD 2
#define COOLDOWN_TIME 2000  // 1 hour

// Master's MAC Address
uint8_t masterAddress[] = {0xE4, 0xB3, 0x23, 0xF7, 0xBD, 0xF4};

// Preferences for persistent storage
Preferences preferences;

// Variables
int greenLEDs = 0;
bool wasWorn = false;
unsigned long lastTriggerTime = 0;

// Structure to send data
typedef struct struct_message {
  int greenLEDCount;
} struct_message;

struct_message myData;

// Function declarations
void updateLEDs();
void sendStatus();
void celebrationShow();

// Callback when data is sent
void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  
  // Initialize preferences
  preferences.begin("apron", false);
  greenLEDs = preferences.getInt("greenLEDs", 0);
  
  // Initialize LED strip
  strip.begin();
  strip.setBrightness(80);
  updateLEDs();
  
  // Initialize WiFi in Station mode
  WiFi.mode(WIFI_STA);
  
  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }
  
  // Register send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer (Master)
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, masterAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  
  Serial.println("ApronApprentice Ready");
  Serial.print("Current progress: ");
  Serial.print(greenLEDs);
  Serial.print("/");
  Serial.println(NUM_LEDS);
  
  // Send initial status
  sendStatus();
}

void loop() {
  int pressureValue = analogRead(PRESSURE_PIN);
  Serial.println(pressureValue);
  bool isWorn = (pressureValue > PRESSURE_THRESHOLD);
  unsigned long currentTime = millis();
  
  // Detect when apron is put on
  if (isWorn && !wasWorn) {
    if (currentTime - lastTriggerTime > COOLDOWN_TIME) {
      greenLEDs++;
      
      // Check if cycle complete
      if (greenLEDs > NUM_LEDS) {
        celebrationShow();  // Play light show
        greenLEDs = 0;
        Serial.println("CYCLE COMPLETE! Resetting.");
      }
      
      // Save progress
      preferences.putInt("greenLEDs", greenLEDs);
      
      updateLEDs();
      sendStatus();  // Send updated status to Master
      lastTriggerTime = currentTime;
      
      Serial.print("Apron worn! Progress: ");
      Serial.print(greenLEDs);
      Serial.print("/");
      Serial.println(NUM_LEDS);
    }
  }
  
  wasWorn = isWorn;
  delay(100);
}

void updateLEDs() {
  strip.clear();
  
  for (int i = 0; i < greenLEDs; i++) {
    strip.setPixelColor(i, strip.Color(0, 255, 0));  // Green
  }
  
  for (int i = greenLEDs; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));  // Red
  }
  
  strip.show();
}

void sendStatus() {
  myData.greenLEDCount = greenLEDs;
  esp_err_t result = esp_now_send(masterAddress, (uint8_t *) &myData, sizeof(myData));
  
  if (result == ESP_OK) {
    Serial.println("Status sent to Master");
  } else {
    Serial.println("Error sending status");
  }
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