#pragma once

#include<iostream>
#include<string>
#include<openssl/ssl.h>
#include<openssl/sslerr.h>

#pragma comment (lib, "libssl.lib")
#pragma comment (lib, "libcrypto.lib")

using namespace std;

class Openssl{
private:
public:
	void sslInitialize(SSL*& ssl, SSL_CTX*& ctx, SOCKET& remoteSocket);
	void sslSend(SSL* ssl, string& data);
	void sslReceive(SSL* ssl, string& rcvdMessage);
	void sslCleanup(SSL_CTX* ctx, SSL* ssl);
};

