#include "socket.h"

void Socket::sockInitialize() {
	int rslt;
    if ((rslt = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
        throw runtime_error("Socket::sockInitialize(): WSAStartup failed with error: " + to_string(rslt));
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // resolve remote host address and port
    if ((rslt = getaddrinfo(this->remoteHost.c_str(), to_string(remotePort).c_str(), &hints, &result)) != 0) {
        //WSACleanup();
        throw runtime_error("Socket::sockInitialize(): getaddrinfo failed with error: " + to_string(rslt));
    }
}


void Socket::sockConnect(SOCKET &remoteSocket) {
    if (result == nullptr) {
        throw runtime_error("Socket::SockConnect(): result is nullptr. Cannot proceed.");
    }
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        if ((remoteSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) == INVALID_SOCKET) {
            throw runtime_error("Socket::SockConnect(): Socket failed with error: " + to_string(WSAGetLastError()));
        }
        if (connect(remoteSocket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen)) == SOCKET_ERROR) {
            closesocket(remoteSocket);
            remoteSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    freeaddrinfo(result);
    if (remoteSocket == INVALID_SOCKET) {
        throw runtime_error("Socket::SockConnect(): Unable to connect to server. INVALID_SOCKET (DST HOST:PORT).");
    }
}


void Socket::sockSend(SOCKET &remoteSocket, string &data) {
    int sendRslt;
    if ((sendRslt = send(remoteSocket, data.c_str(), static_cast<int>(strlen(data.c_str())), 0)) == SOCKET_ERROR) {
		throw runtime_error("Send failed with error: " + to_string(WSAGetLastError()));
	}
}


void Socket::sockReceive(SOCKET& remoteSocket, string& rcvdMessage) {
    int rslt = 0;
    const int bufferLength = 8192;
    char receiveBuffer[bufferLength];
    bool headerReceived = false;
    size_t contentLength = 0;

    try {
        do {
            rslt = recv(remoteSocket, receiveBuffer, bufferLength, 0);
            if (rslt > 0) {
                rcvdMessage.append(receiveBuffer, rslt);
                memset(&receiveBuffer, 0, bufferLength);

                if (!headerReceived) {
                    size_t headerEnd = rcvdMessage.find("\r\n\r\n");
                    if (headerEnd != string::npos) {
                        headerReceived = true;
                        size_t contentLengthPosition = rcvdMessage.find("Content-Length: ");
                        if (contentLengthPosition != string::npos) {
                            contentLengthPosition += 16;
                            size_t contentLengthEnd = rcvdMessage.find("\r\n", contentLengthPosition);
                            contentLength = stoi(rcvdMessage.substr(contentLengthPosition, contentLengthEnd - contentLengthPosition));
                        }
                    }
                }
                if (headerReceived && rcvdMessage.size() >= contentLength + rcvdMessage.find("\r\n\r\n") + 4) { // checking receive complete message
                    break;
                }

            }
            else if (rslt == 0) { // client connection closing
                cout << "Client closed connection." << endl;
                break;
            }
            else {
                throw runtime_error("Socket::receiveSock(): Client data receive error: " + to_string(WSAGetLastError()));
            }
        } while (rslt > 0);
    }
    catch (...) {
        closesocket(remoteSocket);
        //WSACleanup();
        throw runtime_error("Socket::receiveSock(): Error: " + to_string(WSAGetLastError()));
    }
}

void Socket::sockClose(SOCKET &remoteSocket) {
    shutdown(remoteSocket, SD_SEND);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    closesocket(remoteSocket);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    WSACleanup();
}


void Socket::setRemoteHost(string& host) {
    this->remoteHost = host;
}

void Socket::setRemotePort(int& port) {
    this->remotePort = port;
}