#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include "Server.h"

Point2D pPosition[2];
Point2D bPosition;
Accel2D bAccel;

int score;
bool P1Goal, P2Goal;

HANDLE recvData[2], updateData[2];

int main() {

	//�ʱ�ȭ
	bPosition.position_x = 200;
	bPosition.position_y = 400;

	pPosition[1].position_x = 400;
	pPosition[1].position_y = 200;

	recvData[0] = CreateEvent(nullptr, false, false, nullptr);
	recvData[1] = CreateEvent(nullptr, false, false, nullptr);
	updateData[0] = CreateEvent(nullptr, false, true, nullptr);
	updateData[1] = CreateEvent(nullptr, false, true, nullptr);

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
	HANDLE updateClientThread[2];
	ClientId cNum, cNum2;	//id�� Ŭ���̾�Ʈ ������ ���� ������ ����ü

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
			updateClientThread[0] = CreateThread(nullptr, 0, updateClient, (LPVOID)cTemp, 0, nullptr);
			token = !token;
		}
		else
		{
			cNum2.client_id = 1;
			cNum2.client_sock = client_sock;

			ClientId* cTemp;
			cTemp = &cNum2;

			//Ŭ���̾�Ʈ2�� �����͸� �޴� ������
			getClientThread[1] = CreateThread(nullptr, 0, getClient, (LPVOID)cTemp, 0, nullptr);
			updateClientThread[1] = CreateThread(nullptr, 0, updateClient, (LPVOID)cTemp, 0, nullptr);
		}

	}

	//closesocket()
	closesocket(listen_sock);

	//���� ����
	WSACleanup();

	//�̺�Ʈ ����
	CloseHandle(recvData[0]);
	CloseHandle(updateData[0]);
	CloseHandle(recvData[1]);
	CloseHandle(updateData[1]);

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

	//Ŭ���̾�Ʈ ��ȣ ó��(�� Ŭ���̾�Ʈ ���� ����)
	argInfo = (ClientId*)arg;
	id = argInfo->client_id;

	//Ŭ���̾�Ʈ ó��

	//if (id == 0)
	//	printf("Client1 Connected\n");
	//else
	//	printf("Client2 Connected\n");

	//getpeername
	addrLen = sizeof(clientAddr);
	getpeername(argInfo->client_sock, (SOCKADDR*)&clientAddr, &addrLen);

	//printf("data ���� ����\n");
	//data-recving
	while (1)
	{
		//declare additional-needed data

		//update �̺�Ʈ ���
		if (id == 0)
			WaitForSingleObject(updateData[0], INFINITE);
		else
			WaitForSingleObject(updateData[1], INFINITE);


		//��� ������ ����
		//recvCommand(header);
		retval = recvn(argInfo->client_sock, buf, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
		}

		header = atoi(buf);

		//if (id == 0)
		//	printf("[TCP Ŭ���̾�Ʈ1] ��� ���� �Ϸ�: %d\n", header);
		//else
		//	printf("[TCP Ŭ���̾�Ʈ2] ��� ���� �Ϸ�: %d\n", header);

		//����� �б�
		//header switch
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
				//printf("[TCP Ŭ���̾�Ʈ1 ���� ����] pPosition.x : %d, pPosition.y : %d\n", pPosition[0].position_x, pPosition[0].position_y);
			}
			else
			{
				pPosition[1].position_x = temp->position_x;
				pPosition[1].position_y = temp->position_y;
				//printf("[TCP Ŭ���̾�Ʈ2 ���� ����] pPosition.x : %d, pPosition.y : %d\n", pPosition[1].position_x, pPosition[1].position_y);
			}

			break;

		case RACKET_COLLIDE:
			//�浹 ������ ����(Accel)
			//������ ������ ����
			retval = recv(argInfo->client_sock, buf, sizeof(Accel2D), 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
			}

			Accel2D* tempAccel;
			tempAccel = (Accel2D*)buf;

			bAccel.accel_x = tempAccel->accel_x / 4000;
			bAccel.accel_y = tempAccel->accel_y / 4000;

			break;
		}

		//�̺�Ʈ Ȱ��ȭ
		if (id == 0)
			SetEvent(recvData[0]);
		else
			SetEvent(recvData[1]);
	}
}

