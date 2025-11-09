#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)
class CHUDLayer;

class ENGINE_DLL CXmlParser
{
public:
	typedef struct tagXmlDesc
	{
		string szEltag{};
		string szText{};
		map<string, string> attr;
	}XML_DESC;

public:
	static void Load_From_XML(XML::XMLElement* element);
	static void Save_To_XML(XML::XMLDocument& doc, XML_DESC* pDesc, XML::XMLElement* parent = nullptr);
};
NS_END

/* [저장]
ui 저장 단위는 Layer단위 ex)<Layer name = "Default"> ... </Layer>
레이어 하나씩 저장
*/