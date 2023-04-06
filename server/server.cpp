#include <iostream>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

struct RGB 
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

void runServer(int);

int main() 
{
    runServer(80);
    return 0;
}

std::string getHTTPResponse(const RGB& color, const std::string& message) 
{
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html; charset=UTF-8\r\n\r\n";
    response += "<html><body style=\"background-color:rgb(";
    response += std::to_string(color.r) + ",";
    response += std::to_string(color.g) + ",";
    response += std::to_string(color.b) + ")\">";
    response += "<h1>" + message + "</h1>";
    response += "</body></html>";

    return response;
}

std::string getPlainDataResponse(const RGB& color, const std::string& message) 
{
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html; charset=UTF-8\r\n\r\n";
    response += std::to_string(color.r) + " ";
    response += std::to_string(color.g) + " ";
    response += std::to_string(color.b) + " ";
    response += message;
    return response;
}

void handleRequest(SOCKET clientSocket) 
{
    // Получаем запрос от клиента
    char buffer[1024] = { 0 };
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == SOCKET_ERROR) 
    {
        std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        return;
    }

    std::string requestPath(buffer);
    std::string response;

    RGB sampleColor = { 169, 69, 169 };
    std::string sampleMessage = "BRUH????";

    std::cout << requestPath << std::endl;

    // Формируем HTTP-ответ по определённым запросам
    if (requestPath.find("/data") != std::string::npos)
        response = getPlainDataResponse(sampleColor, sampleMessage);
    else if (requestPath.find("/html") != std::string::npos)
        response = getHTTPResponse(sampleColor, sampleMessage);
    else 
        response = "404 Not Found";

    // Получаем цвет из запроса
    //RGB color = { 35, 69, 189 };
    //sscanf(buffer, "GET /color?r=%hhu&g=%hhu&b=%hhu", &color.r, &color.g, &color.b);

    // Отправляем HTTP-ответ клиенту
    int bytesSent = send(clientSocket, response.c_str(), response.size(), 0);
    if (bytesSent == SOCKET_ERROR)
    {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        return;
    }

    // Закрываем соединение с клиентом
    closesocket(clientSocket);
}

void runServer(int port) 
{
    // Инициализируем библиотеку Winsock
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != 0) 
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return;
    }

    // Создаем сокет для прослушивания входящих запросов
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    // Привязываем сокет к порту
    sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    result = bind(listenSocket, (SOCKADDR*)&addr, sizeof(addr));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    // Начинаем прослушивать входящие запросы
    result = listen(listenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) 
    {
        std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    // Обрабатываем входящие запросы
    while (true)
    {
        // Принимаем входящее соединение
        sockaddr_in clientAddr = { 0 };
        int addrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
        
        if (clientSocket == INVALID_SOCKET) 
        {
            std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        // Обрабатываем запрос от клиента
        handleRequest(clientSocket);
    }

    // Закрываем сокет и освобождаем ресурсы Winsock
    closesocket(listenSocket);
    WSACleanup();
}