#ifndef SCREEN_H
#define SCREEN_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
/**
 * Initialize the 1.3" ST7789 display over SPI (240×240).
 * Call once in setup().
 */
void screen_init();

/**
 * Display the full dashboard:
 * - Time & date (computed from hours-of-year) at the top
 * - System metrics in the middle
 * - Current speed big and centered
 * - Previous action to the left
 * - Mode below previous action (bigger text)
 */
void screen_displayDashboard(double hourOfYear, double power,  double pvPower,  double soc,  double irrWeek,  double prevAction,  double currSpeed);
void screen_displayDashboard();
void drawRightArrow(int x, int y, int length, uint16_t color);
#endif // SCREEN_H
