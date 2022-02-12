#include "NTServer.h"
#include <iostream>
NTServer::NTServer(int p_MajorVer, int p_MinorVer, int p_Port)
{
    WORD l_VersionRequested;
    WSADATA l_WsaData;
    l_VersionRequested = MAKEWORD(p_MajorVer, p_MinorVer);
    /**
    * 判断一下创建是否成功
    * 检查所需要创建的版本是否可以成功创建
    * 检查网络端口绑定是否成功
    * 检查监听是否成功
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
    std::cout << "服务端启动成功" << std::endl;

    while (true)
    {
        // Berkeley sockets
        //下面是三种描述符，分别标识当前可读取，可写入，异常
        fd_set l_ReadFds;
        fd_set l_WriteFds;
        fd_set l_ExceptFds;

        // 先将描述符统一清零
        FD_ZERO(&l_ReadFds);
        FD_ZERO(&l_WriteFds);
        FD_ZERO(&l_ExceptFds);

        // 然后将描述符加入到socket集合中，后面要对其进行扫描过滤出我们需要的结果
        FD_SET(m_ServerListenSocket, &l_ReadFds);
        FD_SET(m_ServerListenSocket, &l_WriteFds);
        FD_SET(m_ServerListenSocket, &l_ExceptFds);

        // 将现有的客户端socket加入到集合中
        for (auto & client: m_ClientList)
        {
            FD_SET(client.first, &l_ReadFds);
        }

        // 设置超时时间 select 非阻塞
        timeval timeout = { 1, 0 };
        if (select(m_ServerListenSocket + 1, &l_ReadFds, &l_WriteFds, &l_ExceptFds, &timeout) < 0)
        {
            //此时说明所有客户端都离线
            break;
        }

        // 判断监听socket是否有写入，有的话说明有新的连接请求
        if (FD_ISSET(m_ServerListenSocket, &l_ReadFds))//listensocket
        {
            SOCKADDR_IN l_ClientAddr = {};
            int l_AddrLen = sizeof(SOCKADDR_IN);
            SOCKET l_ClientSocket = INVALID_SOCKET;
            l_ClientSocket = accept(m_ServerListenSocket, (SOCKADDR*)&l_ClientAddr, &l_AddrLen);
            //处理有效的客户端
            if (INVALID_SOCKET != l_ClientSocket)
            {
                // 加入新客户端
                m_ClientList[l_ClientSocket] = "no_name";
                std::cout << "新客户端连接" << std::endl;
            }
        }

        //处理可以读取数据的客户端
        for (int i = 0; i < (int)l_ReadFds.fd_count; ++i)
        {
            if (l_ReadFds.fd_array[i] == m_ServerListenSocket)
                continue;
            DataPkg data;
            SOCKET l_ClientSocket = l_ReadFds.fd_array[i];   
            if (recv(l_ClientSocket, (char*) & data, sizeof(DataPkg), 0) <= 0)
            {
                //说明客户端此时已经关闭，要从列表中删除客户端
                for (auto client : m_ClientList)
                {
                    if (client.first == l_ClientSocket)
                    {
                        m_ClientList.erase(client.first);
                        break;
                    }
                }
                std::cout << "删除客户端" << std::endl;
            }
            else
            {
                //处理接受到的数据
                //std::cout << szRecv << std::endl;
                //此时列表中还没有对应这个id的用户名,那么将其映射
                if (m_UserNameToId.find(data.userName) == m_UserNameToId.end())
                {
                    m_UserNameToId[data.userName] = l_ClientSocket;
                }
                //在列表中找到了可以与之会话的用户，则转发消息
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
