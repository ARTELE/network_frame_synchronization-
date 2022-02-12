// 
// Copyright 2022 ARTELE Studio
//
/*************************************
*	项目			:	io多路复用demo	 *
*	开始时间		:	2022-02-07	     *
*	编写者		:	ZhangXinHao      * 
*	更新日期		:	2022-02-07       *
**************************************/

#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
/*
	类功能:作为框架的服务端，负责接收客户端数据，维护客户端的socket接口
*/

struct DataPkg
{
	char userName[16];
	char talkName[16];
	char data[128];
};
class NTServer
{
	bool m_Vaild = false;  //记录服务端创建成功还是失败的标志位
	SOCKET m_ServerListenSocket;  //服务端监听接口
	std::unordered_map<int,std::string> m_ClientList;
	std::unordered_map<std::string, int> m_UserNameToId;
public:
	/********************************************
	* 输入: 无									*
	* 输出: 无									*
	* 功能: 默认构造函数							*
	*********************************************/
	NTServer() {}

	NTServer(int p_MajorVer,int p_MinorVer,int p_Port);

	void run();

	bool isVaild() { return m_Vaild; }

};

