#pragma once
#include <cstdint>

struct OneWireKey {
    uint8_t data[8];
    char name[32];
};