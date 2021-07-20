#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")


int main()
{
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	
	int wsok = WSAStartup(version, &data);

	if (wsok != 0)
	{
		std::cout << "ERROR STARTING WINSOCK - error: " << wsok << std::endl;
		return 1;
	}

	SOCKET skt = socket(AF_INET, SOCK_DGRAM, 0);
	sockaddr_in serverHint;
	serverHint.sin_addr.S_un.S_addr = ADDR_ANY;
	serverHint.sin_family = AF_INET;
	serverHint.sin_port = htons(54000);

	if (bind(skt, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
	{
		std::cout << "ERROR BINDING SOCKET - error: " << WSAGetLastError() << std::endl;
		return 2;
	}

	sockaddr_in client;
	int clientLength = sizeof(client);
	ZeroMemory(&client, clientLength);

	char *buffer = new char[1024]();

	std::cout << "Server setup successful." << std::endl;
	
	while (true)
	{
		int bytesIn = recvfrom(skt, buffer, 1024, 0, (sockaddr*)&client, &clientLength);
		if (bytesIn == SOCKET_ERROR)
		{
			std::cout << "ERROR RECEIVING MESSAGE FROM CLIENT - error: " << WSAGetLastError() << std::endl;
			continue;
		}
		char* clientIP = new char[256]();

		inet_ntop(AF_INET, &client.sin_addr, clientIP, 256);

		std::cout << "Message received from " << clientIP << ": " << buffer << std::endl;
	}

	closesocket(skt);

	//Shutdown
	WSACleanup();
	return 0;

}
