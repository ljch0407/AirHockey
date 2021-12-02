#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <CommCtrl.h>
#include "Server.h"

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

HANDLE recvData, updateData;

int main() {

	//�ʱ�ȭ
	Connected1P = false;
	Connected2P = false;

	bPosition.position_x = 200;
	bPosition.position_y = 400;

	recvData = CreateEvent(nullptr, false, false, nullptr);
	updateData = CreateEvent(nullptr, false, true, nullptr);

	bAccel.accel_x = 10;
	bAccel.accel_y = 10;

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
	ClientId cNum;	//id�� Ŭ���̾�Ʈ ������ ���� ������ ����ü
	//int id;					//Ŭ���̾�Ʈ id

	while (1)
	{
		//accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ ��ȣ = %d\r\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		if (token)	//Ŭ���̾�Ʈ 1�� ���� ������ ������
		{
			cNum.client_id = 0;
			cNum.client_sock = client_sock;

			ClientId* cTemp;
			cTemp = &cNum;

			//Ŭ���̾�Ʈ1�� �����͸� �޴� ������
			getClientThread[0] = CreateThread(nullptr, 0, getClient, (LPVOID)cTemp, 0, nullptr);
			token = !token;
		}
		//else
		//{
		//	cNum->client_id = 1;
		//	cNum->client_sock = client_sock;

		//	//Ŭ���̾�Ʈ2�� �����͸� �޴� ������
		//	getClientThread[1] = CreateThread(nullptr, 0, getClient, (LPVOID)client_sock, 0, nullptr);
		//}

		while (1)
		{
			if (Game_Start)
			{
				//���� ������Ʈ + ������ �����ϴ� ������ -> Eventó���� ���� sendcommand()�� gc1,2�� ���Ḧ ��ٸ� 
				updateClientThread = CreateThread(nullptr, 0, updateClient, (LPVOID)cInfo, 0, nullptr);
				Game_Start = false;
				break;
			}
		}
	}

	//closesocket()
	closesocket(listen_sock);

	//���� ����
	WSACleanup();

	//�̺�Ʈ ����
	CloseHandle(recvData);
	CloseHandle(updateData);

	return 0;
}

//Client�� ������ ����
DWORD WINAPI getClient(LPVOID arg)
{
	int id, retval, header;
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
		ClientInfo cInfoTemp;
		cInfoTemp.client1 = client_sock1;
		cInfo = &cInfoTemp;
		Connected1P = true;
		Game_Start = true;
		printf("Client1 Connected\n");
	}
	else
	{
		client_sock2 = argInfo->client_sock;
		cInfo->client2 = client_sock2;
		Connected2P = true;
	}

	//if (!Game_Start)
		//checkAllConnected();

	//getpeername
	addrLen = sizeof(clientAddr);
	getpeername(argInfo->client_sock, (SOCKADDR*)&clientAddr, &addrLen);

	printf("data ���� ����\n");
	//data-recving
	while (1)
	{
		//declare additional-needed data

		//update �̺�Ʈ ���
		WaitForSingleObject(updateData, INFINITE);

		//��� ������ ����
		//recvCommand(header);
		retval = recvn(argInfo->client_sock, buf, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
		}
		
		header = atoi(buf);
		printf("��� ���� �Ϸ�: %d\n", header);

		//����� �б�
		//header switch
		header = atoi(buf);
		switch (header)
		{
		case P_POSITION:
			//������ ������ ����
			//position data recv
			retval = recvn(argInfo->client_sock, buf, sizeof(Point2D), 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
			}
			
			Point2D* temp;
			temp = (Point2D*)buf;

			if (id == 0)
			{
				pPosition[0].position_x = temp->position_x;
				pPosition[0].position_y = temp->position_y;
				printf("[TCP Ŭ���̾�Ʈ ���� ����] pPosition.x : %d, pPosition.y : %d\n", pPosition[0].position_x, pPosition[0].position_y);
			}
			else
			{
				//pPosition[1].position_x = temp->position_x;
				//pPosition[1].position_y = temp->position_y;
			}
			break;
		//case RACKET_COLLIDE:
		//	//�浹 ������ ����(Position, Accel, Angle)
		//	//������ ������ ����
		//	retval = recv(argInfo->client_sock, buf, BUFSIZE, 0);
		//	if (retval == SOCKET_ERROR) {
		//		err_display("recv()");
		//	}
		//	Point2D* temp3;
		//	temp = (Point2D*)buf;
		//	if (id == 0)
		//	{
		//		pPosition[0].position_x = temp3->position_x;
		//		pPosition[0].position_y = temp3->position_y;
		//	}
		//	else
		//	{
		//		pPosition[1].position_x = temp3->position_x;
		//		pPosition[1].position_y = temp3->position_y;
		//	}

		//	//���ӵ� ������ ����
		//	retval = recv(argInfo->client_sock, buf, BUFSIZE, 0);
		//	if (retval == SOCKET_ERROR) {
		//		err_display("recv()");
		//	}
		//	Accel2D* temp2;
		//	temp2 = (Accel2D*)buf;
		//	if (id == 0)
		//	{
		//		pAccel[0].accel_x = temp2->accel_x;
		//		pAccel[0].accel_y = temp2->accel_y;
		//	}
		//	else
		//	{
		//		pAccel[1].accel_x = temp2->accel_x;
		//		pAccel[1].accel_y = temp2->accel_y;
		//	}
		//	//�� ������ ����
		//	/*retval = recv(argInfo->client_sock, buf, BUFSIZE, 0);
		//	if (retval == SOCKET_ERROR) {
		//		err_display("recv()");
		//	}
		//	collideAngel = atoi(buf);
		//	*///�߰� -> �̰� 3���� �浹 ����ü�� ��� �ѹ��� ��/�����ϴ� ����� ���?
		//	break;
		}

		//�̺�Ʈ Ȱ��ȭ
		SetEvent(recvData);

		//�ڷ� ������Ʈ(�ǹ� ������ ���ص� ��)
		//update player position -> for 2
		//update player 


	}

}

