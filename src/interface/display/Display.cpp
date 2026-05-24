#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void initDisplay() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println("SSD1306 allocation failed");
        while (1);
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
}

void printAtCenter(const char* text) {
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 0);
    display.println(text);
}

void displayCarousel(const char* prev, const char* cur, const char* next,
                     uint8_t index, uint8_t total) {
    display.clearDisplay();
    int16_t x1, y1;
    uint16_t w, h;

    display.setTextSize(1);
    display.getTextBounds(prev, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 2);
    display.print(prev);

    display.drawFastHLine(0, 13, SCREEN_WIDTH, SSD1306_WHITE);

    uint8_t curSize = (strlen(cur) > 10) ? 1 : 2;
    display.setTextSize(curSize);
    display.getTextBounds(cur, 0, 0, &x1, &y1, &w, &h);
    uint8_t curY = 14 + ((24 - h) / 2);
    display.setCursor((SCREEN_WIDTH - w) / 2, curY);
    display.print(cur);

    display.drawFastHLine(0, 38, SCREEN_WIDTH, SSD1306_WHITE);

    display.setTextSize(1);
    display.getTextBounds(next, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 42);
    display.print(next);

    char buf[8];
    snprintf(buf, sizeof(buf), "%d/%d", index + 1, total);
    display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
    display.setCursor(SCREEN_WIDTH - w - 1, 56);
    display.print(buf);

    display.display();
}

void displayEmulating(const char* name) {
    display.clearDisplay();
    printAtCenter("Emulating:");
    display.setCursor(0, 20);
    display.println(name);
    display.setCursor(0, 56);
    display.print("[2x] Cancel");
    display.display();
}

void displayReading() {
    display.clearDisplay();
    printAtCenter("Reading key..");
    display.setTextSize(1);
    display.setCursor(0, 24);
    display.println("Apply key to");
    display.println("the device...");
    display.setCursor(0, 56);
    display.print("[2x] Cancel");
    display.display();
}

void displayMessage(const char* title, const char* subtitle) {
    display.clearDisplay();
    int16_t x1, y1;
    uint16_t w, h;
    printAtCenter(title);
    display.setTextSize(1);
    display.getTextBounds(subtitle, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 30);
    display.println(subtitle);
    display.display();
}