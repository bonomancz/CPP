#include "socket.h"

Socket::Socket() {
    this->initialize();
    this->setup();
    this->bindSock();
    this->listenSock();
}

void Socket::initialize() {
    int rslt = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (rslt != 0) {
        throw runtime_error("Socket::initialize(): WSAStartup failed with error: " + to_string(rslt));
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // resolve address, port
    rslt = getaddrinfo(NULL, to_string(serverPort).c_str(), &hints, &result);
    if (rslt != 0) {
        //WSACleanup();
        throw runtime_error("Socket::initialize(): getaddrinfo failed with error: " + to_string(rslt));
    }
}

void Socket::setup() {
    this->serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (this->serverSocket == INVALID_SOCKET) {
        freeaddrinfo(result);
        //WSACleanup();
        throw runtime_error("Socket::setup(): Socket failed with error: " + to_string(WSAGetLastError()));
    }
}

void Socket::bindSock() {
    int rslt = ::bind(this->serverSocket, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);
    if (rslt == SOCKET_ERROR) {
        closesocket(this->serverSocket);
        //WSACleanup();
        throw runtime_error("Socket::bindSock(): Bind failed with error: " + to_string(WSAGetLastError()));
    }
}

void Socket::listenSock() {
    int rslt = listen(this->serverSocket, SOMAXCONN);
    if (rslt == SOCKET_ERROR) {
        closesocket(this->serverSocket);
        //WSACleanup();
        throw runtime_error("Socket::listenSock(): Listen failed with error: " + to_string(WSAGetLastError()));
    }
}

SOCKET Socket::acceptSock() {
    SOCKET output = accept(serverSocket, NULL, NULL);
    if (output == INVALID_SOCKET) {
        closesocket(serverSocket);
        //WSACleanup();
        throw runtime_error("Socket::acceptSock(): Accept failed with error: " + to_string(WSAGetLastError()));
    }
    return output;
}

void Socket::receiveSock(SOCKET &clientSocket, string &rcvdMessage, std::function<void(SOCKET&, string&)> onMessageReceived) {
    int rslt = 0;
    const int bufferLength = 8192;
    char receiveBuffer[bufferLength];

    try {
        do {
            rslt = recv(clientSocket, receiveBuffer, bufferLength, 0);
            if (rslt > 0) {
                rcvdMessage.append(receiveBuffer, rslt);
                memset(&receiveBuffer, 0, bufferLength);
                if (rcvdMessage.find("\r\n\r\n") != string::npos) { // if message contains \r\n
                    if (rcvdMessage.find("User-Agent:") != string::npos) {
                        onMessageReceived(clientSocket, rcvdMessage); // solution with callback call
                    }
                }
            }
            else if (rslt == 0) { // client connection closing
                //cout << "Client closed connection." << endl;
                break;
            }
            else {
                throw runtime_error("Socket::receiveSock(): Client data receive error: " + to_string(WSAGetLastError()));
            }
        } while (rslt > 0);
    }
    catch(...){
        closesocket(clientSocket);
        //WSACleanup();
        throw runtime_error("Socket::receiveSock(): Error: " + to_string(WSAGetLastError()));
    }
}

void Socket::sendSock(SOCKET& clientSocket, string& sendMessage) {
    int rslt = send(clientSocket, sendMessage.data(), static_cast<int>(sendMessage.length()), 0);
    if (rslt == SOCKET_ERROR) {
        closesocket(clientSocket);
        //WSACleanup();
        throw runtime_error("Socket::sendSock(): Send failed with error: " + to_string(WSAGetLastError()));
    }
}

void Socket::shutdownSock(SOCKET &clientSocket) {
    int rslt = shutdown(clientSocket, SD_SEND);
    if (rslt == SOCKET_ERROR) {
        closesocket(clientSocket);
        //WSACleanup();
        throw runtime_error("Socket::shutdownSock(): Shutdown failed with error: " + to_string(WSAGetLastError()));
    }
}

string Socket::getClientIpAddr(SOCKET& clientSocket) {
    string output = "";
    sockaddr_in clientIpAddr;
    int addrSize = sizeof(clientIpAddr);
    char clientIP[INET_ADDRSTRLEN];
    if (getpeername(clientSocket, (sockaddr*)&clientIpAddr, &addrSize) == 0) {
        inet_ntop(AF_INET, &clientIpAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        output = string(clientIP);
    }
    return output;
}
