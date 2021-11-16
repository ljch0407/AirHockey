#pragma once
#include "framework.h"
#include "AirHockey.h"
#include <gdiplus.h>

#include <WinSock2.h>
#include <stdio.h>
using namespace Gdiplus;

#pragma comment(lib,"Gdiplus.lib")

#define MAX_LOADSTRING 100
#define Player_R 30

#define SERVERIP "127.0.0.1"
#define SERVERPORT  9000
#define BUFSIZE 512

//플레이어 에 대한 변수와 구조체 선언
struct Point2D
{
    int Position_x;
    int Position_y;
};

struct Accel2D
{
    float Accel_x;
    float Accel_y;
};
