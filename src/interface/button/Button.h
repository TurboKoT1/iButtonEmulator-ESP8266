#pragma once

#include <Arduino.h>

enum class ButtonGesture {
    NoGesture,
    SingleClick,
    DoubleClick,
    Hold
};

ButtonGesture detectButtonGesture();