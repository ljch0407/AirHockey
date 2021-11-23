#pragma once

#define SERVERPORT 9000
#define BUFSIZE 1024
#define BALLSIZE 30
#define XSIDE_COLLIDE 11
#define YSIDE_COLLIDE 22

#define P_POSITION 0001
#define B_POSITION 0002
#define GOAL 0003
#define MOVE_BALL 0004
#define RACKET_COLLIDE 0005

//클라이언트와 해당 클라이언트의 식별자(ID)를 묶은 구조체
struct ClientId {
	SOCKET client_sock;
	int client_id;
};

//양 클라이언트를 묶은 구조체
struct ClientInfo {
	SOCKET client1;
	SOCKET client2;
};

//객체의 2차원 좌표
struct Point2D {
	float position_x;
	float position_y;
};

//객체의 2차원 가속도
struct Accel2D {
	float accel_x;
	float accel_y;
};

DWORD WINAPI updateClient(LPVOID arg);		//Server내부 업데이트 + 데이터 송신
DWORD WINAPI getClient(LPVOID arg);			//Client의 데이터 수신

void recvCommand(SOCKET* client_sock);		//헤더 + 데이터 수신 후 분기 처리
void sendCommand();							//헤더 + 데이터 송신

void err_quit(const char* msg);
void err_display(const char* msg);

//받은 ip 주소 변환하여 저장 -> 단기 폐기
//void recvIP(SOCKADDR* clientAddr);

//플레이어가 모두 연결되었다면 GameStart 변수를 클라에게 전송
void checkAllConnected();

//공의 가속도가 0 이상인지 체크
bool checkMoveBall();

//공의 위치가 Goal라인을 넘었는지 체크
void checkGoal();

//스코어를 확인해서 게임의 종료여부 결정
void checkScore();

//공 위치와 가속도를 초기화
void resetBall();

//새로운 공의 위치와 가속도 계산
Point2D updateBall(Accel2D Accel);

//공과 서킷의 충돌로 인한 공의 가속도 재계산
Accel2D circuitCollide(Accel2D Accel, int collideType);

//공과 라켓의 충돌 처리로 인한 공의 가속도 재계산
Accel2D racketCollide(Accel2D Accel);