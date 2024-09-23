#include "httpd.h"


void Httpd::start() {
	try {
		SOCKET clientSocket;
		while (true) {
			try {
				if ((clientSocket = sock.acceptSock()) != INVALID_SOCKET) {
					this->threads.startNewThread([this, &clientSocket]() {
						this->handleClient(sock, clientSocket);
					});
					//this->status();
					this->threads.removeFinishedThreads();
				}
				else {
					std::this_thread::sleep_for(std::chrono::milliseconds(100)); // busy waiting protection
				}
			}
			catch (runtime_error& e) {
				cerr << "main(): Runtime error: " << e.what() << endl;
			}
		}
	}
	catch (runtime_error& e) {
		cerr << "main(): Runtime error: " << e.what() << endl;
	}
	catch (...) {
		cerr << "main(): Unknown error occured." << endl;
	}
}


void Httpd::stop() {
	this->threads.removeAllThreads();
}


void Httpd::status() {
	//cout << "Threads in ThreadPool: " << to_string(this->threads.getThreadPoolSize()) << endl;
	cout << this->threads.getThreadPoolStatistic() << endl;
}


void Httpd::handleClient(Socket& sock, SOCKET clientSocket) {
	try {
		string clientMessage;
		auto callbackHandler = [this](SOCKET &clientSocket, string &clientMessage) {
			this->receivedMessageHandler(clientSocket, clientMessage);
		};
		sock.receiveSock(clientSocket, clientMessage, callbackHandler);
		closesocket(clientSocket);
	}
	catch (runtime_error& e) {
		cerr << "HandleClient(): Runtime error: " << e.what() << endl;
	}
	catch (...) {
		cerr << "HandleClient(): Unknown error occured." << endl;
	}
}


void Httpd::receivedMessageHandler(SOCKET &clientSocket, string &clientMessage) {
	try {
		string clientInfo = tm.getMillisecTime() + " Client: " + string(sock.getClientIpAddr(clientSocket));
		if (datas.isHttpRequest(clientMessage)) {
			string sendMessage = datas.getHttpReponse(404);
			if (datas.isExistingUrl(clientMessage)) {
				sendMessage = datas.getHttpReponse(200);
			}
			sock.sendSock(clientSocket, sendMessage);
			clientInfo += ", " + datas.getClientRequest(clientMessage);
			cout << clientInfo << endl;
		}
		// set thread as finished
		std::thread::id clientThreadId = std::this_thread::get_id();
		this->threads.setThreadFinished(clientThreadId);
	}
	catch (runtime_error& e) {
		cerr << "receivedMessageHandler(): Runtime error: " << e.what() << endl;
	}
	catch (...) {
		cerr << "receivedMessageHandler(): Unknown error occured." << endl;
	}
}