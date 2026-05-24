#include <Arduino.h>
#include <interface/display/Display.h>
#include <interface/button/Button.h>
#include <interface/serial/SerialComm.h>
#include <onewire/Emulator.h>
#include <onewire/Reader.h>
#include <storage/Storage.h>
#include "config.h"

enum class AppState { CAROUSEL, EMULATING, READING };

static AppState appState = AppState::CAROUSEL;
static uint8_t currentKeyIndex = 0;
static uint8_t selectedKeyIndex = 0;

uint8_t totalKeyCount() {
    return userKeyCount + FACTORY_KEY_COUNT;
}

const OneWireKey& getKey(uint8_t index) {
    if (index < userKeyCount) return userKeys[index];
    return factoryKeys[index - userKeyCount];
}

void showCarousel() {
    uint8_t total = totalKeyCount();
    uint8_t prev  = (currentKeyIndex - 1 + total) % total;
    uint8_t next  = (currentKeyIndex + 1) % total;
    displayCarousel(
        getKey(prev).name,
        getKey(currentKeyIndex).name,
        getKey(next).name,
        currentKeyIndex, total
    );
}

void setup() {
    Serial.begin(115200);
    pinMode(buttonPin, INPUT_PULLUP);
    initDisplay();
    initStorage();
    initSerial();
    showCarousel();
}

void loop() {
    pollSerial();

    if (appState == AppState::EMULATING) {
        pollEmulator();
    }

    ButtonGesture gesture = detectButtonGesture();

    if (appState == AppState::CAROUSEL) {
        if (gesture == ButtonGesture::SingleClick) {
            currentKeyIndex = (currentKeyIndex + 1) % totalKeyCount();
            showCarousel();
        }
        else if (gesture == ButtonGesture::DoubleClick) {
            selectedKeyIndex = currentKeyIndex;
            appState = AppState::EMULATING;
            emulateKey(getKey(selectedKeyIndex));
            displayEmulating(getKey(selectedKeyIndex).name);
        }
        else if (gesture == ButtonGesture::Hold) {
            appState = AppState::READING;
            displayReading();
        }
    }
    else if (appState == AppState::EMULATING) {
        if (gesture == ButtonGesture::DoubleClick) {
            stopEmulation();
            appState = AppState::CAROUSEL;
            showCarousel();
        }
    }
    else if (appState == AppState::READING) {
        OneWireKey newKey;
        if (readKey(newKey)) {
            snprintf(newKey.name, sizeof(newKey.name), "My Key %d", userKeyCount + 1);
            if (saveUserKey(newKey)) {
                displayMessage("Saved!", newKey.name);
            } else {
                displayMessage("Duplicate!", "Already exists");
            }
            delay(1500);
            currentKeyIndex = 0;
            appState = AppState::CAROUSEL;
            showCarousel();
        }
        if (gesture == ButtonGesture::DoubleClick) {
            appState = AppState::CAROUSEL;
            showCarousel();
        }
    }
}

// callbacks for SerialComm
void onSerialEmulate(const OneWireKey& key) {
    appState = AppState::EMULATING;
    emulateKey(key);
    displayEmulating(key.name);
}
void onSerialStopEmulate() {
    stopEmulation();
    appState = AppState::CAROUSEL;
    showCarousel();
}
void onSerialStartRead() {
    appState = AppState::READING;
    displayReading();
}
void onSerialStopRead() {
    appState = AppState::CAROUSEL;
    showCarousel();
}