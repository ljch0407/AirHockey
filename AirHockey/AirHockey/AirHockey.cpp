// AirHockey.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "AirHockey.h"

#define MAX_LOADSTRING 100

#define Player_R 30

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AIRHOCKEY));

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

    return (int) msg.wParam;
}

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


class Player
{
private:
    Point2D m_Position;
    Accel2D m_Accel;
    int m_Goal;

public:

    Player()
    {
        m_Position.Position_x = 0;
        m_Position.Position_y = 0;

        m_Accel.Accel_x = 0.0;
        m_Accel.Accel_y = 0.0;
        m_Goal = 0;
    }

    Player(int xPos, int yPos, float xAccel, float yAccel)
    {
        m_Position.Position_x = xPos;
        m_Position.Position_y = yPos;

        m_Accel.Accel_x = xAccel;
        m_Accel.Accel_y = yAccel;
        m_Goal = 0;
    }
    Point2D GetPos()
    {
        return m_Position;
    }

    Accel2D GetAccel()
    {
        return m_Accel;
    }

    void UpdatePos_x(int pos)
    {
       m_Position.Position_x = pos; 
    }
    void UpdatePos_y(int pos)
    {
        m_Position.Position_y = pos;
    }
    void Goal(void)
    {
        m_Goal++;
    }
};



class Ball
{
private:
    Point2D m_Position;
    Accel2D m_Accel;
    bool Collide;
public:

    Ball()
    {
        m_Position.Position_x = 0;
        m_Position.Position_y = 0;

        m_Accel.Accel_x = 0.0;
        m_Accel.Accel_y = 0.0;
        Collide = false;
    }

    Ball(int xPos, int yPos, float xAccel, float yAccel)
    {
        m_Position.Position_x = xPos;
        m_Position.Position_y = yPos;

        m_Accel.Accel_x = xAccel;
        m_Accel.Accel_y = yAccel;
    }

    Point2D GetPos()
    {
        return m_Position;
    }

    Accel2D GetAccel()
    {
        return m_Accel;
    }

    void UpdatePos_x(int Accel)
    {
        m_Position.Position_x += Accel;
        if (m_Position.Position_x + Player_R > 400)
        {
            m_Position.Position_x = 370;
        }
        else if (m_Position.Position_x - Player_R < 0)
        {
            m_Position.Position_x = 30;
        }

    }
    void UpdatePos_y(int Accel)
    {
        m_Position.Position_y += Accel;

        if (m_Position.Position_y + Player_R > 800)
        {
            m_Position.Position_y = 770;
        }
        else if (m_Position.Position_y - Player_R < 0)
        {
            m_Position.Position_y = 30;
        }
    }

    void UpdateAccel_x()
    {
        if (m_Accel.Accel_x > 0)
            m_Accel.Accel_x -= 0.1;
        else if (m_Accel.Accel_x < 0)
            m_Accel.Accel_x += 0.1;
        else
            m_Accel.Accel_x = 0;
    }
    void UpdateAccel_y()
    {
        if (m_Accel.Accel_y > 0)
            m_Accel.Accel_y -= 0.1;
        else if (m_Accel.Accel_y < 0)
            m_Accel.Accel_y += 0.1;
        else
            m_Accel.Accel_y = 0;
    }

    void ChangeAccel_x(int val)
    {
       
        m_Accel.Accel_x = val;
        
        if (abs((int)m_Accel.Accel_x) >= 20)
        {
            if (m_Accel.Accel_x < 0)
            {
                m_Accel.Accel_x = -20;
            }
            else if (m_Accel.Accel_x > 0)
                m_Accel.Accel_x = 20;
        }
    }

    void ChangeAccel_y(int val)
    {
        m_Accel.Accel_y = val;

        if (abs((int)m_Accel.Accel_y) >= 20)
        {
            if (m_Accel.Accel_y < 0)
            {
                m_Accel.Accel_y = -20;
            }
            else if (m_Accel.Accel_y > 0)
                m_Accel.Accel_y = 20;
        }
    }

    void CheckcollideCircuit()
    {
        if (m_Position.Position_x + Player_R >= 400)
        {

            ChangeAccel_x(-(m_Accel.Accel_x));
        }
        else if (m_Position.Position_x - Player_R <= 0)
        {
            ChangeAccel_x(abs((int)(m_Accel.Accel_x)));
        }

        if (m_Position.Position_y + Player_R >= 800)
        {
            ChangeAccel_y(-(m_Accel.Accel_y));
        }
        else if (m_Position.Position_y - Player_R <= 0)
        {
            ChangeAccel_y(abs((int)(m_Accel.Accel_y)));
        }
    }

