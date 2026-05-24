#include <Arduino.h>
#include "SerialComm.h"
#include "config.h"
#include "onewire/Key.h"
#include "onewire/Emulator.h"
#include "onewire/Reader.h"
#include "storage/Storage.h"

// callbacks to main for changing AppState
extern void onSerialEmulate(const OneWireKey& key);
extern void onSerialStopEmulate();
extern void onSerialStartRead();
extern void onSerialStopRead();

static String inputBuffer = "";

static void sendOK() { Serial.println("OK"); }
static void sendError(const char* msg) { Serial.print("ERR:"); Serial.println(msg); }

static void handleCommand(const String& cmd) {
    if (cmd == "STOPEMULATE") {
        onSerialStopEmulate();
        sendOK();
    }
    else if (cmd == "STOPREAD") {
        onSerialStopRead();
        sendOK();
    }
    else if (cmd == "READ") {
        onSerialStartRead();
        sendOK();
    }
    else if (cmd == "LIST") {
        Serial.printf("COUNT:%d\n", userKeyCount + FACTORY_KEY_COUNT);
        for (uint8_t i = 0; i < userKeyCount; i++) {
            Serial.printf("KEY:%d:USER:", i);
            for (uint8_t b = 0; b < 8; b++) {
                Serial.printf("%02X", userKeys[i].data[b]);
                if (b < 7) Serial.print(":");
            }
            Serial.printf(":%s\n", userKeys[i].name);
        }
        for (uint8_t i = 0; i < FACTORY_KEY_COUNT; i++) {
            Serial.printf("KEY:%d:FACTORY:", userKeyCount + i);
            for (uint8_t b = 0; b < 8; b++) {
                Serial.printf("%02X", factoryKeys[i].data[b]);
                if (b < 7) Serial.print(":");
            }
            Serial.printf(":%s\n", factoryKeys[i].name);
        }
    }
    else if (cmd.startsWith("EMULATE:")) {
        // EMULATE:01:FF:A3:7C:02:B1:00:2F
        OneWireKey key;
        memset(&key, 0, sizeof(key));
        String payload = cmd.substring(8);
        uint8_t byteIndex = 0;
        int start = 0;
        while (byteIndex < 8) {
            int sep = payload.indexOf(':', start);
            String byteStr = (sep == -1) ? payload.substring(start) : payload.substring(start, sep);
            key.data[byteIndex++] = (uint8_t)strtoul(byteStr.c_str(), nullptr, 16);
            if (sep == -1) break;
            start = sep + 1;
        }
        if (byteIndex == 8) {
            snprintf(key.name, sizeof(key.name), "UART Key");
            onSerialEmulate(key);
            sendOK();
        } else {
            sendError("bad key format");
        }
    }
    else if (cmd.startsWith("SAVE:")) {
        // SAVE:01:FF:A3:7C:02:B1:00:2F:MyKeyName
        OneWireKey key;
        memset(&key, 0, sizeof(key));
        String payload = cmd.substring(5);
        uint8_t byteIndex = 0;
        int start = 0;
        while (byteIndex < 8) {
            int sep = payload.indexOf(':', start);
            if (sep == -1) { sendError("bad key format"); return; }
            String byteStr = payload.substring(start, sep);
            key.data[byteIndex++] = (uint8_t)strtoul(byteStr.c_str(), nullptr, 16);
            start = sep + 1;
        }
        String name = payload.substring(start);
        name.trim();
        if (name.length() == 0) name = "UART Key";
        snprintf(key.name, sizeof(key.name), "%s", name.c_str());

        if (saveUserKey(key)) {
            sendOK();
        } else {
            sendError("duplicate or full");
        }
    }
    else {
        sendError("unknown command");
    }
}

void initSerial() {
    // Serial.begin is already called in setup func
}

void pollSerial() {
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
            inputBuffer.trim();
            if (inputBuffer.length() > 0) {
                handleCommand(inputBuffer);
                inputBuffer = "";
            }
        } else {
            inputBuffer += c;
        }
    }
}