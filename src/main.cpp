#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "pin_config.h"
#include "OneButton.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <string>

using namespace std;

TFT_eSPI tft = TFT_eSPI();

uint32_t background = TFT_BLACK;

HTTPClient http;
String fetchData();
void endHttp()
{
	http.end();	
}

void processResponse(String response, String* message, int* r, int* g, int* b)
{
	sscanf(response.c_str(), "%d %d %d", r, g, b);
	int whitespaces = 3, idx = 0;
	while(whitespaces > 0)
	{
		if (response[idx++] == ' ')
			whitespaces--;
	}
	*message = response.substring(idx, response.length());
}

void setupConnection()
{
  	Serial.begin(9600);

  	WiFi.begin("x-home", "deukecrh");
  	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.println("Connecting to WiFi...");
  	}
  	Serial.println("Connected to WiFi");

  	// Send HTTP GET request
  	http.begin("192.168.49.246", 80, "/data");
	fetchData();
}

String fetchData()
{
	int httpResponseCode = http.GET();

	if (httpResponseCode == HTTP_CODE_OK) {
		return http.getString();
	} else {
		Serial.println("Error in HTTP request");
		return "NULL";
	}
}

void setup() 
{
	tft.init();
	tft.fillScreen(background);
	tft.setRotation(1);

	tft.setTextSize(3);
	
	tft.setTextWrap(true);

	delay(1000);

	setupConnection();
}

// Конвертирует 8-битный компонент цвета в 5-битное значение
inline uint8_t conv8to5(uint8_t x) {
  return (x * 249 + 1014) >> 11;
}

// Конвертирует 8-битный компонент цвета в 6-битное значение
inline uint8_t conv8to6(uint8_t x) {
  return (x * 63 + 127) >> 8;
}

// Конвертирует RGB цвет в формат RGB565
inline uint16_t convRGB(uint8_t r, uint8_t g, uint8_t b) {
  uint16_t r5 = conv8to5(r);
  uint16_t g6 = conv8to6(g);
  uint16_t b5 = conv8to5(b);
  return (r5 << 11) | (g6 << 5) | b5;
}

void loop()
{
	String message = "Nothing";
	int r = 0, g = 0, b = 0;

	String response = fetchData();
	Serial.println(response);
	if (response != "NULL")
		processResponse(response, &message, &r, &g, &b);

	background = convRGB(r, g, b);
	tft.setTextColor(TFT_WHITE, background);
	tft.fillScreen(background);
	tft.setCursor(0, 0);
	tft.print(message + "\n" + String(millis() / 1000.0));	

	delay(1000);
}