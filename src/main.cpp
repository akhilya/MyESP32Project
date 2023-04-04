#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "pin_config.h"

#include "OneButton.h"

#define DISABLE_ALL_LIBRARY_WARNINGS

TFT_eSPI tft = TFT_eSPI();

uint32_t background = TFT_RED;

void setup() 
{
    tft.init();
	tft.fillScreen(background);
	tft.setRotation(2);

	tft.setTextSize(5);
	tft.setTextColor(TFT_WHITE, background, true);
	tft.setTextWrap(false);
}

void loop()
{
    static int counter = 0;

	tft.setCursor(0, 0);
	tft.print("Hello world?\n" + String(millis() / 10.0));
	delay(10);
}