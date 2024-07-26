#include <stdio.h>
#pragma warning(push, 3)
#include <windows.h>
#pragma warning(pop)
#include <stdint.h>
#include "Main.h"

HWND g_GameWindow;
BOOL g_GameIsRunning;
GAMEBITMAP g_DrawingSurface;
MONITORINFO g_MonitorInfo = {sizeof(MONITORINFO)};

int32_t g_MonitorWidth;
int32_t g_MonitorHeight;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd){
    
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    if(GameIsAlreadyRunning() == TRUE){

        MessageBox(NULL, "This game is already running!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        goto Exit;
    }

    if(CreateMainGameWindow() != ERROR_SUCCESS){
        goto Exit;
    }

    g_DrawingSurface.BitmapInfo.bmiHeader.biSize = sizeof(g_DrawingSurface.BitmapInfo.bmiHeader);
    g_DrawingSurface.BitmapInfo.bmiHeader.biWidth = GAME_RES_WIDTH;
    g_DrawingSurface.BitmapInfo.bmiHeader.biHeight = GAME_RES_HEIGHT;
    g_DrawingSurface.BitmapInfo.bmiHeader.biBitCount = GAME_BPP;
    g_DrawingSurface.BitmapInfo.bmiHeader.biCompression = BI_RGB;
    g_DrawingSurface.BitmapInfo.bmiHeader.biPlanes = 1;
    if((g_DrawingSurface.Memory = VirtualAlloc(NULL, GAME_CANVAS_MEMORY_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)) == NULL){

        MessageBox(NULL, "Failed to store memory!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        goto Exit;
    }

    memset(g_DrawingSurface.Memory, 0x3F, GAME_CANVAS_MEMORY_SIZE);

    MSG Message = { 0 };

    g_GameIsRunning = TRUE;

    while(g_GameIsRunning == TRUE){
        while(PeekMessageA(&Message, g_GameWindow, 0, 0, PM_REMOVE)){

            DispatchMessageA(&Message);
        }

        ProcessPlayerInput();

        RenderFrameGraphics();

        Sleep(1);
    }

    while (GetMessageA(&Message, NULL, 0, 0) > 0){
        TranslateMessage(&Message);

        DispatchMessageA(&Message);
    }
    
    Exit:

	return(0);
}

LRESULT CALLBACK MainWndProc(_In_ HWND hwnd, _In_ UINT Message, _In_ WPARAM wParam, _In_ LPARAM lParam){

    LRESULT Result = 0;

    switch (Message){
        case WM_CLOSE:{
            g_GameIsRunning = FALSE;

            PostQuitMessage(0);

            break;
        }
        default: {
            Result = DefWindowProcA(hwnd, Message, wParam, lParam);
        }
    }
    return(Result);
}

DWORD CreateMainGameWindow(void){
    
    DWORD Result = ERROR_SUCCESS;
    
    //The settings of the window the game will display to.
    WNDCLASSEXA wc = { 0 };

    wc.cbSize = sizeof(WNDCLASSEXA);                                //The window class's size in bytes.
    wc.style = 0;                                                   //The window's style, e.g. fullscreen and border options.
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandleA(NULL);
    wc.hIcon = LoadIconA(NULL, IDI_APPLICATION);                    //The window's icon image.
    wc.hIconSm = LoadIconA(NULL, IDI_APPLICATION);                  //The window's smaller icon image.
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(255, 0, 255));          //The window's color. The default color is light grey.
    wc.lpszMenuName = NULL;                                         //The window's menu option names displayed at the top, if the window has menu options in the first place.
    wc.lpszClassName = GAME_NAME "_WindowClass";                    //The window class's alternative refrence name.

    // SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    if (RegisterClassExA(&wc) == 0) {

        Result = GetLastError();

        MessageBoxA(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        goto Exit;
    }

    g_GameWindow = CreateWindowExA(WS_EX_CLIENTEDGE, wc.lpszClassName, "RPG Game", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, GetModuleHandleA(NULL), NULL);

    if (g_GameWindow == NULL) {

        Result = GetLastError();

        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        goto Exit;
    }

    if(GetMonitorInfoA(MonitorFromWindow(g_GameWindow, MONITOR_DEFAULTTOPRIMARY), &g_MonitorInfo) == 0){

        Result = ERROR_MONITOR_NO_DESCRIPTOR;

        goto Exit;

    }

    g_MonitorWidth = g_MonitorInfo.rcMonitor.right - g_MonitorInfo.rcMonitor.left;
    g_MonitorHeight = g_MonitorInfo.rcMonitor.bottom - g_MonitorInfo.rcMonitor.top;
    
    if(SetWindowLongPtrA(g_GameWindow, GWL_STYLE, (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~WS_OVERLAPPEDWINDOW) == 0){
        Result = GetLastError();

        goto Exit;
    }
    
    if(SetWindowPos(g_GameWindow, HWND_TOP, g_MonitorInfo.rcMonitor.left, g_MonitorInfo.rcMonitor.top, g_MonitorWidth, g_MonitorHeight, SWP_NOOWNERZORDER | SWP_FRAMECHANGED) == 0){
        Result = GetLastError();

        goto Exit;
    }

    Exit:

    return(Result);
}

BOOL GameIsAlreadyRunning(void) {

    HANDLE Mutex = NULL;

    Mutex = CreateMutexA(NULL, FALSE, GAME_NAME "_GameMutex");

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return(TRUE);
    }
    else {
        return(FALSE);
    }
}

void ProcessPlayerInput(void){
    short WKeyIsDown = GetAsyncKeyState(0x57);
    short SKeyIsDown = GetAsyncKeyState(0x53);
    short AKeyIsDown = GetAsyncKeyState(0x41);
    short DKeyIsDown = GetAsyncKeyState(0x44);

    if(WKeyIsDown){
        MessageBox(NULL, "You pressed the UP button!", "Input", MB_ICONEXCLAMATION | MB_OK);
    }
    if(SKeyIsDown){
        MessageBox(NULL, "You pressed the DOWN button!", "Input", MB_ICONEXCLAMATION | MB_OK);
    }
    if (AKeyIsDown) {
        MessageBox(NULL, "You pressed the LEFT button!", "Input", MB_ICONEXCLAMATION | MB_OK);
    }
    if (DKeyIsDown) {
        MessageBox(NULL, "You pressed the RIGHT button!", "Input", MB_ICONEXCLAMATION | MB_OK);
    }
}

void RenderFrameGraphics(void){
    //memset(g_DrawingSurface.Memory, 0xFF, 4);

    PIXEL32 Pixel = {0};

    Pixel.Blue = 0x20;
    Pixel.Green = 0xB2;
    Pixel.Red = 0x10;
    Pixel.Alpha = 0xFF;

    for(int x = 0; x < (GAME_RES_WIDTH * GAME_RES_HEIGHT) / 4; x++) {
        memcpy((PIXEL32*)g_DrawingSurface.Memory + x, &Pixel, sizeof(PIXEL32));
    }

    HDC DeviceContext = GetDC(g_GameWindow);

    StretchDIBits(DeviceContext, 0, 0, g_MonitorWidth, g_MonitorHeight, 0, 0,GAME_RES_WIDTH, GAME_RES_HEIGHT, g_DrawingSurface.Memory, &g_DrawingSurface.BitmapInfo, DIB_RGB_COLORS, SRCCOPY);

    ReleaseDC(g_GameWindow, DeviceContext);
}
