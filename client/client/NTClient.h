#pragma once
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

struct DataPkg
{
	char userName[16];
	char talkName[16];
	char data[128];
};

class NTClient
{
	bool m_Vaild = false;
	SOCKET m_ConnectSocket;
public:
	NTClient() {}
	NTClient(int p_MajorVer, int p_MinorVer,const char * p_IpAddress, int p_Port);
	void run();
	void test_send_run();
	bool isVaild() { return m_Vaild; }
};

