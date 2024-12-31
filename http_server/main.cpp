#include <iostream>
#include <string>
#include "httpd.h"

using namespace std;

int main(int argc, char** argv) {
	try {
		Httpd httpd;
		string configFile = httpd.processArguments(argc, argv)[0];
		httpd.setServerConfigFile(configFile);
		httpd.initialize();
		httpd.start();
	}
	catch(runtime_error &e){
		cerr << "Runtime error: " << e.what() << endl;
	}
	catch(...){
		cerr << "Unknown error occured." << endl;
	}

	return 0;
}
