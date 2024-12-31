#include "data.h"

string Data::getHttpReponse(int httpResultCode) {
	string textColor, responseText;
	
	switch (httpResultCode) {
		case 200:	textColor = "green";
					responseText = "HTTP/1.1 200 OK";
					break;
		case 404:	textColor = "red";
					responseText = "HTTP/1.1 404 Not Found";
					break;
		case 500:	textColor = "red";
					responseText = "HTTP/1.1 500 Internal Server Error";
					break;
		default:	textColor = "green";
					responseText = "HTTP/1.1 200 OK";
					break;
	}

	string content =
		"<html>"
		"<head>"
		"<title>HTTP Server</title>"
		"</head>"
		"<body style=\"background-color: white;\">"
		"<h1 style=\"color: " + textColor + "; font-family: Tahoma; font-size: 24px;\">" + responseText + "</h1>"
		"</body>"
		"</html>";

	string response = "" + responseText + "\r\n"
		"Content-Type: text/html; charset=UTF-8\r\n"
		"Content-Length: " + std::to_string(content.length()) + "\r\n"
		"Server: HTTP Server" + "\r\n"
		"Connection: close\r\n"
		"Date: Mon, 02 Sep 2024 12:00:00 GMT\r\n"
		"\r\n";
	response += content;
	return response;
}

bool Data::isHttpRequest(string &message) {
	bool output = false;
	regex httpRequestPattern(R"((GET|POST|PUT|DELETE|HEAD|CONNECT|OPTIONS|TRACE|PATCH)\s+\/[^\s]*\s+HTTP\/1\.1)");
	regex headerEndPattern(R"(\r\n\r\n)");
	regex userAgentPattern(R"(User-Agent:\s*.+)");
	if (regex_search(message, httpRequestPattern) && regex_search(message, headerEndPattern) && regex_search(message, userAgentPattern)) {
		output = true;
	}
	return output;
}

bool Data::isExistingUrl(string& message) {
	bool output = false;
	string requestUrl = this->getRequestUrl(message);
	list<string> validUrls = {
		"/",
		"/test",
		"/json",
		"/request",
		"/data"
	};
	for (const auto& url : validUrls) {
		if (url == requestUrl) {
			output = true;
		}
	}
	return output;
}

string Data::getRequestUrl(string& message) {
	string output = "";
	size_t start = message.find(" ") + 1;
	size_t end = message.find(" ", start);
	if (start != std::string::npos && end != std::string::npos) {
		output = message.substr(start, end - start);
	}
	return output;
}

string Data::getClientRequest(string& message) {
	string output;
	istringstream stream(message);
	string line, methodPath, userAgent;
	if (getline(stream, line)) {
		istringstream firstLine(line);
		string method, path;
		firstLine >> method >> path;
		methodPath = method + " " + path;
	}

	while (getline(stream, line)) {
		if (line.rfind("User-Agent:", 0) == 0) {
			userAgent = line.substr(12);
			break;
		}
	}
	output = methodPath + ", " + userAgent;
	return output;
}

string Data::strip(const string& inputString, string chars) {
	try {
		size_t begin = inputString.find_first_not_of(chars);
		if (begin == std::string::npos) {
			return "";
		}
		size_t end = inputString.find_last_not_of(chars);
		return inputString.substr(begin, end - begin + 1);
	}
	catch (runtime_error rex) {
		cerr << rex.what() << endl;
		return "";
	}
}

vector<string> Data::split(const string& inputString, char delimiter) {
	vector<string> output;
	stringstream ss(inputString);
	string token;
	while (getline(ss, token, delimiter)) {
		output.emplace_back(token);
	}
	return output;
}