﻿#include <iostream>
#include "Listener.hpp"

using namespace net;
using namespace std;

int main()
{
	try {
		Listener listener(endpoint(ip_address::Any, 8888));
		listener.Run();

		getchar();
	}
	catch (exception& e) {
		cout << e.what() << '\n';
	}
	
	return 0;
}