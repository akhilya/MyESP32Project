#include <iostream>
#include <string>
#include <cstring>
#include <winsock2.h>
#include "../include/RGBString.h"

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        std::cout << "socket creation error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(8080);

    if (bind(ListenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        std::cout << "bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is running, waiting for client to connect...\n";

    SOCKET ClientSocket = INVALID_SOCKET;

    while (ClientSocket == INVALID_SOCKET) {
        ClientSocket = accept(ListenSocket, NULL, NULL);
        printf("Listening on: %s", service.sin_addr.s_addr);
    }

    std::cout << "Client connected!\n";

    RGBString data = { "Hello, client!", 255, 0, 0 };

    while (true) {
        // Отправляем данные клиенту
        int bytesSent = send(ClientSocket, (char*)&data, sizeof(data), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Sent message:\n";
        std::cout << "Text: " << data.text << '\n';
        std::cout << "Color: (" << data.red << ", " << data.green << ", " << data.blue << ")\n\n";

        // Ждем 1 секунду перед отправкой следующего сообщения
        Sleep(1000);

        // Изменяем данные структуры
        data.red = (data.red + 10) % 256;
        data.green = (data.green + 20) % 256;
        data.blue = (data.blue + 30) % 256;
        std::string message = "Hello, client! Count: ";
        message += std::to_string(data.red);
        strcpy(data.text, message.c_str());
    }

    closesocket(ClientSocket);
    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}