//���� ������Ʈ + �� Ŭ���̾�Ʈ���� ������ ����
DWORD WINAPI updateClient(LPVOID arg)
{
	printf("update ����\n");

	//Ŭ���̾�Ʈ ��ȣ ó��(�� Ŭ���̾�Ʈ ���� ����)

	ClientInfo* tempinfo;
	tempinfo = (ClientInfo*)arg;

	SOCKADDR_IN clientAddr;
	int addrLen;
	SOCKET client_sock1;
	int retval;
	char buf[BUFSIZE];

	client_sock1 = tempinfo->client1;

	//getpeername
	addrLen = sizeof(clientAddr);
	getpeername(client_sock1, (SOCKADDR*)&clientAddr, &addrLen);
	
	//server update
	

	//bPosition = updateBall(bAccel);

	//sendCommand()
	printf("data �۽� ����\n");

	while (1)
	{
		//getclient ���� ���
		WaitForSingleObject(recvData, INFINITE);

		//��� ���� ����
		snprintf(buf, sizeof(buf), "%d", B_POSITION);

		retval = send(client_sock1, buf, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}

		printf("��� ���� �Ϸ�\n");

		//�� ������Ʈ
		bPosition = updateBall(bAccel);

		Point2D temp;
		temp.position_x = bPosition.position_x;
		temp.position_y = bPosition.position_y;

		retval = send(client_sock1, (char*)&temp, sizeof(Point2D), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}

		printf("������ ���� �Ϸ�: position_x: %d, position_y: %d\n", temp.position_x, temp.position_y);

		//eventȰ��ȭ
		SetEvent(updateData);
	}

	//if (Allconnected)
		//send All-Connected
	
	//if (!checkMoveBall())
		//send STRIKE-Effect

	//if (Game_end)
		//send Game-End


}

void recvCommand(SOCKET* client_sock)
{
}


//void recvIP(SOCKADDR* clientAddr, int id)	//(SOCKADDR *)�� ����ȯ�ؼ� ���ڷ� �Ѱܾ� �ϳ�?
//{
//	//������ Ŭ���̾�Ʈ�� ���� ���
//	//DisplayText("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ ��ȣ = %d\r\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
//	//if (id == 0)
//	//	clientAddr[0] = inet_ntoa(clientAddr)
//}

//�÷��̾ ��� ����Ǿ��ٸ� GameStart ������ Ŭ�󿡰� ����
void checkAllConnected()
{
	if (Connected1P && Connected2P)
		Game_Start = true;
}

//���� ���ӵ��� 0 �̻����� üũ
bool checkMoveBall()
{
	if ((bAccel.accel_x == 0) && (bAccel.accel_y == 0))
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

//���ο� ���� ��ġ�� ���ӵ� ���
Point2D updateBall(Accel2D Accel)
{
	Accel2D newAccel = Accel;

	if (bPosition.position_x + BALLSIZE > 400 && newAccel.accel_x > 0)
	{
		bPosition.position_x = 400 - BALLSIZE;
		bAccel = circuitCollide(newAccel, XSIDE_COLLIDE);
	}
	else if (bPosition.position_x - BALLSIZE < 0 && newAccel.accel_x < 0)
	{
		bPosition.position_x = BALLSIZE;
		bAccel = circuitCollide(newAccel, XSIDE_COLLIDE);
	}

	if (bPosition.position_y + BALLSIZE > 800 && newAccel.accel_y > 0)
	{
		bPosition.position_y = 800 - BALLSIZE;
		bAccel = circuitCollide(newAccel, YSIDE_COLLIDE);
	}
	else if (bPosition.position_y - BALLSIZE < 0 && newAccel.accel_y < 0)
	{
		bPosition.position_y = BALLSIZE;
		bAccel = circuitCollide(newAccel, YSIDE_COLLIDE);
	}

	bPosition.position_x += newAccel.accel_x;
	bPosition.position_y += newAccel.accel_y;

	return bPosition;
}

//���� ��Ŷ�� �浹�� ���� ���� ���ӵ� ����
Accel2D circuitCollide(Accel2D Accel, int collideType)
{
	Accel2D newAccel = Accel;

	if (collideType == XSIDE_COLLIDE)
		newAccel.accel_x = -newAccel.accel_x;
	else if (collideType == YSIDE_COLLIDE)
		newAccel.accel_y = -newAccel.accel_y;

	return newAccel;
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