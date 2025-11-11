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

	enum class NAVI_MODE { NONE, ADD_POINT, ADD_CELL, DELETE_CELL, RESET_ALL, END };
	enum class TOOL_MODE { MAP, LIGHT, CAMERA, END };
	enum class ADD_OBJECT { VIL_BUI03_04, PLAYER, END };
}

using namespace Tool_Map;

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;