#include <windows.h>

#include "build/resource.h"

#define ID_FILE_OPEN    9001
#define ID_FILE_EXIT    9002
#define ID_HELP_ABOUT   10001

const char g_szClassName[] = "myWindowClass";
HBITMAP g_hbmBall = NULL;
HBITMAP g_hbmMask = NULL;

HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
    HDC hdcMem, hdcMem2;
    HBITMAP hbmMask;
    BITMAP bm;

    // Create monochrome (1 bit) mask bitmap.  

    GetObject(hbmColour, sizeof(BITMAP), &bm);
    hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

    // Get some HDCs that are compatible with the display driver

    hdcMem = CreateCompatibleDC(0);
    hdcMem2 = CreateCompatibleDC(0);

    SelectObject(hdcMem, hbmColour);
    SelectObject(hdcMem2, hbmMask);

    // Set the background colour of the colour image to the colour
    // you want to be transparent.
    SetBkColor(hdcMem, crTransparent);

    // Copy the bits from the colour image to the B+W mask... everything
    // with the background colour ends up white while everythig else ends up
    // black...Just what we wanted.

    BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

    // Take our new mask and use it to turn the transparent colour in our
    // original colour image to black so the transparency effect will
    // work right.
    BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

    // Clean up.

    DeleteDC(hdcMem);
    DeleteDC(hdcMem2);

    return hbmMask;
}

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
            g_hbmMask = CreateBitmapMask(g_hbmBall, RGB(0, 0, 0));
            if(g_hbmMask == NULL)
            {
                MessageBox(hwnd, "Could not create mask!", "Error", MB_OK | MB_ICONEXCLAMATION);
            }
            break;
        case WM_PAINT:
            {
                BITMAP bm;
                PAINTSTRUCT ps;
                HBITMAP hbmOld;

                HDC hdc = BeginPaint(hwnd, &ps);

                HDC hdcMem = CreateCompatibleDC(hdc);

                GetObject(g_hbmBall, sizeof(bm), &bm);

                hbmOld = (HBITMAP) SelectObject(hdcMem, g_hbmMask);              
                BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCAND);

                SelectObject(hdcMem, g_hbmBall);
                BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCPAINT);

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
