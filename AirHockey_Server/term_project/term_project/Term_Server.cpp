#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <CommCtrl.h>
#include "Server.h"

unsigned long clientIP[2];	//두 사용자의 ip 주소(net)

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

	//초기화
	Connected1P = false;
	Connected2P = false;

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
	HANDLE updateClientThread;
	ClientId* cNum;	//id와 클라이언트 정보를 같이 관리할 구조체
	int id;				//클라이언트 id

	while (1)
	{
		//accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		if (token)	//클라이언트 1의 연결 설정을 받으면
		{
			cNum->client_id = 0;
			cNum->client_sock = client_sock;

			//클라이언트1의 데이터를 받는 스레드
			getClientThread[0] = CreateThread(nullptr, 0, getClient, (LPVOID)cNum, 0, nullptr);
			token = !token;
		}
		else
		{
			cNum->client_id = 1;
			cNum->client_sock = client_sock;

			//클라이언트2의 데이터를 받는 스레드
			getClientThread[1] = CreateThread(nullptr, 0, getClient, (LPVOID)client_sock, 0, nullptr);
		}

		if (Game_Start)
		{
			//서버 업데이트 + 데이터 전송하는 스레드 -> Event처리를 통해 sendcommand()는 gc1,2의 종료를 기다림 
			HANDLE getClientThread = CreateThread(nullptr, 0, updateClient, (LPVOID)cInfo, 0, nullptr);
		}
	}

	//closesocket()
	closesocket(listen_sock);

	//윈속 종료
	WSACleanup();
	return 0;
}

//Client의 데이터 수신
DWORD WINAPI getClient(LPVOID arg)
{
	int id;
	SOCKADDR_IN clientAddr;
	int addrLen;
	char buf[BUFSIZE];
	ClientId* argInfo;

	//event생성

	//클라이언트 번호 처리(각 클라이언트 정보 구분)
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

		//데이터 수신
		recvCommand();

		//자료 업데이트(의미 없으면 안해도 됨)
		//update player position -> for 2
		//update player 

		//이벤트 활성화 후 send() 이벤트 종료 대기

	}

}

//서버 업데이트 + 양 클라이언트에게 데이터 전송
DWORD WINAPI updateClient(LPVOID arg)
{
	//server update
	bool Allconnected = checkAllConnected();

	//getclient 종료 대기

	//sendCommand()

	//event활성화
}


//void recvIP(SOCKADDR* clientAddr, int id)	//(SOCKADDR *)로 형변환해서 인자로 넘겨야 하나?
//{
//	//접속한 클라이언트의 정보 출력
//	//DisplayText("[TCP 서버] 클라이언트 접속: IP 주소 = %s, 포트 번호 = %d\r\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
//	//if (id == 0)
//	//	clientAddr[0] = inet_ntoa(clientAddr)
//}

//플레이어가 모두 연결되었다면 GameStart 변수를 클라에게 전송
bool checkAllConnected()
{
	if (Connected1P && Connected2P)
		return true;
	else
		return false;
}

//공의 가속도가 0 이상인지 체크
bool checkMoveBall()
{
	if ((bAccel.accel_x == 0) && (bAccel.accel_y))
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

//스코어를 확인해서 게임의 종료여부 결정
void checkScore()
{
	if (score == 100)
		Game_end = true;
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