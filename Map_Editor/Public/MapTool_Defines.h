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

	enum class MAP_THEME { TUTORIAL, GORILLA, SCARLET, END };
	enum class NAVI_MODE { NONE, ADD_POINT, ADD_CELL, DELETE_CELL, RESET_ALL, END };
	enum class TOOL_MODE { MAP, LIGHT, CAMERA, END };
	enum class ADD_OBJECT { BAMBOO, REED, ROCK1, ROCK2, ROCK3, ROCK4, ROCK5, ROCK6, ROCK7, ROCK8,
							STONEWALL1, STONEWALL2, STONE1, STONE2, STONE3, STONE4, STAIR, INSCRIPTION_L, INSCRIPTION_R, TOMBSTONE, TOMBSTONEBASE1, TOMBSTONEBASE2,
							VIL_BUI03_04, 
							PLAYER, END };
}

using namespace Tool_Map;

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;