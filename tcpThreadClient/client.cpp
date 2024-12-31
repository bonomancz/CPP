#include "client.h"

void Client::run() {
	SOCKET clientSocket;
	string receivedMessage;
	string sendMessage = dat.getSendMessage();

	this->socket.sockInitialize();
	this->socket.sockConnect(clientSocket);
	this->socket.sockSend(clientSocket, sendMessage);
	this->socket.sockReceive(clientSocket, receivedMessage);
	this->socket.sockClose(clientSocket);
	cout << tm.getMillisecTime() << " " << dat.getServerResponse(receivedMessage) << endl;
}

void Client::runInThreads() {
	try {
		string sendMessage = this->dat.getSendMessage();
		string threadLog;
		for (int i = 0; i < this->threadsCount; i++) {
			this->threads.startNewThread([this, &sendMessage, &threadLog]() {
				std::thread::id clientThreadId = std::this_thread::get_id();
				{
					std::unique_lock<std::mutex> lockStart(mtxStart);
					cvStart.wait(lockStart, [this] { return this->threads.getThreadPoolSize() == this->getThreadsCount(); });
				}
				this->cvStart.notify_one();
				try {
					Socket sock;
					SOCKET clientSocket;
					string receivedMessage;
					sock.setRemoteHost(this->remoteHost);
					sock.setRemotePort(this->remotePort);
					sock.sockInitialize();
					sock.sockConnect(clientSocket);

					string isSSL = "";
					if (this->remotePort == 80 || this->remotePort == 8080) {
						sock.sockSend(clientSocket, sendMessage);
						sock.sockReceive(clientSocket, receivedMessage);
					}else {
						// ### SSL CONTEXT ######################
						Openssl openssl;
						SSL_CTX* ctx = nullptr;
						SSL* ssl = nullptr;
						openssl.sslInitialize(ssl, ctx, clientSocket);
						openssl.sslSend(ssl, sendMessage);
						openssl.sslReceive(ssl, receivedMessage);
						openssl.sslCleanup(ctx, ssl);
						isSSL = " (SSL)";
					}
					sock.sockClose(clientSocket);
					{
						std::lock_guard<std::mutex> lockLog(mtxLog);
						threadLog += tm.getMillisecTime() + " " + dat.getServerResponse(receivedMessage) + isSSL + "\n";
					}
				}
				catch (runtime_error &e) {
					{
						std::lock_guard<std::mutex> lockLog(mtxLog);
						threadLog += tm.getMillisecTime() + " " + e.what() + "\n";
					}
				}
				this->threads.setThreadFinished(clientThreadId);
				this->cvStop.notify_one();
			});
		}

		{
			std::unique_lock <std::mutex> lockStop(mtxStop);
			cvStop.wait(lockStop, [this] { return this->threads.getUnfinishedThreadsCount() == 0; });
		}
		this->threads.removeFinishedThreads();
		cout << threadLog;
	}
	catch (runtime_error &e) {
		cerr << "runInThreads(): Runtime error: " << e.what() << endl;
	}
}

void Client::sayHello(string& message) {
	cout << message << endl;
}

void Client::setHost(string& host) {
	this->remoteHost = host;
	dat.setRemoteHost(host);
}

void Client::setPort(int& port) {
	this->remotePort = port;
}

void Client::setUrl(string& url) {
	this->remoteUrl = url;
	dat.setRemoteUrl(url);
}

void Client::setThreadsCount(int& threadsCount) {
	this->threadsCount = threadsCount;
}

int Client::getThreadsCount() {
	return this->threadsCount;
}