    void CheckCollideRacket(Player* point)
    {
        if (m_Position.Position_x > point->GetPos().Position_x - 30 && m_Position.Position_x < point->GetPos().Position_x + 30)
        {
            if (m_Position.Position_x > point->GetPos().Position_x)
            {
                if (m_Position.Position_y + 30 > point->GetPos().Position_y && m_Position.Position_y - 30 < point->GetPos().Position_y)
                {
                    if (m_Position.Position_y > point->GetPos().Position_y)
                    {
                        ChangeAccel_x(abs((int)(m_Accel.Accel_x)) + point->GetAccel().Accel_x);
                        ChangeAccel_y(abs((int)(m_Accel.Accel_y))+ point->GetAccel().Accel_y);
                    }

                    else if (m_Position.Position_y < point->GetPos().Position_y)
                    {
                        ChangeAccel_x(abs((int)(m_Accel.Accel_x)) + point->GetAccel().Accel_x);
                        ChangeAccel_y(-(m_Accel.Accel_y)-point->GetAccel().Accel_y);
                    }
                    
                }
            }
            else if (m_Position.Position_x < point->GetPos().Position_x)
            {
                if (m_Position.Position_y + 30 > point->GetPos().Position_y && m_Position.Position_y - 30 < point->GetPos().Position_y)
                {
                    if (m_Position.Position_y > point->GetPos().Position_y)
                    {
                         ChangeAccel_x(-(m_Accel.Accel_x) - point->GetAccel().Accel_x);
                         ChangeAccel_y(abs((int)(m_Accel.Accel_y)) + point->GetAccel().Accel_y);
                    }
                    else if (m_Position.Position_y < point->GetPos().Position_y)
                    {
                        ChangeAccel_x(-(m_Accel.Accel_x) - point->GetAccel().Accel_x);
                        ChangeAccel_y(-(m_Accel.Accel_y) - point->GetAccel().Accel_y);
                    }
                }
            }
        }
    }

    void CheckGoal(Player* p1, Player* p2)
    {

        if (m_Position.Position_x >= 170 && m_Position.Position_x <= 230)
        {
            if (m_Position.Position_y >= 0 && m_Position.Position_y <= 50)
            {
                m_Position.Position_x = 200;
                m_Position.Position_y = 400;
                m_Accel.Accel_x = 0;
                m_Accel.Accel_y = 0;
                p1->Goal();
               
            }
            else if (m_Position.Position_y >= 750 && m_Position.Position_y < 800)
            {
                m_Position.Position_x = 200;
                m_Position.Position_y = 400;
                m_Accel.Accel_x = 0;
                m_Accel.Accel_y = 0;
                p2->Goal();
            }
        }
    }
};

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

Player player(40, 60, 20, 20);
Player player2(40, 60, 0, 20);
Ball ball(200, 400, 20, 20);

POINT mouse;
HDC hdc;

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

    HBRUSH hBrush, oldBrush;
  
    SetTimer(hWnd, 1, 10, NULL);

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
            

            hBrush = CreateSolidBrush(RGB(255, 0, 0));
            oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            Ellipse(hdc, player.GetPos().Position_x-Player_R, player.GetPos().Position_y- Player_R, player.GetPos().Position_x + Player_R, player.GetPos().Position_y + Player_R);
           

            hBrush = CreateSolidBrush(RGB(0, 255, 0));
            oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            Ellipse(hdc, ball.GetPos().Position_x - Player_R, ball.GetPos().Position_y - Player_R, ball.GetPos().Position_x + Player_R, ball.GetPos().Position_y + Player_R);
           

            hBrush = CreateSolidBrush(RGB(0, 0, 255));
            oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            Ellipse(hdc, player2.GetPos().Position_x - Player_R, player2.GetPos().Position_y - Player_R, player2.GetPos().Position_x + Player_R, player2.GetPos().Position_y + Player_R);

            hBrush = CreateSolidBrush(RGB(0, 255, 255));
            oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            Rectangle(hdc, 170, 0, 230, 50);
            Rectangle(hdc, 170, 750, 230, 800);

            SelectObject(hdc, oldBrush);
            DeleteObject(hBrush);
            
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_TIMER:

        GetCursorPos(&mouse);
        ScreenToClient(hWnd, &mouse);
        player.UpdatePos_x(mouse.x);
        player.UpdatePos_y(mouse.y);


        ball.UpdatePos_x(ball.GetAccel().Accel_x);
        ball.UpdatePos_y(ball.GetAccel().Accel_y);

        ball.UpdateAccel_x();
        ball.UpdateAccel_y();
        
        ball.CheckCollideRacket(&player);
        ball.CheckCollideRacket(&player2);

        ball.CheckGoal(&player,&player2);
        ball.CheckcollideCircuit();
        InvalidateRgn(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
        break;
    case WM_DESTROY:
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