//���� ������Ʈ + �� Ŭ���̾�Ʈ���� ������ ����
DWORD WINAPI updateClient(LPVOID arg)
{
	printf("update ����\n");

	int id, retval, header;
	SOCKADDR_IN clientAddr;
	int addrLen;
	char buf[BUFSIZE];
	ClientId* argInfo;

	//Ŭ���̾�Ʈ ��ȣ ó��(�� Ŭ���̾�Ʈ ���� ����)
	argInfo = (ClientId*)arg;
	id = argInfo->client_id;

	//getpeername
	addrLen = sizeof(clientAddr);
	getpeername(argInfo->client_sock, (SOCKADDR*)&clientAddr, &addrLen);
	
	//sendCommand()
	//printf("data �۽� ����\n");

	while (1)
	{
		//getclient ���� ���
		if (id == 0)
			WaitForSingleObject(recvData[0], INFINITE);
		else
			WaitForSingleObject(recvData[1], INFINITE);


		//������Ʈ
		if (id == 0)
		{
			bPosition = updateBall(bAccel);

			checkGoal();

		}

		if (P1Goal || P2Goal)
			snprintf(buf, sizeof(buf), "%d", GOAL);
		else
			snprintf(buf, sizeof(buf), "%d", B_POSITION);

		header = atoi(buf);

		switch (header)
		{
		case GOAL:
			printf("[�� ����] ���� ���ھ�: %d\n", score);
			//��� ����
			retval = send(argInfo->client_sock, buf, sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			}

			//���� ������ ����
			retval = send(argInfo->client_sock, (char*)&score, sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			}

			break;

		case B_POSITION:

			//��� ���� ����
			retval = send(argInfo->client_sock, buf, sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			}

			//printf("��� ���� �Ϸ�\n");

			Point2D temp;
			if (id == 0)
			{
				temp.position_x = bPosition.position_x;
				temp.position_y = bPosition.position_y;
			}
			else
			{
				temp.position_x = 385 - bPosition.position_x;
				temp.position_y = 710 - bPosition.position_y;
			}
			
			//�� ������ ����
			retval = send(argInfo->client_sock, (char*)&temp, sizeof(Point2D), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			}

			//printf("�� ������ ���� �Ϸ�: position_x: %d, position_y: %d\n", temp.position_x, temp.position_y);


			//�÷��̾� ������ ����
			if (id == 0)
			{
				temp.position_x = pPosition[1].position_x;
				temp.position_y = pPosition[1].position_y;
				printf("player1: %f, %f\n", temp.position_x, temp.position_y);

				retval = send(argInfo->client_sock, (char*)&temp, sizeof(Point2D), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
				}

				//printf("�÷��̾�2 ������ ���� �Ϸ�: position_x: %d, position_y: %d\n", temp.position_x, temp.position_y);
			}
			else
			{
				temp.position_x = pPosition[0].position_x;
				temp.position_y = pPosition[0].position_y;

				retval = send(argInfo->client_sock, (char*)&temp, sizeof(Point2D), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
				}

				//printf("�÷��̾�1 ������ ���� �Ϸ�: position_x: %d, position_y: %d\n", temp.position_x, temp.position_y);
			}

			break;
		}

		//eventȰ��ȭ
		if (id == 0)
			SetEvent(updateData[0]);
		else
			SetEvent(updateData[1]);
	}

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
	P1Goal = false;
	P2Goal = false;

	//check goal line
	if (bPosition.position_x >= 170 && bPosition.position_y <= 230)
	{
		if (bPosition.position_y >= 0 && bPosition.position_y <= 50)
		{
			resetBall();
			P2Goal = true;
			if (score / 10 == 9)
				score = 100;
			else score += 10;
		}
		else if (bPosition.position_y >= 650 && bPosition.position_y < 720)
		{
			resetBall();
			P1Goal = true;
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