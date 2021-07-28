#pragma once
#include <Winsock2.h>
#include <Ws2tcpip.h>

#pragma comment (lib, "ws2_32.lib")
#include<string>

using namespace std;

int sendString(std::string msg, SOCKET sock, sockaddr_in addr)
{
    return sendto(sock, msg.c_str(), msg.size() + 1, 0, (sockaddr*)&addr, sizeof(addr));
}

int receiveString(SOCKET sock, sockaddr_in addr, std::string *str, int bufferSize=1024)
{
    int serverLength = sizeof(addr);

    char *buffer = new char[bufferSize];
    int bytesIn = recvfrom(sock, buffer, 1024, 0, (sockaddr*)&addr, &serverLength);
    (*str) = buffer;
    delete[] buffer;
    return bytesIn;
    if (bytesIn == SOCKET_ERROR)
    {
        std::cout << "ERROR RECEIVING MESSAGE FROM CLIENT - error: " << WSAGetLastError() << std::endl;
        return -3;
    }
}

namespace cmds
{
    const string playCross = "PLAY_CROSS";
    const string playCircle = "PLAY_CIRCLE";
    const string refreshBoard = "REFRESH_BOARD";
    const string circleWins = "CIRCLE_WINS";
    const string crossWins = "CROSS_WINS";
    const string draw = "DRAW_GAME";
    const string leaveMatch = "LEAVE_MATCH";
    const string otherPlayerLeft = "OTHER_PLAYER_HASLEFT";
}