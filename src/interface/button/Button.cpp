#include <Arduino.h>
#include "Button.h"
#include "config.h"

static unsigned long lastButtonChange = 0;
static unsigned long buttonPressStart = 0;
static unsigned long lastReleaseTime = 0;

static bool buttonPressed = false;
static bool clickPending = false;
static bool holdHandled = false;

ButtonGesture detectButtonGesture() {
    bool reading = (digitalRead(buttonPin) == LOW);
    unsigned long currentMillis = millis();

    if (currentMillis - lastButtonChange < debounceDelay) {
        return ButtonGesture::NoGesture;
    }

    if (reading && !buttonPressed) {
        buttonPressed = true;
        holdHandled = false;
        buttonPressStart = currentMillis;
        lastButtonChange = currentMillis;
    }

    if (buttonPressed && !holdHandled &&
        currentMillis - buttonPressStart >= holdThreshold) {
        
        holdHandled = true;
        clickPending = false;

        return ButtonGesture::Hold;
    }

    if (!reading && buttonPressed) {
        buttonPressed = false;
        lastButtonChange = currentMillis;

        if (holdHandled) {
            return ButtonGesture::NoGesture;
        }

        if (clickPending &&
            currentMillis - lastReleaseTime <= doubleClickThreshold) {
            
            clickPending = false;
            return ButtonGesture::DoubleClick;
        }

        clickPending = true;
        lastReleaseTime = currentMillis;
    }

    if (clickPending &&
        currentMillis - lastReleaseTime >= doubleClickThreshold) {
        
        clickPending = false;
        return ButtonGesture::SingleClick;
    }

    return ButtonGesture::NoGesture;
}