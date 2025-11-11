#pragma once

#include <process.h>
#include "../Default/framework.h"

namespace Client
{
	const unsigned int			g_iWinSizeX = 1660;
	const unsigned int			g_iWinSizeY = 900;

	const unsigned int			g_iHalfWinSizeX = 640;
	const unsigned int			g_iHalfWinSizeY = 360;

	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };
}

using namespace Client;

extern HINSTANCE		g_hInstance;
extern float			g_fGameFrame;

extern bool				g_bIsFocus;
extern bool				g_bIsMouseLock;

extern HWND				g_hWnd;