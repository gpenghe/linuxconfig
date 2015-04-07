#include <windows.h>

#include "build/resource.h"

#define ID_FILE_OPEN    9001
#define ID_FILE_EXIT    9002
#define ID_HELP_ABOUT   10001

const char g_szClassName[] = "myWindowClass";
HBITMAP g_hbmBall = NULL;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case ID_FILE_EXIT:
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
            }
            break;
        case WM_CREATE:
            {
                HMENU hMenu, hSubMenu;
                
                hMenu = CreateMenu();
                hSubMenu = CreatePopupMenu();
                AppendMenu(hSubMenu, MF_STRING, ID_FILE_OPEN, "&Open...");
                AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, "&Exit");
                AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&File");

                hSubMenu = CreatePopupMenu();
                AppendMenu(hSubMenu, MF_STRING, ID_HELP_ABOUT, "&About...");
                AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Help");

                SetMenu(hwnd, hMenu);
            }

            g_hbmBall = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1));
            if(g_hbmBall == NULL)
            {
                MessageBox(hwnd, "Could not load IDB_BALL!", "Error", MB_OK | MB_ICONEXCLAMATION);
            }
            break;
        case WM_PAINT:
            {
                BITMAP bm;
                PAINTSTRUCT ps;

                HDC hdc = BeginPaint(hwnd, &ps);

                HDC hdcMem = CreateCompatibleDC(hdc);

                HBITMAP hbmOld = (HBITMAP) SelectObject(hdcMem, g_hbmBall);

                GetObject(g_hbmBall, sizeof(bm), &bm);

                BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

                SelectObject(hdcMem, hbmOld);

                DeleteDC(hdcMem);

                EndPaint(hwnd, &ps);
            }
            break;
        case WM_LBUTTONDOWN:
            {
                char szFileName[MAX_PATH];
                HMODULE hModule = GetModuleHandle(NULL);
                GetModuleFileName(hModule, szFileName, MAX_PATH);
                MessageBox(hwnd, szFileName, "Test", MB_OK | MB_ICONINFORMATION);
            }
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    // Step 1: Registering a Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Windows Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Create thhe Window
    hwnd = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            g_szClassName,
            "The title of my window",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 640 /*w*/, 480 /*h*/,
            NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Eror!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
