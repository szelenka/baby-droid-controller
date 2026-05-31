#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "config.h"

// Button pin array
const uint8_t buttonPins[NUM_BUTTONS] = {
    BUTTON_1_PIN, BUTTON_2_PIN, BUTTON_3_PIN, BUTTON_4_PIN,
    BUTTON_5_PIN, BUTTON_6_PIN, BUTTON_7_PIN, BUTTON_8_PIN
};

// Button state tracking for debouncing
bool buttonStates[NUM_BUTTONS] = {false};
bool lastButtonStates[NUM_BUTTONS] = {false};
unsigned long lastDebounceTime[NUM_BUTTONS] = {0};

// Transmission tracking
unsigned long lastSendTime = 0;
uint8_t lastSentButtonMask = 0;

// Target MAC address
uint8_t targetAddress[] = TARGET_MAC_ADDRESS;

// ESP-NOW peer info
esp_now_peer_info_t peerInfo;

// Power LED blink state
unsigned long lastBlinkTime = 0;
bool powerLedState = false;

// Button send interval
#define BUTTON_SEND_INTERVAL 100  // Send every 100ms while button held

void printMACAddress() {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    Serial.print("ESP32 MAC Address: {0x");
    for (int i = 0; i < 6; i++) {
        Serial.print(mac[i], HEX);
        if (i < 5) Serial.print(", 0x");
    }
    Serial.println("}");
    Serial.println("Use this MAC address in the chassis ALLOWED_CONTROLLER_MAC");
}

void onDataSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
    // Optional: Log send status for debugging
    // Serial.print("Send Status: ");
    // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void initESPNow() {
    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Small delay to allow WiFi to initialize
    delay(100);

    // Print MAC address for setup
    printMACAddress();

    // Set WiFi channel
    esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Register send callback
    esp_now_register_send_cb(onDataSent);

    // Register peer
    memcpy(peerInfo.peer_addr, targetAddress, 6);
    peerInfo.channel = WIFI_CHANNEL;
    peerInfo.encrypt = false;

    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    Serial.println("ESP-NOW initialized successfully");
}

uint8_t getButtonMask() {
    uint8_t mask = 0;
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (buttonStates[i]) {
            mask |= (1 << i);
        }
    }
    return mask;
}

void sendButtonState(uint8_t buttonMask) {
    esp_err_t result = esp_now_send(targetAddress, &buttonMask, 1);

    if (result == ESP_OK) {
        Serial.print("Button mask 0b");
        Serial.print(buttonMask, BIN);
        Serial.println(" sent");
    } else {
        Serial.println("Error sending data");
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32 Baby Droid Controller Starting...");

    // Initialize LED pins
    pinMode(LED_POWER_PIN, OUTPUT);
    pinMode(LED_BUTTON_PIN, OUTPUT);

    // Initialize button pins with internal pull-up resistors
    for (int i = 0; i < NUM_BUTTONS; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }

    // Initialize ESP-NOW
    initESPNow();

    Serial.println("Controller ready!");
}

void loop() {
    unsigned long currentTime = millis();

    // Handle power LED blinking
    if (currentTime - lastBlinkTime >= POWER_LED_BLINK_INTERVAL) {
        lastBlinkTime = currentTime;
        powerLedState = !powerLedState;
        digitalWrite(LED_POWER_PIN, powerLedState ? HIGH : LOW);
    }

    // Check all buttons with debouncing
    bool stateChanged = false;
    for (int i = 0; i < NUM_BUTTONS; i++) {
        // Read button (LOW when pressed due to INPUT_PULLUP)
        bool reading = (digitalRead(buttonPins[i]) == LOW);

        // Check if button state changed
        if (reading != lastButtonStates[i]) {
            lastDebounceTime[i] = currentTime;
        }

        // If enough time has passed, consider it a valid state change
        if ((currentTime - lastDebounceTime[i]) > DEBOUNCE_DELAY) {
            if (reading != buttonStates[i]) {
                buttonStates[i] = reading;
                stateChanged = true;
            }
        }

        lastButtonStates[i] = reading;
    }

    // Get current button mask
    uint8_t currentMask = getButtonMask();

    // Send if state changed or periodic update needed while buttons held
    bool needsPeriodicUpdate = (currentMask != 0) &&
                               (currentTime - lastSendTime >= BUTTON_SEND_INTERVAL);

    if (stateChanged || needsPeriodicUpdate) {
        sendButtonState(currentMask);
        lastSentButtonMask = currentMask;
        lastSendTime = currentTime;
    }

    // Control button LED based on any button being pressed
    digitalWrite(LED_BUTTON_PIN, currentMask != 0 ? HIGH : LOW);
}
