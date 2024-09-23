
#include <iostream>
#include "httpd.h"

using namespace std;

int main(int argc, char** argv) {
	try {
		Httpd httpd;
		httpd.start();
	}
	catch(runtime_error &e){
		cerr << "main(): Runtime error: " << e.what() << endl;
	}
	catch(...){
		cerr << "main(): Unknown error occured." << endl;
	}

	return 0;
}
