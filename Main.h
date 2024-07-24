#pragma once

#define GAME_NAME "RPG_Game"
#define GAME_RES_WIDTH 384
#define GAME_RES_HEIGHT 216
#define GAME_BPP 32
#define GAME_CANVAS_MEMORY_SIZE (GAME_RES_WIDTH * GAME_RES_HEIGHT * (GAME_BPP / 8))

typedef struct GAMEBITMAP{
	BITMAPINFO BitmapInfo;

	void* Memory;

} GAMEBITMAP;

LRESULT CALLBACK MainWndProc(_In_ HWND hwnd, _In_ UINT Message, _In_ WPARAM wParam, _In_ LPARAM lParam);

DWORD CreateMainGameWindow(void);

BOOL GameIsAlreadyRunning(void);

void ProcessPlayerInput(void);
void RenderFrameGraphics(void);

