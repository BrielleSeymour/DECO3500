# DECO3500
DECO3500 Social &amp; Mobile Computing Team Project

## Setup and Requirements

### Hardware Requirements

**Apprentice Apron:**
* 1x Waveshare ESP32*S3*Zero development board
* 1x WS2812B LED strip (14 LEDs)
* 1x RP*C7.6*LT Thin Film Pressure Sensor
* 1x 10kΩ resistor
* 1x Smartphone with camera
* 1x Apron with chest*mounted phone holder
* 1x Laptop/screen with speakers (for video display)
* Breadboard and jumper wires
* USB cable for power and programming

**Master Apron:**
* 1x Waveshare ESP32*S3*Zero development board
* 1x WS2812B LED strip (14 LEDs)
* 1x Smartphone with camera
* 1x Apron with chest*mounted phone holder
* 1x Laptop/screen with speakers (for video display)
* Breadboard and jumper wires
* USB cable for power and programming

### Software Requirements

* Arduino IDE (with ESP32 board support installed)
* Adafruit NeoPixel library
* Preferences library (included with ESP32 core)
* ESP*NOW library (included with ESP32 core)
* FaceTime or similar video calling app

### Wiring Configuration

**Apprentice Apron:**
* LED Strip Data → GPIO 1
* LED Strip 5V → 5V power
* LED Strip GND → GND
* Pressure Sensor terminal 1 → GPIO 2
* Pressure Sensor terminal 2 → 3.3V
* 10kΩ Resistor → Between GPIO 2 and GND

**Master Apron:**
* LED Strip Data → GPIO 1
* LED Strip 5V → 5V power
* LED Strip GND → GND

### Initial Setup

1. Install Arduino IDE and add ESP32 board support
2. Install Adafruit NeoPixel library via Library Manager
3. Update MAC addresses in code files:
   * Get each ESP32's MAC address by running the MAC detection sketch
   * Update `masterAddress` in ApronApprentice code
4. Upload ApronApprentice code to apprentice's ESP32
5. Upload ApronMaster code to master's ESP32
6. Mount phones in apron holders at chest level
7. Connect laptops/screens to same WiFi network as phones
8. Start FaceTime call between the two phones
