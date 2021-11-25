// AirHockey.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "GameHeader.h"
#include <WinSock2.h>
#include <gdiplus.h>
#pragma comment(lib,"Gdiplus.lib")
#pragma comment(lib,"ws2_32")
using namespace Gdiplus;
#pragma warning(disable : 4996)




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

Player player(40, 60, 20, 20);
Player player2(40, 60, 0, 20);
Ball ball(200, 400, 20, 20);

POINT mouse;
HDC hdc;

HWND hEdit, hEdit1, hEdit2;

SOCKET sock;
int COMMAND = P_POSITION;

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

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_AIRHOCKEY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);


    ULONG_PTR gdiplusToken;
    GdiplusStartupInput gdiplusStartupInput;
    if(GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL)!=Ok)
        return 0;

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AIRHOCKEY));

    CreateThread(NULL, 0, Client, NULL, 0, NULL);

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
    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AIRHOCKEY));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_AIRHOCKEY);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
      CW_USEDEFAULT, 0, 400, 800, nullptr, nullptr, hInstance, nullptr);

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

  
    SetTimer(hWnd, 1, 1, NULL);
    SetTimer(hWnd, 2, 100, NULL);

    switch (message)
    {
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

            Image* image = Image::FromFile(L"image/table1.png");
            memdc2.DrawImage(image, 0, 0);
           
            image = Image::FromFile(L"image/ball.png");
            memdc2.DrawImage(image, ball.GetPos().Position_x - Player_R, ball.GetPos().Position_y+ Player_R);
            
            image = Image::FromFile(L"image/enemy_racket.png");
            memdc2.DrawImage(image, player2.GetPos().Position_x - Player_R, player2.GetPos().Position_y - Player_R);

            image = Image::FromFile(L"image/my_racket.png");
            memdc2.DrawImage(image, player.GetPos().Position_x - Player_R, player.GetPos().Position_y - Player_R);
          
            memdc.DrawImage(&bitmap2,0,0);
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

            ball.UpdateAccel_x();
            ball.UpdateAccel_y();

            if (ball.CheckCollideRacket(&player))
            {
                //COMMAND = RACKET_COLLIDE;
            }

            ball.CheckGoal(&player, &player2);
            ball.CheckcollideCircuit();
            InvalidateRgn(hWnd, NULL, FALSE);
            break;
        case 2:
            
            break;
        default:
            break;
        }
        
        break;
    case WM_DESTROY:
        KillTimer(hWnd, 1);
        KillTimer(hWnd, 2);
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
    //Point2D buf;
    char buf[BUFSIZE];
    Accel2D A_buf;
    char hbuf[BUFSIZE];
    //데이터 전송
    while (1)
    {
        
        //retval = recv(sock, (char*)&buf, sizeof(Point2D), 0);
        //ball.UpdatePos_x(buf.Position_x);
        //ball.UpdatePos_y(buf.Position_y);

        if (COMMAND == P_POSITION)
        {
            snprintf(hbuf, sizeof(hbuf), "%d", P_POSITION);
            retval = send(sock, hbuf, BUFSIZE, 0);

            Point2D* temp;
            Point2D tbuf;
            //temp = player.GetPos();
            tbuf.Position_x = 40;
            tbuf.Position_y = 40;
            temp = &tbuf;
            //snprintf(buf, sizeof(buf), "%d %d", temp.Position_x, temp.Position_y);
            //플레이어 위치 전송
            retval = send(sock, (char*)temp, BUFSIZE, 0);
            if (retval == SOCKET_ERROR)
                err_display((char*)"send()");

        }
        /*else if (COMMAND == RACKET_COLLIDE)
        {
            buf = player.GetPos();
            A_buf = player.GetAccel();
            retval = send(sock, (char*)&buf, sizeof(Point2D), 0);
            if (retval == SOCKET_ERROR)
                err_display((char*)"send()");
            retval = send(sock, (char*)&A_buf, sizeof(Accel2D), 0);
            if (retval == SOCKET_ERROR)
                err_display((char*)"send()");
        }*/

    }

    closesocket(sock);

    WSACleanup();
    return 0;
}