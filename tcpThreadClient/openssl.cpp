#include "openssl.h"


void Openssl::sslInitialize(SSL*& ssl, SSL_CTX*& ctx, SOCKET& remoteSocket) {
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();
    if (!(ctx = SSL_CTX_new(TLS_client_method()))) {
		throw runtime_error("initializeSSL(): Failed to create SSL context.");
	}
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, static_cast<int>(remoteSocket));
    if (SSL_connect(ssl) != 1) {
        this->sslCleanup(ctx, ssl);
        throw runtime_error("initializeSSL(): Failed to establish SSL connection.");
    }
}

void Openssl::sslSend(SSL* ssl, string& data) {
	if (SSL_write(ssl, data.c_str(), static_cast<int>(data.size())) < 1) {
		throw runtime_error("SSL data send failed.");
	}
}

void Openssl::sslReceive(SSL* ssl, string& rcvdMessage) {
    int rslt = 0;
    const int bufferLength = 8192;
    char receiveBuffer[bufferLength];
    bool headerReceived = false;
    size_t contentLength = 0;

    try {
        do {
            rslt = SSL_read(ssl, receiveBuffer, bufferLength);
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
                cerr << "Client closed connection." << endl;
                break;
            }
            else {
                throw runtime_error("Socket::sslReceive(): Client data receive error.");
            }
        } while (rslt > 0);
    }
    catch (...) {
        SSL_free(ssl);
        throw runtime_error("Socket::sslReceive(): Unknown error.");
    }
}

void Openssl::sslCleanup(SSL_CTX* ctx, SSL* ssl) {
	SSL_free(ssl);
	SSL_CTX_free(ctx);
	EVP_cleanup();
}
