#include "ClientManager.h"

const char* ssid = "bruhhh";
const char* password = "leha_8907";

RGBString ClientManager::receivedData;

void ClientManager::setupConnection() 
{
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }

    WiFiClient client;
    IPAddress serverIP(127, 0, 0, 1);
    int serverPort = 8080;
    if (client.connect(serverIP, serverPort)) {
        client.write("get_data", 8);
    }

    int bytesRead = client.read((uint8_t*)&receivedData, sizeof(RGBString));
    if (bytesRead == sizeof(RGBString)) {
        Serial.println("Received data:");
        Serial.print("Red: ");
        Serial.println(receivedData.red);
        Serial.print("Green: ");
        Serial.println(receivedData.green);
        Serial.print("Blue: ");
        Serial.println(receivedData.blue);
        Serial.print("Text: ");
        Serial.println(receivedData.text);
    }

    client.stop();
}