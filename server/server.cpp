#include <iostream>
#include <string>
#include "winsock2.h"
#include "windows.h"
#include "stdio.h"
#include <fstream>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

struct RGB 
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

void runServer(int);
void readAndStoreUserPage(string);

string userPageHTML;

int main() 
{
	readAndStoreUserPage("userPage.html");
	runServer(80);

	return 0;
}

string getHTTPResponse(const RGB& color, const string& message) 
{
	string response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/html; charset=UTF-8\r\n\r\n";
	response += userPageHTML;

	return response;
}

string getPlainDataResponse(const RGB& color, const string& message) 
{
	string response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/html; charset=UTF-8\r\n\r\n";
	response += to_string(color.r) + " ";
	response += to_string(color.g) + " ";
	response += to_string(color.b) + " ";
	response += message;
	return response;
}

RGB bgColor = { 255, 0, 0 };
string sampleMessage = "Sample message";

void handleRequest(SOCKET clientSocket) 
{
	// Получаем запрос от клиента
	char buffer[1024] = { 0 };
	int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (bytesReceived == SOCKET_ERROR) 
	{
		cerr << "recv failed: " << WSAGetLastError() << endl;
		return;
	}

	string request(buffer);
	string response;

	cout << request << endl;

	// Формируем HTTP-ответ по определённым запросам
	if (request.find("/data") != string::npos)
		response = getPlainDataResponse(bgColor, sampleMessage);
	else if (request.find("/html") != string::npos)
	{
		// Получаем цвет из запроса
		char messageBuffer[500];
		sscanf(request.c_str(), "GET /?r=%hhu&g=%hhu&b=%hhu&message=%s", 
			&bgColor.r, &bgColor.g, &bgColor.b, messageBuffer);
		sampleMessage = messageBuffer;
		// Подготавливаем ответ на запрос 
		response = getHTTPResponse(bgColor, sampleMessage);
	}
	else 
		response = "404 Not Found";

	// Отправляем HTTP-ответ клиенту
	int bytesSent = send(clientSocket, response.c_str(), response.size(), 0);
	if (bytesSent == SOCKET_ERROR)
	{
		cerr << "send failed: " << WSAGetLastError() << endl;
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
		cerr << "WSAStartup failed: " << result << endl;
		return;
	}

	// Создаем сокет для прослушивания входящих запросов
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		cerr << "socket failed: " << WSAGetLastError() << endl;
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
		cerr << "bind failed: " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	// Начинаем прослушивать входящие запросы
	result = listen(listenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) 
	{
		cerr << "listen failed: " << WSAGetLastError() << endl;
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
			cerr << "accept failed: " << WSAGetLastError() << endl;
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

void readAndStoreUserPage(string filename)
{
	ifstream ifs(filename.c_str());
	userPageHTML.assign(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
}