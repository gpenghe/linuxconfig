#include <windows.h>
#include <commctrl.h>

#include "build/resource.h"

#define ID_FILE_OPEN    9001
#define ID_FILE_EXIT    9002
#define ID_HELP_ABOUT   10001
#define ID_BUTTON_PLAY  20001

const int PANEL_H   = 60;
const int BUTTON_W  = 70;
const int BUTTON_H  = 21;
const int BUTTON_BDR_DOWN  = 10;

const char g_szClassName[] = "myWindowClass";
HBITMAP g_hbmBall = NULL;
HBITMAP g_hbmMask = NULL;
WNDCLASSEX g_wc_panel;
HWND g_hwndMain;
HWND g_hwndPanel;
HWND g_hwndButtonPlay;

HRESULT RedrawControls()
{
    RECT rectClientArea;

    GetClientRect(g_hwndMain, &rectClientArea);

    SetWindowPos(g_hwndPanel, NULL, 
        0,  // x
        rectClientArea.bottom - PANEL_H,    // y
        rectClientArea.right,   // w
        PANEL_H,                // h
        SWP_SHOWWINDOW);

    SetWindowPos(g_hwndButtonPlay, NULL, 
        rectClientArea.right/2 - BUTTON_W/2,    // x
        PANEL_H - BUTTON_BDR_DOWN - BUTTON_H,   // y
        BUTTON_W,   // w
        BUTTON_H,   // h
        SWP_SHOWWINDOW);
    
    return S_OK;
}

LRESULT CALLBACK PanelWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case ID_BUTTON_PLAY:
                    MessageBox(hwnd, "You clicked me", "Test", MB_OK);
                    break;
            }
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}
HRESULT register_window_classes()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASSEX wc;

    // Registering the "panel" Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = PanelWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "My Panel";
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
    
    g_wc_panel = wc;
    if(!RegisterClassEx(&g_wc_panel))
    {
        MessageBox(NULL, "Panel Window Class Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return E_FAIL; 
    }

    return S_OK;
}

HRESULT InitializeComponent(HWND hWnd) {
    RECT rect;
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // Create the Panel
    GetClientRect(hWnd, &rect);
    g_hwndPanel = CreateWindowEx(
            0,
            g_wc_panel.lpszClassName,
            "The title of my window",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            0, rect.bottom - PANEL_H, rect.right /*w*/, PANEL_H/*h*/,
            hWnd, NULL, hInstance, NULL);

    if(g_hwndPanel == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Eror!", MB_ICONEXCLAMATION | MB_OK);
        return E_FAIL;
    }

    g_hwndButtonPlay = CreateWindowEx(WS_EX_APPWINDOW
        , "BUTTON", NULL
        , WS_CHILD | WS_VISIBLE | 
          BS_FLAT
        , rect.right/2 - BUTTON_W/2, BUTTON_BDR_DOWN + BUTTON_H, BUTTON_W, BUTTON_H
        , g_hwndPanel, (HMENU)ID_BUTTON_PLAY, hInstance, NULL);        
    SetWindowText(g_hwndButtonPlay, "Play");

    return S_OK;
}

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
        case WM_SIZE:
            RedrawControls();
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
            {
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
            }
            {
                if(S_OK != InitializeComponent(hwnd)) {
                    MessageBox(hwnd, "Could not create components!", "Error", MB_OK | MB_ICONEXCLAMATION);
                    // TODO: exit cleanly
                }
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
#if 0
            {
                char szFileName[MAX_PATH];
                HMODULE hModule = GetModuleHandle(NULL);
                GetModuleFileName(hModule, szFileName, MAX_PATH);
                MessageBox(hwnd, szFileName, "Test", MB_OK | MB_ICONINFORMATION);
            }
#endif
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
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
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOWTEXT);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Windows Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    register_window_classes();

    // Step 2: Create the Window
    g_hwndMain = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            g_szClassName,
            "The title of my window",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 640 /*w*/, 480 /*h*/,
            NULL, NULL, hInstance, NULL);

    if(g_hwndMain == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Eror!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(g_hwndMain, nCmdShow);
    UpdateWindow(g_hwndMain);

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
