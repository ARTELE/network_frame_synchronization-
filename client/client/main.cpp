#include <iostream>
#include "NTClient.h"
#include <thread>
using namespace std;
int main(int argc, char* argv[])
{
	
	NTClient client(2, 2, "127.0.0.1", 9999);

	thread receive([&]() {
		std::cout << "׼���ý�����Ϣ" << std::endl;
		client.run();
		});

	thread send([&]() {
		if (client.isVaild())
		{
			client.test_send_run();
		}
		});
	receive.join();
	send.join();
	return 0;
}