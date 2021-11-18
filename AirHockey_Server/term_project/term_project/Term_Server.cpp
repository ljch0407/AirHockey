#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <CommCtrl.h>
#include "Server.h"

unsigned long clientIP[2];	//�� ������� ip �ּ�(net)

ClientInfo* cInfo;
SOCKET client_sock1, client_sock2;

Point2D pPosition[2];
Point2D bPosition;
Accel2D pAccel[2];
Accel2D bAccel;

float collideAngel;
int score;

bool Game_Start;
bool Ractket;
bool Game_end;
bool Connected1P, Connected2P;

int main() {

	//�ʱ�ȭ
	Connected1P = false;
	Connected2P = false;

	int retval;

	//���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	//socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	//bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	//listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	//������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	bool token = true;
	HANDLE getClientThread[2];
	HANDLE updateClientThread;
	ClientId* cNum;	//id�� Ŭ���̾�Ʈ ������ ���� ������ ����ü
	int id;				//Ŭ���̾�Ʈ id

	while (1)
	{
		//accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		if (token)	//Ŭ���̾�Ʈ 1�� ���� ������ ������
		{
			cNum->client_id = 0;
			cNum->client_sock = client_sock;

			//Ŭ���̾�Ʈ1�� �����͸� �޴� ������
			getClientThread[0] = CreateThread(nullptr, 0, getClient, (LPVOID)cNum, 0, nullptr);
			token = !token;
		}
		else
		{
			cNum->client_id = 1;
			cNum->client_sock = client_sock;

			//Ŭ���̾�Ʈ2�� �����͸� �޴� ������
			getClientThread[1] = CreateThread(nullptr, 0, getClient, (LPVOID)client_sock, 0, nullptr);
		}

		if (Game_Start)
		{
			//���� ������Ʈ + ������ �����ϴ� ������ -> Eventó���� ���� sendcommand()�� gc1,2�� ���Ḧ ��ٸ� 
			HANDLE getClientThread = CreateThread(nullptr, 0, updateClient, (LPVOID)cInfo, 0, nullptr);
		}
	}

	//closesocket()
	closesocket(listen_sock);

	//���� ����
	WSACleanup();
	return 0;
}

//Client�� ������ ����
DWORD WINAPI getClient(LPVOID arg)
{
	int id;
	SOCKADDR_IN clientAddr;
	int addrLen;
	char buf[BUFSIZE];
	ClientId* argInfo;

	//event����

	//Ŭ���̾�Ʈ ��ȣ ó��(�� Ŭ���̾�Ʈ ���� ����)
	argInfo = (ClientId*)arg;
	id = argInfo->client_id;

	if (id == 0)
	{
		client_sock1 = argInfo->client_sock;
		cInfo->client1 = client_sock1;
		Connected1P = true;
	}
	else
	{
		client_sock2 = argInfo->client_sock;
		cInfo->client2 = client_sock2;
		Connected2P = true;
	}

	//getpeername
	addrLen = sizeof(clientAddr);
	getpeername(argInfo->client_sock, (SOCKADDR*)&clientAddr, &addrLen);

	//data-recving
	while (1)
	{
		//declare additional-needed data

		//������ ����
		recvCommand();

		//�ڷ� ������Ʈ(�ǹ� ������ ���ص� ��)
		//update player position -> for 2
		//update player 

		//�̺�Ʈ Ȱ��ȭ �� send() �̺�Ʈ ���� ���

	}

}

//���� ������Ʈ + �� Ŭ���̾�Ʈ���� ������ ����
DWORD WINAPI updateClient(LPVOID arg)
{
	//server update
	bool Allconnected = checkAllConnected();

	//getclient ���� ���

	//sendCommand()

	//eventȰ��ȭ
}


//void recvIP(SOCKADDR* clientAddr, int id)	//(SOCKADDR *)�� ����ȯ�ؼ� ���ڷ� �Ѱܾ� �ϳ�?
//{
//	//������ Ŭ���̾�Ʈ�� ���� ���
//	//DisplayText("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ ��ȣ = %d\r\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
//	//if (id == 0)
//	//	clientAddr[0] = inet_ntoa(clientAddr)
//}

//�÷��̾ ��� ����Ǿ��ٸ� GameStart ������ Ŭ�󿡰� ����
bool checkAllConnected()
{
	if (Connected1P && Connected2P)
		return true;
	else
		return false;
}

//���� ���ӵ��� 0 �̻����� üũ
bool checkMoveBall()
{
	if ((bAccel.accel_x == 0) && (bAccel.accel_y))
		return false;
	else
		return true;
}

//�� ��ġ�� ���ӵ��� �ʱ�ȭ
void resetBall()
{
	bAccel.accel_x = 0;
	bAccel.accel_y = 0;
	bPosition.position_x = 200;
	bPosition.position_y = 400;
}

//���� ��ġ�� Goal������ �Ѿ����� üũ
void checkGoal()
{
	bool P1Goal = false;
	bool P2Goal = false;

	//check goal line
	if (bPosition.position_x >= 170 && bPosition.position_y <= 230)
	{
		if (bPosition.position_y >= 0 && bPosition.position_y <= 50)
		{
			resetBall();
			if (score / 10 == 9)
				score = 100;
			else score += 10;
		}
		else if (bPosition.position_y >= 750 && bPosition.position_y < 800)
		{
			resetBall();
			if (score % 10 == 9)
				score = 100;
			else score += 1;
		}
	}
}

//���ھ Ȯ���ؼ� ������ ���Ῡ�� ����
void checkScore()
{
	if (score == 100)
		Game_end = true;
}

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(nullptr, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

//���� �Լ� ���� ���
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(nullptr, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

//����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}