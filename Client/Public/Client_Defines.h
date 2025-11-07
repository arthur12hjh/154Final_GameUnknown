#pragma once

#include <process.h>
#include "../Default/framework.h"

namespace Client
{
	const unsigned int			g_iWinSizeX = 1600;
	const unsigned int			g_iWinSizeY = 900;

	const unsigned int			g_iHalfWinSizeX = 800;
	const unsigned int			g_iHalfWinSizeY = 450;

	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };
}

using namespace Client;

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;