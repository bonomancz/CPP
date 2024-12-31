#include "data.h"

string Datas::getSendMessage() {
	string sendMessage = "GET " + this->url + " HTTP/1.1\r\n"s +
		"Host: " + this->host + "\r\n"s +
		"User-Agent: Customer HTTP Client/6.0\r\n"s +
		"Connection: close\r\n"s +
		"\r\n";
	return sendMessage;
}

void Datas::trim(string& message) {
	// Trim
	message.erase(0, message.find_first_not_of(" \t\n\r"));
	message.erase(message.find_last_not_of(" \t\n\r") + 1);
}

string Datas::getServerResponse(string& message) {
	string line, output;
	bool responseSet = false, httpServerSet = false;
	istringstream inStream(message);
	while (getline(inStream, line)) {
		this->trim(line);
		if (line.find("HTTP/1.1") != string::npos && !responseSet) {
			output += line;
			responseSet = true;
		}
		if (line.find("Server:") != string::npos && !httpServerSet) {
			output += ", " + line;
			httpServerSet = true;
		}
		if (responseSet && httpServerSet) {
			break;
		}
	}

	return output;
}

void Datas::setRemoteUrl(string& url) {
	this->url = url;
}

void Datas::setRemoteHost(string& host) {
	this->host = host;
}
