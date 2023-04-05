#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "pin_config.h"
#include "OneButton.h"
#include "ClientManager.h"

#define DISABLE_ALL_LIBRARY_WARNINGS

TFT_eSPI tft = TFT_eSPI();

uint32_t background = TFT_BLACK;
ClientManager cm;

void setup() 
{
    tft.init();
	tft.fillScreen(background);
	tft.setRotation(1);

	tft.setTextSize(3);
	tft.setTextColor(TFT_WHITE, background, true);
	tft.setTextWrap(true);

	cm.setupConnection();
}

void loop()
{
	tft.setCursor(0, 0);
	tft.print(String(cm.receivedData.text) + "\n" + String(millis() / 1000.0));
	delay(10);
}