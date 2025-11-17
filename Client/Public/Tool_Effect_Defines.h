#pragma once

#include <process.h>
#include "../Default/framework.h"

namespace Tool_Effect
{
	const unsigned int			g_iWinSizeX = 1600;
	const unsigned int			g_iWinSizeY = 900;

	const unsigned int			g_iHalfWinSizeX = 800;
	const unsigned int			g_iHalfWinSizeY = 450;

	enum class LEVEL { STATIC, LOADING, TOOL, END };
}

using namespace Tool_Effect;

extern HINSTANCE		g_hInstance;
extern float			g_fGameFrame;

extern bool				g_bIsFocus;
extern bool				g_bIsMouseLock;

extern HWND				g_hWnd;