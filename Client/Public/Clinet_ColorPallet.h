#pragma once
#include "Engine_Defines.h"

namespace Client
{
#define COLOR_PATTERN_SCARLET	XMFLOAT4(3.98f, 2.5f, 4.40f, 1.f)
#define COLOR_PATTERN_GIGAS		XMFLOAT4(1.f, 1.f , 1.f, 1.f)

#define COLOR_PATTERN_RED		XMFLOAT4(1.f, 0.f , 0.f, 1.f)
#define COLOR_PATTERN_GREEN		XMFLOAT4(0.f, 1.f , 0.f, 1.f)
#define COLOR_PATTERN_BLUE		XMFLOAT4(0.f, 0.f , 1.f, 1.f)
#define COLOR_PATTERN_GRAY		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.f)

constexpr static XMFLOAT4 CLINET_COLOR_PATTERN[7] = {
			COLOR_PATTERN_SCARLET,
			COLOR_PATTERN_GIGAS,
			COLOR_PATTERN_RED,
			COLOR_PATTERN_GREEN,
			COLOR_PATTERN_BLUE,
			COLOR_PATTERN_GRAY,
};
	
}