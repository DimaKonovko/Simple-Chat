#define _CRT_SECURE_NO_WARNINGS
#define MAX_CONNECTIONS 2
#define BUF_LENGTH 256

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

#include <iostream>
#include <WinSock2.h>

using namespace std;

// working with clients
void clientHandler(int index);

SOCKET connections[MAX_CONNECTIONS] = { NULL };	// array of connections

int main()
{	
	int iResult = 0;

	//--------------------------------------------------------------------
	// Initialize winsock

	WORD wVersionRequested;
	wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;

	iResult = WSAStartup(wVersionRequested, &wsaData);
	if (iResult != 0) {
		cout << "WSAStartup failed with error: " << WSAGetLastError() << endl;
		system("pause");
		return 1;
	}	 

	//--------------------------------------------------------------------
	// Creating a socket fo listening

	SOCKET listenSocket;

	listenSocket = socket(AF_INET, SOCK_STREAM, NULL);
	if (listenSocket == INVALID_SOCKET) {
		cout << "socket failed with error: " << WSAGetLastError() << endl;
		system("pause");
		WSACleanup();
		return 1;
	}

	//--------------------------------------------------------------------
	// Initializing SOCKADDR structure

	SOCKADDR_IN serverAddr;
	int serverAddrSize = sizeof(serverAddr);

	cout << "Enter IP: ";
	char ip[15];
	cin >> ip;
	serverAddr.sin_addr.s_addr = inet_addr(ip);

	serverAddr.sin_family = AF_INET;

	int port;
	cout << "Enter port: ";
	cin >> port;
	serverAddr.sin_port = htons(port);

	//--------------------------------------------------------------------
	// Binding SOCKADDR structure with listening socket

	iResult = bind(listenSocket, (SOCKADDR *)&serverAddr, serverAddrSize);
	if (iResult == SOCKET_ERROR) {
		cout << "bind failed with error: " << WSAGetLastError() << endl;
		iResult = closesocket(listenSocket);
		if (iResult == SOCKET_ERROR) {
			cout << "closesocket failed with error: " << WSAGetLastError() << endl;
		}
		system("pause");
		WSACleanup();
		return 1;
	}
	cout << "Server has been successfully started" << endl;

	//--------------------------------------------------------------------
	// Listening
	
	iResult = listen(listenSocket, 0);
	if (iResult == SOCKET_ERROR) {
		cout << "listen failed with error: " << WSAGetLastError() << endl;
		iResult = closesocket(listenSocket);
		if (iResult == SOCKET_ERROR) {
			cout << "closesocket failed with error: " << WSAGetLastError() << endl;
		}
		system("pause");
		WSACleanup();
		return 1;
	}
	 
	//--------------------------------------------------------------------
	// Accepting clients

	SOCKET acceptSocket;

	cout << "waiting for incoming requests..." << endl;
	while (true) {
		acceptSocket = accept(listenSocket, (SOCKADDR*)&serverAddr, &serverAddrSize);
		if (acceptSocket == INVALID_SOCKET) {
			cout << "accept failed with error: " << WSAGetLastError() << endl;
			iResult = closesocket(listenSocket);
			if (iResult == SOCKET_ERROR) {
				cout << "closesocket failed with error: " << WSAGetLastError() << endl;
			}
			system("pause");
			WSACleanup();
			return 1;
		}
		cout << "Client has been successfully connected" << endl;

		//--------------------------------------------------------------------
		// Working with client

		char buf[BUF_LENGTH] = " Welcome to server!\n Server is ready to receive your messages\n\0";
		send(acceptSocket, buf, sizeof(buf), NULL);
		
		for (int i = 0; i < MAX_CONNECTIONS; i++) {
			if (connections[i] == NULL) {
				connections[i] = acceptSocket;
				// creating a thread for working with clients
				CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientHandler, (LPVOID)(i), NULL, NULL);
				break;
			}
		}
	}

	iResult = closesocket(listenSocket);
	if (iResult == SOCKET_ERROR) {
		cout << "closesocket failed with error: " << WSAGetLastError() << endl;
	}
	system("pause");   
	WSACleanup();
	return 0;
}



void clientHandler(int index)
{
	char buf[BUF_LENGTH];
	int iResult;
	while (true) {
		iResult = recv(connections[index], buf, sizeof(buf), NULL);	// receiving messages from client
		if (iResult == -1) {
			iResult = closesocket(connections[index]);
			if (iResult == SOCKET_ERROR) {
				cout << "closesocket failed with error: " << WSAGetLastError() << endl;
			}
			else {
				connections[index] = NULL;
				break;
			}
		}
		// sending messages to other clients
		for (int i = 0; i < MAX_CONNECTIONS; i++) {
			if (connections[i] != NULL)
				if (i != index) {
					send(connections[i], buf, sizeof(buf), NULL);
				}
		}
	}
	cout << "Client disconnected" << endl;
}
