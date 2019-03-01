#define _CRT_SECURE_NO_WARNINGS
#define BUF_LENGTH 256

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

#include <iostream>
#include <WinSock2.h>
 
using namespace std;

void clientHandler(void);

SOCKET clientSocket;	//����� �������, ������������ ��� ������������ ���������� � ��������

int main()
{
	int iResult = 0;	//����������, ��������������� ��� �������� ����������� ������ �������

	//---------------------------------------------------------------------
	// ����������� ����������

	WORD wVersionRequested;
	wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;

	iResult = WSAStartup(wVersionRequested, &wsaData);
	if (iResult != 0) {
		cout << "WSAStartup failed with error: " << WSAGetLastError() << endl;
		system("pause");
		return 1;
	}

	//---------------------------------------------------------------------
	// �������� ������ �������

	clientSocket = socket(AF_INET, SOCK_STREAM, NULL);
	if (clientSocket == INVALID_SOCKET) {
		cout << "socket failed with error: " << WSAGetLastError() << endl;
		system("pause");
		WSACleanup();
		return 1;
	}

	//-------------------- -------------------------------------------------
	// �������������� ������ � ����� �������

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

	//---------------------------------------------------------------------
	// ������ ������� �� ������������ ���������� � ��������

	iResult = connect(clientSocket, (SOCKADDR *)&serverAddr, serverAddrSize);
	if (iResult != 0) {
		cout << "connect failed with error: " << WSAGetLastError() << endl;
		iResult = closesocket(clientSocket);
		if (iResult == SOCKET_ERROR) {
			cout << "closesocket failed with error: " << WSAGetLastError() << endl;
		}
		system("pause");
		WSACleanup();
		return 1;
	}

	//---------------------------------------------------------------------
	// �������������� ������� � ��������

	char buf[BUF_LENGTH];
	int choice;

	recv(clientSocket, buf, sizeof(buf), NULL);
	cout << buf << endl;

	// �������� ������ ��� �������� ��������� �� �������
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientHandler, NULL, NULL, NULL);

	while (true) {
		cin.getline(buf, sizeof(buf));
		send(clientSocket, buf, sizeof(buf), NULL);
		Sleep(10);
	}


	closesocket(clientSocket);
	iResult = closesocket(clientSocket);
	if (iResult == SOCKET_ERROR) {
		cout << "closesocket failed with error: " << WSAGetLastError() << endl;
	}
	system("pause");
	WSACleanup();
	return 0;
}



void clientHandler(void)
{
	char receivedMsg[BUF_LENGTH];
	while (true) {
		recv(clientSocket, receivedMsg, sizeof(receivedMsg), NULL);
		cout << "        -" << receivedMsg << endl;
	}
}