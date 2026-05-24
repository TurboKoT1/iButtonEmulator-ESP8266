#pragma once

void initDisplay();
void printAtCenter(const char* text);
void displayCarousel(const char* prev, const char* cur, const char* next,
                     uint8_t index, uint8_t total);
void displayEmulating(const char* name);
void displayReading();
void displayMessage(const char* title, const char* subtitle);