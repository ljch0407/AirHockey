// AirHockey.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "GameHeader.h"
#include <WinSock2.h>
#include <gdiplus.h>
#include <iostream>
#pragma comment(lib,"Gdiplus.lib")
#pragma comment(lib,"ws2_32")
using namespace Gdiplus;
#pragma warning(disable : 4996)


HANDLE recvData, updateData;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI Client(LPVOID arg);
DWORD WINAPI Update(LPVOID arg);

Player player(40, 60, 20, 20);
Player player2(40, 60, 20, 20);
Ball ball(200, 400, 20, 20);

POINT mouse;
HDC hdc;

HWND hEdit, hEdit1, hEdit2;

SOCKET sock;
int COMMAND = P_POSITION;

int Poschangex(int x)
{
   /* int temp = 0;
    if (x >= 200)
    {
        temp = x - 200;
        x = x - temp * 2;
    }
    else if (x < 200)
    {
        temp = 200 - x;
        x = x + temp * 2;
    }*/
    return 400 - x;
}

int Poschangey(int y)
{
    /*int temp = 0;
    if (y >= 400)
    {
        temp = y - 400;
        y = y - temp * 2;
    }
    else if (y < 400)
    {
        temp = 400 - y;
        y = y + temp * 2;
    }*/
    return 710 - y;
}

void DisPlayText(char* fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);

    char cbuf[BUFSIZE + 256];
    vsprintf(cbuf, fmt, arg);

    int nLength = GetWindowTextLength(hEdit2);
    SendMessage(hEdit2, EM_SETSEL, nLength, nLength);
    SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

    va_end(arg);
}

void err_quit(char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)&lpMsgBuf, msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

void err_display(char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    DisPlayText((char*)"[%s] %s", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

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
    return len - left;

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

#ifdef _DEBUG
    if (::AllocConsole() == TRUE) {
        FILE* nfp[3];
        freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
        freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
        freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
        std::ios::sync_with_stdio();
    }
#endif

    //이벤트 설정
    recvData = CreateEvent(nullptr, false, false, nullptr);
    updateData = CreateEvent(nullptr, false, true, nullptr);

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_AIRHOCKEY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);


    ULONG_PTR gdiplusToken;
    GdiplusStartupInput gdiplusStartupInput;
    if (GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Ok)
        return 0;

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AIRHOCKEY));

    //네트워크 연결 설정
    int retval;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    //socket()
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
        err_quit((char*)"socket()");

    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR)
        err_quit((char*)"connect()");

    CreateThread(NULL, 0, Client, (LPVOID)sock, 0, NULL);
    CreateThread(nullptr, 0, Update, (LPVOID)sock, 0, nullptr);

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}


//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AIRHOCKEY));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_AIRHOCKEY);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 400, 800 , nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}


