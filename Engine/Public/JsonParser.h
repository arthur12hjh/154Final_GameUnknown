#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)
class ENGINE_DLL CJsonParser
{
public :
	static void		ReadJsonData(const char* pFilePath, nlohmann::json& OutData);
	static void		SaveJsonData(const char* pFilePath, nlohmann::json& OutData);

};
NS_END