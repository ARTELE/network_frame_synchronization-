#include "NTServer.h"
#include <iostream>
NTServer::NTServer(int p_MajorVer, int p_MinorVer, int p_Port)
{
    WORD l_VersionRequested;
    WSADATA l_WsaData;
    l_VersionRequested = MAKEWORD(p_MajorVer, p_MinorVer);
    /**
    * �ж�һ�´����Ƿ�ɹ�
    * �������Ҫ�����İ汾�Ƿ���Գɹ�����
    * �������˿ڰ��Ƿ�ɹ�
    * �������Ƿ�ɹ�
    **/
    if (WSAStartup(l_VersionRequested, &l_WsaData) == 0)
    {
        if (LOBYTE(l_WsaData.wVersion) != p_MinorVer || HIBYTE(l_WsaData.wVersion) != p_MajorVer)
        {
            WSACleanup();
        }
        else
        {
            m_ServerListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (m_ServerListenSocket == INVALID_SOCKET) 
            {
                WSACleanup();
            }
            else
            {
                sockaddr_in l_Service;
                l_Service.sin_family = AF_INET;
                l_Service.sin_addr.s_addr = INADDR_ANY;
                l_Service.sin_port = htons(p_Port);
                if (bind(m_ServerListenSocket,(SOCKADDR*)&l_Service,sizeof(l_Service)) == SOCKET_ERROR)
                {
                    closesocket(m_ServerListenSocket);
                    WSACleanup();
                }
                else
                {
                    if (listen(m_ServerListenSocket, 5) == SOCKET_ERROR) 
                    {
                        closesocket(m_ServerListenSocket);
                        WSACleanup();
                    }
                    else
                    {
                        m_Vaild = true;
                    }
                }
            }
        }
    }
}

void NTServer::run()
{
    std::cout << "����������ɹ�" << std::endl;

    while (true)
    {
        // Berkeley sockets
        //�������������������ֱ��ʶ��ǰ�ɶ�ȡ����д�룬�쳣
        fd_set l_ReadFds;
        fd_set l_WriteFds;
        fd_set l_ExceptFds;

        // �Ƚ�������ͳһ����
        FD_ZERO(&l_ReadFds);
        FD_ZERO(&l_WriteFds);
        FD_ZERO(&l_ExceptFds);

        // Ȼ�����������뵽socket�����У�����Ҫ�������ɨ����˳�������Ҫ�Ľ��
        FD_SET(m_ServerListenSocket, &l_ReadFds);
        FD_SET(m_ServerListenSocket, &l_WriteFds);
        FD_SET(m_ServerListenSocket, &l_ExceptFds);

        // �����еĿͻ���socket���뵽������
        for (auto & client: m_ClientList)
        {
            FD_SET(client.first, &l_ReadFds);
        }

        // ���ó�ʱʱ�� select ������
        timeval timeout = { 1, 0 };
        if (select(m_ServerListenSocket + 1, &l_ReadFds, &l_WriteFds, &l_ExceptFds, &timeout) < 0)
        {
            //��ʱ˵�����пͻ��˶�����
            break;
        }

        // �жϼ���socket�Ƿ���д�룬�еĻ�˵�����µ���������
        if (FD_ISSET(m_ServerListenSocket, &l_ReadFds))//listensocket
        {
            SOCKADDR_IN l_ClientAddr = {};
            int l_AddrLen = sizeof(SOCKADDR_IN);
            SOCKET l_ClientSocket = INVALID_SOCKET;
            l_ClientSocket = accept(m_ServerListenSocket, (SOCKADDR*)&l_ClientAddr, &l_AddrLen);
            //������Ч�Ŀͻ���
            if (INVALID_SOCKET != l_ClientSocket)
            {
                // �����¿ͻ���
                m_ClientList[l_ClientSocket] = "no_name";
                std::cout << "�¿ͻ�������" << std::endl;
            }
        }

        //������Զ�ȡ���ݵĿͻ���
        for (int i = 0; i < (int)l_ReadFds.fd_count; ++i)
        {
            if (l_ReadFds.fd_array[i] == m_ServerListenSocket)
                continue;
            DataPkg data;
            SOCKET l_ClientSocket = l_ReadFds.fd_array[i];   
            if (recv(l_ClientSocket, (char*) & data, sizeof(DataPkg), 0) <= 0)
            {
                //˵���ͻ��˴�ʱ�Ѿ��رգ�Ҫ���б���ɾ���ͻ���
                for (auto client : m_ClientList)
                {
                    if (client.first == l_ClientSocket)
                    {
                        m_ClientList.erase(client.first);
                        break;
                    }
                }
                std::cout << "ɾ���ͻ���" << std::endl;
            }
            else
            {
                //������ܵ�������
                //std::cout << szRecv << std::endl;
                //��ʱ�б��л�û�ж�Ӧ���id���û���,��ô����ӳ��
                if (m_UserNameToId.find(data.userName) == m_UserNameToId.end())
                {
                    m_UserNameToId[data.userName] = l_ClientSocket;
                }
                //���б����ҵ��˿�����֮�Ự���û�����ת����Ϣ
                if (m_UserNameToId.find(data.talkName) != m_UserNameToId.end())
                {
                    send(m_UserNameToId[data.talkName], (char *)&data, sizeof(DataPkg), 0);
                }
            }
        }
    }

    for (auto & client :m_ClientList)
    {
        closesocket(client.first);
    }
    closesocket(m_ServerListenSocket);
    WSACleanup();

}
