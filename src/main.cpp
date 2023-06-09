#include <String>
#include "Arduino.h"
#include "TFT_eSPI.h"
#include "SPI.h"
//#include "pinConfig.h"
//#include "OneButton.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "displayUtility.h"
#include "URLDecoder.h"

using namespace std;

uint32_t background = TFT_BLACK;

TFT_eSPI tft = TFT_eSPI();
const char *SSID = "x-home";
const char *PASSWORD = "deukecrh";
const char *SERVER_IP = "192.168.49.246";

HTTPClient HTTP;
String fetchData();
void breakHTTPConnection()
{
	HTTP.end();
	Serial.println("Disconnected from HTTP server");
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
	*message = urlDecode(response.substring(idx, response.length()).c_str()).c_str();
}

void setupConnection()
{
	Serial.begin(9600);

	WiFi.begin(SSID, PASSWORD);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
		Serial.println("Connecting to WiFi...");
	}
	Serial.println("Connected to WiFi");

	// Send HTTP GET request
	HTTP.begin(SERVER_IP, 80, "/data");
}

const char *NULL_HTTP_RESPONSE = ""; 
String fetchData()
{
	if (HTTP.GET() == HTTP_CODE_OK)
		return HTTP.getString();
	else 
	{
		Serial.println("Error in HTTP request");
		return NULL_HTTP_RESPONSE;
	}
}

void setup() 
{
	tft.init();
	tft.fillScreen(background);
	tft.setRotation(-1);

	tft.setTextSize(3);
	tft.setTextWrap(true);

	delay(1000);

	setupConnection();
}

void loop()
{
	String message = "Nothing";
	int r = 0, g = 0, b = 0;

	String response = fetchData();
	Serial.println(response);
	if (response != NULL_HTTP_RESPONSE)
		processResponse(response, &message, &r, &g, &b);

	background = convertRGBtoTFT(r, g, b);
	tft.setTextColor(TFT_WHITE, background);
	tft.fillScreen(background);
	tft.setCursor(0, 0);
	tft.print(message + "\n\n" + String(millis() / 1000.0) + " sec");	

	delay(1000);
}