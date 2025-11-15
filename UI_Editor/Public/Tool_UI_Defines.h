#pragma once

#define GUI ImGui

namespace Tool_UI
{
	const unsigned int			g_iWinSizeX = 1600;
	const unsigned int			g_iWinSizeY = 900;

	const unsigned int			g_iHalfWinSizeX = 800;
	const unsigned int			g_iHalfWinSizeY = 450;

	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };
}

using namespace Tool_UI;

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;