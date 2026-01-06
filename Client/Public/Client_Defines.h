#pragma once

#include <process.h>
#include "../Default/framework.h"
#include "Clinet_ColorPallet.h"
#include "ClientStruct.h"
#include "GameStruct.h"

namespace Client
{
	const unsigned int			g_iWinSizeX = 1600;
	const unsigned int			g_iWinSizeY = 900;

	//const unsigned int			g_iWinSizeX = 1920;
	//const unsigned int			g_iWinSizeY = 1080;

	enum class LEVEL 
	{ 
		STATIC,
		LOADING,
		LOGO,
		GAMEPLAY,
		SCARLET,
		BEATSABER_GAME,
		LEVEL_PROB,
		ENDING,
		END };
}

using namespace Client;

extern HINSTANCE		g_hInstance;
extern float			g_fGameFrame;

extern unsigned int		g_iHalfWinSizeX;
extern unsigned int		g_iHalfWinSizeY;

extern bool				g_bIsFocus;
extern bool				g_bIsMouseLock;
extern bool				g_bIsImgKeyBoardFoucs;

extern HWND				g_hWnd;