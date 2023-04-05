#include <iostream>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

struct RGB {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

void run_server(int);

int main() {
    run_server(80);
    return 0;
}

std::string get_http_response(const RGB& color, const std::string& message) {
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

void handle_request(SOCKET client_socket) {
    // Получаем запрос от клиента
    char buffer[1024] = { 0 };
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == SOCKET_ERROR) {
        std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        return;
    }

    // Получаем цвет из запроса
    RGB color = { 35, 69, 189 };
    sscanf(buffer, "GET /color?r=%hhu&g=%hhu&b=%hhu", &color.r, &color.g, &color.b);

    // Формируем HTTP-ответ
    std::string response = get_http_response(color, "Hello, world!");

    // Отправляем HTTP-ответ клиенту
    int bytes_sent = send(client_socket, response.c_str(), response.size(), 0);
    if (bytes_sent == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        return;
    }

    // Закрываем соединение с клиентом
    closesocket(client_socket);
}

void run_server(int port) {
    // Инициализируем библиотеку Winsock
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return;
    }

    // Создаем сокет для прослушивания входящих запросов
    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == INVALID_SOCKET) {
        std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    // Привязываем сокет к порту
    sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    result = bind(listen_socket, (SOCKADDR*)&addr, sizeof(addr));
    if (result == SOCKET_ERROR) {
        std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
        closesocket(listen_socket);
        WSACleanup();
        return;
    }

    // Начинаем прослушивать входящие запросы
    result = listen(listen_socket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listen_socket);
        WSACleanup();
        return;
    }

    // Обрабатываем входящие запросы
    while (true) {
        // Принимаем входящее соединение
        sockaddr_in client_addr = { 0 };
        int addr_len = sizeof(client_addr);
        SOCKET client_socket = accept(listen_socket, (SOCKADDR*)&client_addr, &addr_len);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
            closesocket(listen_socket);
            WSACleanup();
            return;
        }

        // Обрабатываем запрос от клиента
        handle_request(client_socket);
    }

    // Закрываем сокет и освобождаем ресурсы Winsock
    closesocket(listen_socket);
    WSACleanup();
}