//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  
    switch (message)
    {
    case WM_CREATE:
        SetTimer(hWnd, 1, 100, NULL);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;

        hdc = BeginPaint(hWnd, &ps);

        Bitmap bitmap1(400, 800);
        Bitmap bitmap2(400, 800);
        Graphics* g = new Graphics(hdc);
        Graphics memdc(&bitmap1);
        Graphics memdc2(&bitmap2);
        SolidBrush whitebursh(Color(255, 255, 255, 255));
        memdc.FillRectangle(&whitebursh, 0, 0, 400, 800);
        memdc2.FillRectangle(&whitebursh, 0, 0, 400, 800);

        //테이블 이미지 2번으로 사용.
        Image* image = Image::FromFile(L"image/table2.png");
        memdc2.DrawImage(image, 0, 0);

        if (player2.GetScore() == 0)
            image = Image::FromFile(L"image/score/enemy_score0.png");

        else if (player2.GetScore() == 1)
            image = Image::FromFile(L"image/score/enemy_score1.png");
        else if (player2.GetScore() == 2)
            image = Image::FromFile(L"image/score/enemy_score2.png");
        else if (player2.GetScore() == 3)
            image = Image::FromFile(L"image/score/enemy_score3.png");
        else if (player2.GetScore() == 4)
            image = Image::FromFile(L"image/score/enemy_score4.png");
        else if (player2.GetScore() == 5)
            image = Image::FromFile(L"image/score/enemy_score5.png");
        else if (player2.GetScore() == 6)
            image = Image::FromFile(L"image/score/enemy_score6.png");
        else if (player2.GetScore() == 7)
            image = Image::FromFile(L"image/score/enemy_score7.png");
        else if (player2.GetScore() == 8)
            image = Image::FromFile(L"image/score/enemy_score8.png");
        else if (player2.GetScore() == 9)
            image = Image::FromFile(L"image/score/enemy_score9.png");
        memdc2.DrawImage(image, 100, 200);

        if (player.GetScore() == 0)
            image = Image::FromFile(L"image/score/my_score0.png");
        else if (player.GetScore() == 1)
            image = Image::FromFile(L"image/score/my_score1.png");
        else if (player.GetScore() == 2)
            image = Image::FromFile(L"image/score/my_score2.png");
        else if (player.GetScore() == 3)
            image = Image::FromFile(L"image/score/my_score3.png");
        else if (player.GetScore() == 4)
            image = Image::FromFile(L"image/score/my_score4.png");
        else if (player.GetScore() == 5)
            image = Image::FromFile(L"image/score/my_score5.png");
        else if (player.GetScore() == 6)
            image = Image::FromFile(L"image/score/my_score6.png");
        else if (player.GetScore() == 7)
            image = Image::FromFile(L"image/score/my_score7.png");
        else if (player.GetScore() == 8)
            image = Image::FromFile(L"image/score/my_score8.png");
        else if (player.GetScore() == 9)
            image = Image::FromFile(L"image/score/my_score9.png");

        memdc2.DrawImage(image, 100, 600);

        image = Image::FromFile(L"image/ball.png");
        memdc2.DrawImage(image, ball.GetPos().Position_x - Player_R, ball.GetPos().Position_y + Player_R);

        image = Image::FromFile(L"image/enemy_racket.png");
        memdc2.DrawImage(image, player2.GetPos().Position_x - Player_R, player2.GetPos().Position_y + Player_R);

        image = Image::FromFile(L"image/my_racket.png");
        memdc2.DrawImage(image, player.GetPos().Position_x - Player_R, player.GetPos().Position_y - Player_R);

        memdc.DrawImage(&bitmap2, 0, 0);
        g->DrawImage(&bitmap1, 0, 0);

        delete image;
        delete g;

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_TIMER:
        switch (wParam)
        {
        case 1:
            GetCursorPos(&mouse);
            ScreenToClient(hWnd, &mouse);
            player.UpdatePos_x(mouse.x);
            player.UpdatePos_y(mouse.y);
            
            std::cout << "mousex: " << mouse.x << ", " << mouse.y << std::endl;

            InvalidateRgn(hWnd, NULL, FALSE);
            SetTimer(hWnd, 1, 100, NULL);
            break;

        default:
            break;
        }

        break;
    case WM_DESTROY:
        FreeConsole();
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

DWORD WINAPI Client(LPVOID arg)
{
    int id, retval, header;
    SOCKADDR_IN clientAddr;
    int addrLen;
    char buf[BUFSIZE];
    SOCKET client_sock = (SOCKET)arg;

    //getpeername
    addrLen = sizeof(clientAddr);
    getpeername(client_sock, (SOCKADDR*)&clientAddr, &addrLen);

    char hbuf[BUFSIZE];
    //데이터 전송
    while (1)
    {
        //update 이벤트 대기
        WaitForSingleObject(updateData, INFINITE);
       
        if (ball.CheckCollideRacket(&player))
            COMMAND = RACKET_COLLIDE;

        if (COMMAND == P_POSITION) {

            snprintf(hbuf, sizeof(hbuf), "%d", COMMAND);
            retval = send(client_sock, hbuf, sizeof(int), 0);

            Point2D tbuf;
            tbuf = player.GetPos();
            retval = send(client_sock, (char*)&tbuf, sizeof(Point2D), 0);
            if (retval == SOCKET_ERROR)
                err_display((char*)"send()");
        }
        else if (COMMAND == RACKET_COLLIDE)
        {
            snprintf(hbuf, sizeof(hbuf), "%d", COMMAND);
            retval = send(client_sock, hbuf, sizeof(int), 0);

            Accel2D Abuf;
            Abuf = player.GetAccel();
            retval = send(client_sock, (char*)&Abuf, sizeof(Accel2D), 0);
            if (retval == SOCKET_ERROR)
                err_display((char*)"send()");
        }

        //이벤트 활성화
        COMMAND = P_POSITION;
        SetEvent(recvData);

    }

    closesocket(sock);

    WSACleanup();
    return 0;
}

DWORD WINAPI Update(LPVOID arg)
{
    int id, retval, header;
    SOCKADDR_IN clientAddr;
    int addrLen;
    char buf[BUFSIZE];
    SOCKET client_sock = (SOCKET)arg;

    //getpeername
    addrLen = sizeof(clientAddr);
    getpeername(client_sock, (SOCKADDR*)&clientAddr, &addrLen);

    while (1)
    {
        WaitForSingleObject(recvData, INFINITE);

        retval = recvn(client_sock, buf, sizeof(int), 0);
        //printf("헤더 데이터: %d\n", atoi(buf));
        if (atoi(buf) == 2)
        {
            retval = recvn(client_sock, buf, sizeof(Point2D), 0);

            Point2D* temp;
            temp = (Point2D*)buf;

            //printf("Position_X: %d, Position_Y: %d\n", temp->Position_x, temp->Position_y);

            ball.UpdatePos_x(temp->Position_x);
            ball.UpdatePos_y(temp->Position_y);

            retval = recvn(client_sock, buf, sizeof(Point2D), 0);
            temp = (Point2D*)buf;

            //printf("2P - Position_X: %d, Position_Y: %d\n", temp->Position_x, temp->Position_y);

            player2.UpdatePos_x(Poschangex(temp->Position_x));
            player2.UpdatePos_y(Poschangey(temp->Position_y));
        }
        else if (atoi(buf) == 3)
        {
            retval = recvn(client_sock, buf, sizeof(int), 0);
            int* temp = (int*)buf;
            int score = *temp;
            printf("score : %d \n", score);
            player.Goal(score / 10);
            player2.Goal(score % 10);
        }
        SetEvent(updateData);
    }
}