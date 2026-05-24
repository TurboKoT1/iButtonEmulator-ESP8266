#include <LittleFS.h>
#include <Arduino.h>
#include "config.h"
#include "storage.h"

OneWireKey userKeys[MAX_USER_KEYS];
uint8_t userKeyCount = 0;

static bool writeAllKeys() {
    File f = LittleFS.open(KEYS_FILE, "w");
    if (!f) return false;
    f.write((uint8_t*)&userKeyCount, sizeof(userKeyCount));
    for (uint8_t i = 0; i < userKeyCount; i++) {
        f.write((uint8_t*)&userKeys[i], sizeof(OneWireKey));
    }
    f.close();
    return true;
}

void initStorage() {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS init failed");
        return;
    }
    Serial.println("LittleFS mounted");
    loadUserKeys();
}

bool loadUserKeys() {
    userKeyCount = 0;
    if (!LittleFS.exists(KEYS_FILE)) return true;

    File f = LittleFS.open(KEYS_FILE, "r");
    if (!f) return false;

    f.read((uint8_t*)&userKeyCount, sizeof(userKeyCount));
    if (userKeyCount > MAX_USER_KEYS) userKeyCount = 0;

    for (uint8_t i = 0; i < userKeyCount; i++) {
        f.read((uint8_t*)&userKeys[i], sizeof(OneWireKey));
    }
    f.close();

    Serial.printf("Loaded %d user keys\n", userKeyCount);
    return true;
}

bool saveUserKey(const OneWireKey& key) {
    if (userKeyCount >= MAX_USER_KEYS) return false;

    for (uint8_t i = 0; i < userKeyCount; i++) {
        if (memcmp(userKeys[i].data, key.data, 8) == 0) return false;
    }
    for (uint8_t i = 0; i < FACTORY_KEY_COUNT; i++) {
        if (memcmp(factoryKeys[i].data, key.data, 8) == 0) return false;
    }

    userKeys[userKeyCount++] = key;
    return writeAllKeys();
}

bool deleteUserKey(uint8_t index) {
    if (index >= userKeyCount) return false;
    for (uint8_t i = index; i < userKeyCount - 1; i++) {
        userKeys[i] = userKeys[i + 1];
    }
    userKeyCount--;
    return writeAllKeys();
}