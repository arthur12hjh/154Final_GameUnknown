#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)
class ENGINE_DLL CStringHelper
{
public :
	static void ConvertWideToUTF(const WCHAR* _wstr, char* _Outstr = nullptr);
	static void ConvertUTFToWide(const char* _str, WCHAR* _outwstr = nullptr);
};
NS_END