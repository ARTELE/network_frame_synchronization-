// 
// Copyright 2022 ARTELE Studio
//
/*************************************
*	��Ŀ			:	io��·����demo	 *
*	��ʼʱ��		:	2022-02-07	     *
*	��д��		:	ZhangXinHao      * 
*	��������		:	2022-02-07       *
**************************************/

#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
/*
	�๦��:��Ϊ��ܵķ���ˣ�������տͻ������ݣ�ά���ͻ��˵�socket�ӿ�
*/

struct DataPkg
{
	char userName[16];
	char talkName[16];
	char data[128];
};
class NTServer
{
	bool m_Vaild = false;  //��¼����˴����ɹ�����ʧ�ܵı�־λ
	SOCKET m_ServerListenSocket;  //����˼����ӿ�
	std::unordered_map<int,std::string> m_ClientList;
	std::unordered_map<std::string, int> m_UserNameToId;
public:
	/********************************************
	* ����: ��									*
	* ���: ��									*
	* ����: Ĭ�Ϲ��캯��							*
	*********************************************/
	NTServer() {}

	NTServer(int p_MajorVer,int p_MinorVer,int p_Port);

	void run();

	bool isVaild() { return m_Vaild; }

};

