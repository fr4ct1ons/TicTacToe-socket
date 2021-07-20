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

int main () {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        std::cout << "start error" << std::endl;
        return -1;
    }

    SOCKET conn_socket = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "Socket number: " << conn_socket << std::endl;
    
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(conn_socket, (sockaddr*) &server_addr, sizeof(sockaddr_in))) {
        int err = WSAGetLastError();
        std::cout << "Connect error: " << err << std::endl;
        closesocket(conn_socket);
        WSACleanup();
        return -2;
    }

    int serverLength = sizeof(server_addr);

    //char *buffer = new char[1024];
    std::string msg = "";
    int bytesIn = receiveString(conn_socket, server_addr, &msg);
    //int bytesIn = recvfrom(conn_socket, buffer, 1024, 0, (sockaddr*)&server_addr, &serverLength);
    if (bytesIn == SOCKET_ERROR)
    {
        std::cout << "ERROR RECEIVING MESSAGE FROM CLIENT - error: " << WSAGetLastError() << std::endl;
        return -3;
    }
    int playerMode = 0;

    const int circleMode = 1, crossMode = 2;
    //std::cout << "Message received from Server: " << msg << std::endl;
    if(msg == "SET_PLAYER_CIRCLE")
    {
        playerMode = circleMode;
        std::cout << "You will play as circle." << std::endl;
    }
    else if(msg == "SET_PLAYER_CROSS")
    {
        playerMode = crossMode;
        std::cout << "You will play as cross." << std::endl;
    }

    game_state s;
    while (true) {
        int bytesIn = receiveString(conn_socket, server_addr, &msg);
        
        std::string token = msg.substr(0, msg.find(' '));
        //std::cout << "Token: " << token << std::endl;
        if(token == cmds::refreshBoard)
        {
            //std::cout << "Refreshing board." << std::endl;
            std::string split = msg.substr(msg.find(' ') + 1, 9);
            std::cout << "Refreshing board:" << split << "-" << std::endl;
            s.set_board(split);
            s.print_board();
        }

        if(msg == cmds::draw)
        {
            std::cout << "There was a DRAW! No player has won..." << std::endl;
            break;
        }
        else if(msg == cmds::circleWins)
        {
            if(playerMode == circleMode)
            {
                std::cout << "Circle wins! You have won!" << std::endl;
            }
            else
            {
                std::cout << "Circle wins! You have lost..." << std::endl;
            }
            break;
            
        }
        else if(msg == cmds::crossWins)
        {
            if(playerMode == crossMode)
            {
                std::cout << "Cross wins! You have won!" << std::endl;
            }
            else
            {
                std::cout << "Cross wins! You have lost..." << std::endl;
            }
            break;
            
        }

        if(msg == cmds::playCircle)
        {
            if(playerMode == circleMode)
            {
                int cell = -1;
                while(cell == -1)
                {
                    std::cout << "It's your turn. Enter which cell you want to play at." << std::endl;
                    std::cin >> cell;
                    if(cell >= 0 && cell <= 8)
                    {
                        if(s.board[cell] == ' ')
                        {
                            char val[50];
                            itoa(cell, val, 10);
                            msg = val;
                            sendString(msg, conn_socket, server_addr);
                        }
                        else
                        {
                            std::cout << "There is already a '" << s.board[cell] << "' at the selected cell." << std::endl;
                            cell = -1;
                        }
                    }
                    else
                    {
                        std::cout << "Invalid cell!" << std::endl;
                        cell = -1;
                    }
                }
            }
            else
                std::cout << "It's the other player's turn." << std::endl;

        }
        if(msg == cmds::playCross)
        {
            if(playerMode == crossMode)
            {
                int cell = -1;
                while(cell == -1)
                {
                    std::cout << "It's your turn. Enter which cell you want to play at." << std::endl;
                    std::cin >> cell;
                    if(cell >= 0 && cell <= 8)
                    {
                        if(s.board[cell] == ' ')
                        {
                            char val[50];
                            itoa(cell, val, 10);
                            msg = val;
                            sendString(msg, conn_socket, server_addr);
                        }
                        else
                        {
                            std::cout << "There is already a '" << s.board[cell] << "' at the selected cell." << std::endl;
                            cell = -1;
                        }
                    }
                    else
                    {
                        std::cout << "Invalid cell!" << std::endl;
                        cell = -1;
                    }
                }
            }
            else
                std::cout << "It's the other player's turn." << std::endl;
        }
        
    }

    // Close connection
    shutdown(conn_socket, SD_BOTH);
    closesocket(conn_socket);

    WSACleanup();
    return 0;

}