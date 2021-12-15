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

	//초기화
	bPosition.position_x = 200;
	bPosition.position_y = 400;

	pPosition[1].position_x = 400;
	pPosition[1].position_y = 200;

	recvData[0] = CreateEvent(nullptr, false, false, nullptr);
	recvData[1] = CreateEvent(nullptr, false, false, nullptr);
	updateData[0] = CreateEvent(nullptr, false, true, nullptr);
	updateData[1] = CreateEvent(nullptr, false, true, nullptr);

	int retval;

	//윈속 초기화
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

	//데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	bool token = true;
	HANDLE getClientThread[2];
	HANDLE updateClientThread[2];
	ClientId cNum, cNum2;	//id와 클라이언트 정보를 같이 관리할 구조체

	while (1)
	{
		//accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		printf("[TCP 서버] 클라이언트 접속: IP 주소 = %s, 포트 번호 = %d\r\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		if (token)	//클라이언트 1의 연결 설정을 받으면
		{
			cNum.client_id = 0;
			cNum.client_sock = client_sock;

			ClientId* cTemp;
			cTemp = &cNum;

			//클라이언트1의 데이터를 받는 스레드
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

			//클라이언트2의 데이터를 받는 스레드
			getClientThread[1] = CreateThread(nullptr, 0, getClient, (LPVOID)cTemp, 0, nullptr);
			updateClientThread[1] = CreateThread(nullptr, 0, updateClient, (LPVOID)cTemp, 0, nullptr);
		}

	}

	//closesocket()
	closesocket(listen_sock);

	//윈속 종료
	WSACleanup();

	//이벤트 제거
	CloseHandle(recvData[0]);
	CloseHandle(updateData[0]);
	CloseHandle(recvData[1]);
	CloseHandle(updateData[1]);

	return 0;
}

//Client의 데이터 수신
DWORD WINAPI getClient(LPVOID arg)
{
	int id, retval, header;
	SOCKADDR_IN clientAddr;
	int addrLen;
	char buf[BUFSIZE];
	ClientId* argInfo;

	//클라이언트 번호 처리(각 클라이언트 정보 구분)
	argInfo = (ClientId*)arg;
	id = argInfo->client_id;

	//클라이언트 처리

	//if (id == 0)
	//	printf("Client1 Connected\n");
	//else
	//	printf("Client2 Connected\n");

	//getpeername
	addrLen = sizeof(clientAddr);
	getpeername(argInfo->client_sock, (SOCKADDR*)&clientAddr, &addrLen);

	//printf("data 수신 시작\n");
	//data-recving
	while (1)
	{
		//declare additional-needed data

		//update 이벤트 대기
		if (id == 0)
			WaitForSingleObject(updateData[0], INFINITE);
		else
			WaitForSingleObject(updateData[1], INFINITE);


		//헤더 데이터 수신
		//recvCommand(header);
		retval = recvn(argInfo->client_sock, buf, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
		}

		header = atoi(buf);

		//if (id == 0)
		//	printf("[TCP 클라이언트1] 헤더 수신 완료: %d\n", header);
		//else
		//	printf("[TCP 클라이언트2] 헤더 수신 완료: %d\n", header);

		//헤더별 분기
		//header switch
		switch (header)
		{
		case P_POSITION:
			//포지션 데이터 수신
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
				//printf("[TCP 클라이언트1 수신 정보] pPosition.x : %d, pPosition.y : %d\n", pPosition[0].position_x, pPosition[0].position_y);
			}
			else
			{
				pPosition[1].position_x = temp->position_x;
				pPosition[1].position_y = temp->position_y;
				//printf("[TCP 클라이언트2 수신 정보] pPosition.x : %d, pPosition.y : %d\n", pPosition[1].position_x, pPosition[1].position_y);
			}

			break;

		case RACKET_COLLIDE:
			//충돌 데이터 수신(Accel)
			//포지션 데이터 수신
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

		//이벤트 활성화
		if (id == 0)
			SetEvent(recvData[0]);
		else
			SetEvent(recvData[1]);
	}
}

//서버 업데이트 + 양 클라이언트에게 데이터 전송
DWORD WINAPI updateClient(LPVOID arg)
{
	printf("update 실행\n");

	int id, retval, header;
	SOCKADDR_IN clientAddr;
	int addrLen;
	char buf[BUFSIZE];
	ClientId* argInfo;

	//클라이언트 번호 처리(각 클라이언트 정보 구분)
	argInfo = (ClientId*)arg;
	id = argInfo->client_id;

	//getpeername
	addrLen = sizeof(clientAddr);
	getpeername(argInfo->client_sock, (SOCKADDR*)&clientAddr, &addrLen);
	
	//sendCommand()
	//printf("data 송신 시작\n");

	while (1)
	{
		//getclient 종료 대기
		if (id == 0)
			WaitForSingleObject(recvData[0], INFINITE);
		else
			WaitForSingleObject(recvData[1], INFINITE);


		//업데이트
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
			printf("[골 정보] 현재 스코어: %d\n", score);
			//헤더 전송
			retval = send(argInfo->client_sock, buf, sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			}

			//점수 데이터 전송
			retval = send(argInfo->client_sock, (char*)&score, sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			}

			break;

		case B_POSITION:

			//헤더 파일 전송
			retval = send(argInfo->client_sock, buf, sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			}

			//printf("헤더 전송 완료\n");

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
			
			//공 데이터 전송
			retval = send(argInfo->client_sock, (char*)&temp, sizeof(Point2D), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			}

			//printf("공 데이터 전송 완료: position_x: %d, position_y: %d\n", temp.position_x, temp.position_y);


			//플레이어 데이터 전송
			if (id == 0)
			{
				temp.position_x = pPosition[1].position_x;
				temp.position_y = pPosition[1].position_y;
				printf("player1: %f, %f\n", temp.position_x, temp.position_y);

				retval = send(argInfo->client_sock, (char*)&temp, sizeof(Point2D), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
				}

				//printf("플레이어2 데이터 전송 완료: position_x: %d, position_y: %d\n", temp.position_x, temp.position_y);
			}
			else
			{
				temp.position_x = pPosition[0].position_x;
				temp.position_y = pPosition[0].position_y;

				retval = send(argInfo->client_sock, (char*)&temp, sizeof(Point2D), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
				}

				//printf("플레이어1 데이터 전송 완료: position_x: %d, position_y: %d\n", temp.position_x, temp.position_y);
			}

			break;
		}

		//event활성화
		if (id == 0)
			SetEvent(updateData[0]);
		else
			SetEvent(updateData[1]);
	}

}

//공의 가속도가 0 이상인지 체크
bool checkMoveBall()
{
	if ((bAccel.accel_x == 0) && (bAccel.accel_y == 0))
		return false;
	else
		return true;
}

//공 위치와 가속도를 초기화
void resetBall()
{
	bAccel.accel_x = 0;
	bAccel.accel_y = 0;
	bPosition.position_x = 200;
	bPosition.position_y = 400;
}

//공의 위치가 Goal라인을 넘었는지 체크
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

//새로운 공의 위치와 가속도 계산
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

//공과 서킷의 충돌로 인한 공의 가속도 재계산
Accel2D circuitCollide(Accel2D Accel, int collideType)
{
	Accel2D newAccel = Accel;

	if (collideType == XSIDE_COLLIDE)
		newAccel.accel_x = -newAccel.accel_x;
	else if (collideType == YSIDE_COLLIDE)
		newAccel.accel_y = -newAccel.accel_y;

	return newAccel;
}


// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(nullptr, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

//소켓 함수 오류 출력
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(nullptr, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

//사용자 정의 데이터 수신 함수
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