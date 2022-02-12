#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>

//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
//#include <WinSock2.h>
//
//#pragma comment(lib, "ws2_32.lib")

#include "NTServer.h"
using namespace std;
int main(int argc, char* agrv[])
{
    NTServer server(2,2,9999);
    if (server.isVaild())
    {
        server.run();
    }
    return 0;
}