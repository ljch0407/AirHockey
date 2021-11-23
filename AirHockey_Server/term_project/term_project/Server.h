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

//Ŭ���̾�Ʈ�� �ش� Ŭ���̾�Ʈ�� �ĺ���(ID)�� ���� ����ü
struct ClientId {
	SOCKET client_sock;
	int client_id;
};

//�� Ŭ���̾�Ʈ�� ���� ����ü
struct ClientInfo {
	SOCKET client1;
	SOCKET client2;
};

//��ü�� 2���� ��ǥ
struct Point2D {
	float position_x;
	float position_y;
};

//��ü�� 2���� ���ӵ�
struct Accel2D {
	float accel_x;
	float accel_y;
};

DWORD WINAPI updateClient(LPVOID arg);		//Server���� ������Ʈ + ������ �۽�
DWORD WINAPI getClient(LPVOID arg);			//Client�� ������ ����

void recvCommand(SOCKET* client_sock);		//��� + ������ ���� �� �б� ó��
void sendCommand();							//��� + ������ �۽�

void err_quit(const char* msg);
void err_display(const char* msg);

//���� ip �ּ� ��ȯ�Ͽ� ���� -> �ܱ� ���
//void recvIP(SOCKADDR* clientAddr);

//�÷��̾ ��� ����Ǿ��ٸ� GameStart ������ Ŭ�󿡰� ����
void checkAllConnected();

//���� ���ӵ��� 0 �̻����� üũ
bool checkMoveBall();

//���� ��ġ�� Goal������ �Ѿ����� üũ
void checkGoal();

//���ھ Ȯ���ؼ� ������ ���Ῡ�� ����
void checkScore();

//�� ��ġ�� ���ӵ��� �ʱ�ȭ
void resetBall();

//���ο� ���� ��ġ�� ���ӵ� ���
Point2D updateBall(Accel2D Accel);

//���� ��Ŷ�� �浹�� ���� ���� ���ӵ� ����
Accel2D circuitCollide(Accel2D Accel, int collideType);

//���� ������ �浹 ó���� ���� ���� ���ӵ� ����
Accel2D racketCollide(Accel2D Accel);