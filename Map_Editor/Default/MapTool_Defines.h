#pragma once

#include <process.h>
#include "../Default/framework.h"

namespace Tool_Map
{
	const unsigned int			g_iWinSizeX = 1600;
	const unsigned int			g_iWinSizeY = 900;

	const unsigned int			g_iHalfWinSizeX = 800;
	const unsigned int			g_iHalfWinSizeY = 450;

	enum class LEVEL { STATIC, LOADING, LOGO, VILLAGE, ICELAKE, BOSS1, BOSS2, END };
}

using namespace Tool_Map;

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;