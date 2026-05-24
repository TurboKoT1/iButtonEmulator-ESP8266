#pragma once
#include "onewire/Key.h"

constexpr uint8_t MAX_USER_KEYS = 20;

extern OneWireKey userKeys[MAX_USER_KEYS];
extern uint8_t userKeyCount;

void initStorage();
bool loadUserKeys();
bool saveUserKey(const OneWireKey& key);
bool deleteUserKey(uint8_t index);