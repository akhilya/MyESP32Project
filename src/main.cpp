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
	int messageStart = response.indexOf("<h1>"),
		messageEnd =  response.indexOf("</h1>");
	*message = response.substring(messageStart+4, messageEnd);

	int rStart = response.indexOf("b("),
		rEnd = response.indexOf(",", rStart);

	String rValueString = response.substring(rStart+2, rEnd);

	rStart = rEnd + 1;
	rEnd = response.indexOf(",", rStart);

	String gValueString = response.substring(rStart, rEnd);

	rStart = rEnd + 1;
	rEnd = response.indexOf(")", rStart);

	String bValueString = response.substring(rStart, rEnd);

	*r = atoi(rValueString.c_str());
	*g = atoi(gValueString.c_str());
	*b = atoi(bValueString.c_str());
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
  	http.begin("192.168.49.246", 80, "/GET");
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
	tft.setTextColor(TFT_WHITE, background, true);
	tft.setTextWrap(true);

	delay(5000);

	setupConnection();
}

void loop()
{
	String message = "Nothing";
	int r = 0, g = 0, b = 0;

	String response = fetchData();
	Serial.println(response);
	if (response != "NULL")
		processResponse(response, &message, &r, &g, &b);

	tft.setCursor(0, 0);
	tft.print(message + "\n" + String(millis() / 1000.0));
	delay(3000);
}