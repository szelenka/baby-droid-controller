#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// LED Pin Definitions (Seeed Studio XIAO ESP32C6)
#define LED_POWER_PIN       D1   // GPIO22 (D1) - Power indicator LED (blinks)
#define LED_BUTTON_PIN      D2   // GPIO23 (D2) - Button press indicator LED

// Button Pin Definitions (Seeed Studio XIAO ESP32C6)
// Bit order matches chassis BUTTON_x_BIT definitions
#define BUTTON_1_PIN        D4   // GPIO1  (D4)  - Spin dome left
#define BUTTON_2_PIN        D3   // GPIO15 (D3)  - Spin dome right
#define BUTTON_3_PIN        D5   // GPIO2  (D5)  - Turn right
#define BUTTON_4_PIN        D6   // GPIO21 (D6)  - Turn left
#define BUTTON_5_PIN        D7   // GPIO17 (D7)  - Play sound
#define BUTTON_6_PIN        D9   // GPIO20 (D9)  - Drive backwards
#define BUTTON_7_PIN        D10  // GPIO18 (D10) - Drive forwards
#define BUTTON_8_PIN        D8   // GPIO19 (D8)  - Auxiliary

#define NUM_BUTTONS         8

// Timing Constants
#define POWER_LED_BLINK_INTERVAL  1000  // 1 second blink interval
#define DEBOUNCE_DELAY            50    // 50ms debounce time

// ESP-NOW Configuration
// Replace with your target ESP32's MAC address
// Format: {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}
#define TARGET_MAC_ADDRESS {0xF4, 0x65, 0x0B, 0x33, 0x44, 0x8C}
// WiFi Channel (must match between both ESP32s)
#define WIFI_CHANNEL        1

#endif // CONFIG_H
