// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
//#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
//#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define P_POSITION 0001
#define B_POSITION 0002
#define GOAL 0003
#define MOVE_BALL 0004
#define RACKET_COLLIDE 0005

#define Player_R 30
//플레이어 에 대한 변수와 구조체 선언
struct Point2D
{
    float Position_x;
    float Position_y;
};

struct Accel2D
{
    float Accel_x;
    float Accel_y;
};
