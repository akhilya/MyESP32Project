#include <WiFi.h>

const char* ssid = "your_SSID_here";
const char* password = "your_PASSWORD_here";

struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    char text[256];
};

void setupConnection() {
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }

    WiFiClient client;
    IPAddress serverIP(192, 168, 1, 100);
    int serverPort = 1234;
    if (client.connect(serverIP, serverPort)) {
        client.write("get_data", 8);
    }

    RGB data;
    int bytesRead = client.read((uint8_t*)&data, sizeof(RGB));
    if (bytesRead == sizeof(RGB)) {
        Serial.println("Received data:");
        Serial.print("Red: ");
        Serial.println(data.r);
        Serial.print("Green: ");
        Serial.println(data.g);
        Serial.print("Blue: ");
        Serial.println(data.b);
        Serial.print("Text: ");
        Serial.println(data.text);
    }

    client.stop();
}