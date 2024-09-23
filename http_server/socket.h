#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <functional>
#include <stdexcept>

#pragma comment(lib, "Ws2_32.lib")
#pragma once

using namespace std;

class Socket{
private:
	WSADATA wsaData;
	SOCKET serverSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	int serverPort = 8443;

	void initialize();
	void setup();
	void bindSock();
	void listenSock();
	void shutdownSock(SOCKET& clientSocket);

public:
	Socket();
	SOCKET acceptSock();
	void receiveSock(SOCKET &clientSocket, string &receivedMessage, std::function<void(SOCKET&, string&)> onMessageReceived);
	void sendSock(SOCKET& clientSocket, string& sendMessage);
	string getClientIpAddr(SOCKET &clientSocket);
};

