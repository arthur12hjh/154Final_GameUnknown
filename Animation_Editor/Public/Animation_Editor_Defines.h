#pragma once

#include <process.h>
#include "../Default/framework.h"

#include "Engine_Defines.h"

namespace Animation_Editor
{
	const unsigned int			g_iWinSizeX = 1600;
	const unsigned int			g_iWinSizeY = 900;

	const unsigned int			g_iHalfWinSizeX = 800;
	const unsigned int			g_iHalfWinSizeY = 450;

	enum class LEVEL { STATIC, LOADING, EDITOR, END };
	
	///  -> string szEventTag
	///  -> string szEventArgument
	///  -> string szEvent 관련 정보들
	///  -> _uint  iEventKeyFrame
	typedef struct AnimNotify
	{
		unsigned int					iNotifyKeyFrame;
		std::string						szNotifyTag;
		std::string						szNotifyArg01;
		std::string						szNotifyArg02;
	}ANIM_NOTIFY;

}

using namespace Animation_Editor;

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;


extern bool				g_bIsImgKeyBoardFoucs;