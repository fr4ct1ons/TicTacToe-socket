#include <Winsock2.h>
#include <Ws2tcpip.h>
#include "tictactoe.hpp"
#include "helper.hpp"

#pragma comment (lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define PORT 51234
#define BUFLEN 100

int wrap(char* buf, game_state &s) {
    game_state* buf2 = (game_state*) buf;
    buf2[0] = s;
    return sizeof(game_state);
}

void unwrap(char* buf, int n, game_state& s) {
    game_state* buf2 = (game_state*) buf;
    s = buf2[0];
}

void syncBoard()
{

}

int main () {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        std::cout << "start error" << std::endl;
        return -1;
    }

    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "Socket number: " << listen_socket << std::endl;
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    if (bind(listen_socket, (sockaddr*) &addr, sizeof(sockaddr_in))) {
        std::cout << "Binding error" << std::endl;
        closesocket(listen_socket);
        WSACleanup();
        return -2;
    }

    if (listen(listen_socket, SOMAXCONN)) {
        std::cout << "Listen error" << std::endl;
        closesocket(listen_socket);
        WSACleanup();
        return -3;
    }

    sockaddr_in client1_addr;
    int addr_len = sizeof(sockaddr_in);
    std::cout << "Waiting for client connection..." << std::endl;
    SOCKET conn1_socket = accept(listen_socket, (sockaddr*) &client1_addr, &addr_len);
    if (conn1_socket <= 0) {
        int err = WSAGetLastError();
        std::cout << "Accept error: " << err << std::endl;
        closesocket(listen_socket);
        WSACleanup();
        return -4;
    }

    std::cout << "First client connected." << std::endl;

    sockaddr_in client2_addr;
    std::cout << "Waiting for client connection..." << std::endl;
    SOCKET conn2_socket = accept(listen_socket, (sockaddr*) &client2_addr, &addr_len);
    if (conn2_socket <= 0) {
        int err = WSAGetLastError();
        std::cout << "Accept error2: " << err << std::endl;
        closesocket(listen_socket);
        WSACleanup();
        return -5;
    }

    std::cout << "Second client connected." << std::endl;

    game_state s;
    int sendOk = 0;
    std::string msg = "";

    msg = "SET_PLAYER_CROSS";
    sendString(msg, conn1_socket, client1_addr);
    msg = "SET_PLAYER_CIRCLE";
    sendString(msg, conn2_socket, client2_addr);
    //sendto(conn1_socket, msg.c_str(), msg.size() + 1, 0, (sockaddr*)&client1_addr, sizeof(client1_addr));

    while (true) {
        msg = cmds::playCross;
        sendString(msg, conn1_socket, client1_addr);
        sendString(msg, conn2_socket, client2_addr);
        receiveString(conn1_socket, client1_addr, &msg);
        std::cout << "Cross player sent " << msg << std::endl;

        if(msg == cmds::leaveMatch)
        {
            msg = cmds::otherPlayerLeft;
            sendString(msg, conn2_socket, client2_addr);
            break;
        }

        int crossCell = 0;
        crossCell = std::atoi(msg.c_str());
        s.board[crossCell] = 'x';
        msg = cmds::refreshBoard + ' ' + s.get_board(); //TODO: Não enviar o tabuleiro todo
        sendString(msg, conn1_socket, client1_addr);
        sendString(msg, conn2_socket, client2_addr);

        char getWinner = s.get_winner();
        
        if(getWinner != 0)
        {
            if(getWinner == 'd')
            {
                msg = cmds::draw;
                sendString(msg, conn1_socket, client1_addr);
                sendString(msg, conn2_socket, client2_addr);
                std::cout << "There was a DRAW! No player has won..." << std::endl;
                break;
            }
            else if(getWinner == 'x')
            {
                msg = cmds::crossWins;
                sendString(msg, conn1_socket, client1_addr);
                sendString(msg, conn2_socket, client2_addr);
                std::cout << "The cross player has won!" << std::endl;
                break;
            }
            else if(getWinner == 'o')
            {
                msg = cmds::circleWins;
                sendString(msg, conn1_socket, client1_addr);
                sendString(msg, conn2_socket, client2_addr);
                std::cout << "The circle player has won!" << std::endl;
                break;
            }
        }

        msg = cmds::playCircle;
        sendString(msg, conn1_socket, client1_addr);
        sendString(msg, conn2_socket, client2_addr);
        receiveString(conn2_socket, client2_addr, &msg);
        std::cout << "circle player sent " << msg << std::endl;

        if(msg == cmds::leaveMatch)
        {
            msg = cmds::otherPlayerLeft;
            sendString(msg, conn1_socket, client1_addr);
            break;
        }

        int circleCell = 0;
        circleCell = std::atoi(msg.c_str());
        s.board[circleCell] = 'o';
        msg = cmds::refreshBoard + ' ' + s.get_board(); //TODO: Não enviar o tabuleiro todo
        sendString(msg, conn1_socket, client1_addr);
        sendString(msg, conn2_socket, client2_addr);

        getWinner = s.get_winner();
        
        if(getWinner != 0)
        {
            if(getWinner == 'd')
            {
                msg = cmds::draw;
                sendString(msg, conn1_socket, client1_addr);
                sendString(msg, conn2_socket, client2_addr);
                std::cout << "There was a DRAW! No player has won..." << std::endl;
                break;
            }
            else if(getWinner == 'x')
            {
                msg = cmds::crossWins;
                sendString(msg, conn1_socket, client1_addr);
                sendString(msg, conn2_socket, client2_addr);
                std::cout << "The cross player has won!" << std::endl;
                break;
            }
            else if(getWinner == 'o')
            {
                msg = cmds::circleWins;
                sendString(msg, conn1_socket, client1_addr);
                sendString(msg, conn2_socket, client2_addr);
                std::cout << "The circle player has won!" << std::endl;
                break;
            }
        }
    }

    // Close connection
    shutdown(conn1_socket, SD_BOTH);
    closesocket(conn1_socket);

    shutdown(conn2_socket, SD_BOTH);
    closesocket(conn2_socket);

    // Close listening socket
    closesocket(listen_socket);
    WSACleanup();
    return 0;

}