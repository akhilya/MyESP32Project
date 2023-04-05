all: server.cpp
	g++ server.cpp -o server.exe -lws2_32

clean:
	del server.exe