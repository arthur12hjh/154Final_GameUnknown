#pragma once

#include <process.h>
#include "../Default/framework.h"

namespace Client
{
	const unsigned int			g_iWinSizeX = 1600;
	const unsigned int			g_iWinSizeY = 900;

<<<<<<< HEAD
	const unsigned int			g_iHalfWinSizeX = 800;
	const unsigned int			g_iHalfWinSizeY = 450;
=======
>>>>>>> adbb795150dfd6bf476ed25523ec24c731b3e995

	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };
}

using namespace Client;

extern HINSTANCE		g_hInstance;
extern float			g_fGameFrame;
extern unsigned int		g_iHalfWinSizeX;
extern unsigned int		g_iHalfWinSizeY;


extern bool				g_bIsFocus;
extern bool				g_bIsMouseLock;

extern HWND				g_hWnd;