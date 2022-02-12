#include "NTClient.h"
#include <iostream>
NTClient::NTClient(int p_MajorVer, int p_MinorVer, const char* p_IpAddress, int p_Port)
{
	WSADATA l_WsaData;
	int l_Result = WSAStartup(MAKEWORD(2, 2), &l_WsaData);
	if (l_Result != NO_ERROR)
	{
		return;
	}

	//创建一个socket连接server
	
	m_ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_ConnectSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return;
	}

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	sockaddr_in l_ClientService;
	l_ClientService.sin_family = AF_INET;
	l_ClientService.sin_addr.s_addr = inet_addr("127.0.0.1");
	l_ClientService.sin_port = htons(9999);

	//连接服务器
	if (connect(m_ConnectSocket, (SOCKADDR*)&l_ClientService, sizeof(l_ClientService)) == SOCKET_ERROR)
	{
		WSACleanup();
	}
	m_Vaild = true;
	std::cout << "客户端创建连接成功" << std::endl;
}

void NTClient::run()
{
	
	while (true)
	{
		fd_set l_FdReads;
		FD_ZERO(&l_FdReads);

		FD_SET(m_ConnectSocket, &l_FdReads);

		timeval timeout = { 0, 0 };
		if (select(m_ConnectSocket, &l_FdReads, NULL, NULL, &timeout) < 0)
		{
			break;
		}

		if (FD_ISSET(m_ConnectSocket, &l_FdReads))
		{
			//FD_CLR(ConnectSocket, &fdReads);

			DataPkg data;
			// 5、接收客户端的请求
			// 先接收消息头
			int recvLen = recv(m_ConnectSocket, (char*) & data, sizeof(DataPkg), 0);
			if (recvLen <= 0)
			{
				printf("与服务器断开连接，任务结束...");
				return;
			}
			std::cout << data.data << std::endl;
		}
	}
	WSACleanup();
}

void NTClient::test_send_run()
{
	DataPkg data;
	std::cout << "输入用户名" << std::endl;
	std::cin >> data.userName;
	// 输入请求命令
	while (true)
	{
		std::cout << "输入要对话的用户名" << std::endl;
		std::cin >> data.talkName;
		std::cout << "输入对话内容" << std::endl;
		std::cin >> data.data;
		send(m_ConnectSocket, (const char*) & data, sizeof(DataPkg), 0);
	}
}
