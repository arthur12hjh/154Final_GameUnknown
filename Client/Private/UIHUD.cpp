#include "pch.h"
#include "UIHUD.h"
#include "HUDLayer.h"

#include "JsonParser.h"
#include "StringHelper.h"

#include "GameInstance.h"
#include "UIBase.h"

CUIHUD::CUIHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameHUD{ pDevice, pContext }
{
}

HRESULT CUIHUD::Save_Data(_wstring szLayerTag)
{
	auto pLayer = m_pLayers.find(szLayerTag);

	if (pLayer == m_pLayers.end())
		return E_FAIL;

	auto pUIObjects = pLayer->second->Get_UserInterfaces();
	
	Json jUIObjects;

	if (pUIObjects)
	{
		for (auto& pUIObj : *pUIObjects)
		{
			CUIBase* pUI = dynamic_cast<CUIBase*>(pUIObj.second);

			if (!pUI->Get_Parent())
			{
				Json jChildren;

				Save_Hierarchy(pUI, jChildren, true);
				jUIObjects.push_back(jChildren);
			}
			else
				continue;
		}
	}

	_wstring szFilePath{};
	szFilePath = TEXT("../Bin/DataFiles/UI/") + szLayerTag + TEXT(".json");

	_char szPath[MAX_PATH]{};
	CStringHelper::ConvertWideToUTF(szFilePath.c_str(), szPath);

	// json배열이 이상함
	CJsonParser::SaveJsonData(szPath, jUIObjects);
	
	MSG_BOX("저장 성공");

	return S_OK;
}

HRESULT CUIHUD::Export_Anim_Prefab(_wstring szAnimTag, void* pDesc)
{
	CUIBase::UI_ANIM_DESC AnimDesc = *static_cast<CUIBase::UI_ANIM_DESC*>(pDesc);

	Json jAnim;

	_char szText[MAX_PATH]{};
	CStringHelper::ConvertWideToUTF(szAnimTag.c_str(), szText);
	jAnim["szAnimTag"] = szText;
	jAnim["fDuration"] = AnimDesc.fDuration;
	jAnim["isLoop"] = AnimDesc.isLoop;

	for (auto& TrackDesc : AnimDesc.m_Tracks)
	{
		Json jTrack;
		
		CStringHelper::ConvertWideToUTF(TrackDesc.first.c_str(), szText);
		jTrack["szTrackTag"] = szText;
		jTrack["vStartParam"] = {
			TrackDesc.second->vStartParam.x,
			TrackDesc.second->vStartParam.y,
			TrackDesc.second->vStartParam.z,
			TrackDesc.second->vStartParam.w
		};
		jTrack["vEndParam"] = {
			TrackDesc.second->vEndParam.x,
			TrackDesc.second->vEndParam.y,
			TrackDesc.second->vEndParam.z,
			TrackDesc.second->vEndParam.w
		};

		jAnim["Tracks"].push_back(jTrack);
	}

	_wstring szFilePath{};
	szFilePath = TEXT("../Bin/DataFiles/UI/UI_Anims/") + szAnimTag + TEXT(".json");

	_char szPath[MAX_PATH]{};
	CStringHelper::ConvertWideToUTF(szFilePath.c_str(), szPath);

	// json배열이 이상함
	CJsonParser::SaveJsonData(szPath, jAnim);

	MSG_BOX("저장 성공");

	return S_OK;
}

HRESULT CUIHUD::Load_Anim_Files()
{
	WIN32_FIND_DATAW fd;
	_wstring search = L"../Bin/DataFiles/UI/UI_Anims/*.json";

	HANDLE hFind = FindFirstFileW(search.c_str(), &fd);

	if (hFind == INVALID_HANDLE_VALUE)
		return E_FAIL;

	do
	{
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			_wstring szfileName = fd.cFileName;

			size_t pos = szfileName.find_last_of(L'.');

			m_AnimDatas.push_back(szfileName.substr(0, pos));
		}
	} while (FindNextFileW(hFind, &fd));

	FindClose(hFind);

	return S_OK;
}

HRESULT CUIHUD::Import_Anim_Prefab(_wstring szAnimTag, void* pAnimOut)
{
	Json jAnim;

	_wstring szFilePath{};
	szFilePath = TEXT("../Bin/DataFiles/UI/UI_Anims/") + szAnimTag + TEXT(".json");

	_char szPath[MAX_PATH]{};
	CStringHelper::ConvertWideToUTF(szFilePath.c_str(), szPath);

	CJsonParser::ReadJsonData(szPath, jAnim);

	WCHAR szText[MAX_PATH]{};

	CUIBase::UI_ANIM_DESC* AnimDesc = new CUIBase::UI_ANIM_DESC();

	AnimDesc->isLoop = jAnim["isLoop"].get<_bool>();
	AnimDesc->fDuration = jAnim["fDuration"].get<_float>();

	for (auto& Track : jAnim["Tracks"])
	{
		CUIBase::UI_ANIM_TRACK_DESC TrackDesc{};

		CStringHelper::ConvertUTFToWide(Track["szTrackTag"].get<string>().c_str(), szText);
		TrackDesc.szTrackTag = szText;
		TrackDesc.vStartParam = {
			Track["vStartParam"][0].get<_float>(),
			Track["vStartParam"][1].get<_float>(),
			Track["vStartParam"][2].get<_float>(),
			Track["vStartParam"][3].get<_float>()
		};
		TrackDesc.vEndParam = {
			Track["vEndParam"][0].get<_float>(),
			Track["vEndParam"][1].get<_float>(),
			Track["vEndParam"][2].get<_float>(),
			Track["vEndParam"][3].get<_float>()
		};

		AnimDesc->Add_UI_Track_Desc(szText, TrackDesc);
	}

	*static_cast<CUIBase::UI_ANIM_DESC*>(pAnimOut) = *AnimDesc;

	return S_OK;
}

void CUIHUD::Save_Hierarchy(CUIBase* pUI, Json& OutData, _bool bIsRoot)
{
	if (!pUI)
		return;

	CUIBase::UIBASE_DESC pDesc = pUI->Get_UIBase_Desc();

	_char szText[MAX_PATH]{};

	Json jObj;

	jObj["fAlpha"] = pDesc.fAlpha;
	jObj["fX"] = pDesc.fX;
	jObj["fY"] = pDesc.fY;
	jObj["fOffsetX"] = pDesc.fOffsetX;
	jObj["fOffsetY"] = pDesc.fOffsetY;
	jObj["fSizeX"] = pDesc.fSizeX;
	jObj["fSizeY"] = pDesc.fSizeY;
	jObj["iLevel"] = pDesc.iLevel;
	jObj["iDepth"] = pDesc.iDepth;
	jObj["iRenderGroup"] = pDesc.iRenderGroup;
	CStringHelper::ConvertWideToUTF(pDesc.szUITag.c_str(), szText);
	jObj["szUITag"] = szText;
	CStringHelper::ConvertWideToUTF(pDesc.szLayerTag.c_str(), szText);
	jObj["szLayerTag"] = szText;
	CStringHelper::ConvertWideToUTF(pDesc.szProtoTag.c_str(), szText);
	jObj["szProtoTag"] = szText;

	for (auto& pAnimDesc : pDesc.Get_UI_Anim_Descs())
	{
		jObj["szAnimTags"].push_back(pAnimDesc.first);
	}

	if (pDesc.Get_UI_Texture_Desc())
	{
		Json TextureDesc;

		CStringHelper::ConvertWideToUTF(pDesc.Get_UI_Texture_Desc()->szTextureComTag.c_str(), szText);
		TextureDesc["szTextureComTag"] = szText;
		CStringHelper::ConvertWideToUTF(pDesc.Get_UI_Texture_Desc()->szProtoTag.c_str(), szText);
		TextureDesc["szProtoTag"] = szText;
		TextureDesc["iTextureIndex"] = pDesc.Get_UI_Texture_Desc()->iTextureIndex;
		TextureDesc["iPass"] = pDesc.Get_UI_Texture_Desc()->iPass;
		jObj["TextureDesc"] = TextureDesc;
	}

	if (pDesc.Get_UI_Text_Desc())
	{
		Json TextDesc;

		//CStringHelper::ConvertWideToUTF(pDesc.Get_UI_Text_Desc()->szText.c_str(), szText);

		TextDesc["szText"] = WStringToUTF8(pDesc.Get_UI_Text_Desc()->szText.c_str());
		TextDesc["vColor"] = {
			pDesc.Get_UI_Text_Desc()->vColor.x,
			pDesc.Get_UI_Text_Desc()->vColor.y,
			pDesc.Get_UI_Text_Desc()->vColor.z,
			pDesc.Get_UI_Text_Desc()->vColor.w
		};
		jObj["TextDesc"] = TextDesc;
	}

	const auto* children = pUI->Get_Children();
	if (children)
	{
		for (auto* pChild : *children)
		{
			Save_Hierarchy(pChild, jObj, false);
		}
	}
	
	if(bIsRoot)
		OutData = jObj;
	else
		OutData["Children"].push_back(jObj);
}

HRESULT CUIHUD::Load_Data(_wstring szLayerTag)
{
	Json jUIObjects;

	_wstring szFilePath{};
	szFilePath = TEXT("../Bin/DataFiles/UI/") + szLayerTag + TEXT(".json");

	_char szPath[MAX_PATH]{};
	CStringHelper::ConvertWideToUTF(szFilePath.c_str(), szPath);

	CJsonParser::ReadJsonData(szPath, jUIObjects);

	WCHAR szText[MAX_PATH]{};

	for (const auto& pUIObject : jUIObjects)
	{
		CUIBase::UIBASE_DESC Desc{};
		Desc.fAlpha = pUIObject["fAlpha"].get<_float>();
		Desc.fX = pUIObject["fX"].get<_float>();
		Desc.fY = pUIObject["fY"].get<_float>();
		Desc.fSizeX = pUIObject["fSizeX"].get<_float>();
		Desc.fSizeY = pUIObject["fSizeY"].get<_float>();
		Desc.fOffsetX = pUIObject["fOffsetX"].get<_float>();
		Desc.fOffsetY = pUIObject["fOffsetY"].get<_float>();
		Desc.iDepth = pUIObject["iDepth"].get<_uint>();
		Desc.iLevel = pUIObject["iLevel"].get<_uint>();
		CStringHelper::ConvertUTFToWide(pUIObject["szUITag"].get<string>().c_str(), szText);
		Desc.szUITag = szText;
		CStringHelper::ConvertUTFToWide(pUIObject["szLayerTag"].get<string>().c_str(), szText);
		Desc.szLayerTag = szText;
		CStringHelper::ConvertUTFToWide(pUIObject["szProtoTag"].get<string>().c_str(), szText);
		Desc.szProtoTag = szText;

		if (pUIObject.contains("TextureDesc"))
		{
			CUIBase::UI_TEXTURE_DESC TextureDesc{};
			Json jDesc = pUIObject["TextureDesc"];
			TextureDesc.iPass = jDesc["iPass"].get<_uint>();
			TextureDesc.iTextureIndex = jDesc["iTextureIndex"].get<_uint>();
			CStringHelper::ConvertUTFToWide(jDesc["szTextureComTag"].get<string>().c_str(), szText);
			TextureDesc.szTextureComTag = szText;
			CStringHelper::ConvertUTFToWide(jDesc["szProtoTag"].get<string>().c_str(), szText);
			TextureDesc.szProtoTag = szText;

			Desc.Set_UI_Texture_Desc(TextureDesc);
		}

		if (pUIObject.contains("TextDesc"))
		{
			CUIBase::UI_TEXT_DESC TextDesc{};
			Json jDesc = pUIObject["TextDesc"];
			//CStringHelper::ConvertUTFToWide(jDesc["szText"].get<string>().c_str(), szText);
			//TextDesc.szText = szText;
			TextDesc.szText = UTF8ToWString(jDesc["szText"].get<string>().c_str());
			TextDesc.vColor = {
				jDesc["vColor"][0].get<_float>(),
				jDesc["vColor"][1].get<_float>(),
				jDesc["vColor"][2].get<_float>(),
				jDesc["vColor"][3].get<_float>()
			};

			Desc.Set_UI_Text_Desc(TextDesc);
		}

		CGameObject* pUI = nullptr;

		if (FAILED(Add_UserInterface(Desc.iLevel, Desc.szProtoTag.c_str(), Desc.szLayerTag.c_str(), Desc.szUITag.c_str(), &pUI, &Desc)))
			return E_FAIL;

		CUIBase* pCreatedObj = dynamic_cast<CUIBase*>(pUI);

		if (!pCreatedObj)
			return E_FAIL;

		if (pUIObject.contains("Children"))
		{
			for (const auto& pChild : pUIObject["Children"])
			{
				Load_Hierarchy(pCreatedObj, pChild);
			}
		}
		else
			continue;
	}

	//MSG_BOX("읽기 성공");

	return S_OK;
}

void CUIHUD::Load_Hierarchy(CUIBase* pUIParent, Json jData)
{
	if (!pUIParent)
		return;

	WCHAR szText[MAX_PATH]{};

	CUIBase::UIBASE_DESC Desc{};
	Desc.fAlpha = jData["fAlpha"].get<_float>();
	Desc.fX = jData["fX"].get<_float>();
	Desc.fY = jData["fY"].get<_float>();
	Desc.fSizeX = jData["fSizeX"].get<_float>();
	Desc.fSizeY = jData["fSizeY"].get<_float>();
	Desc.fOffsetX = jData["fOffsetX"].get<_float>();
	Desc.fOffsetY = jData["fOffsetY"].get<_float>();
	Desc.iDepth = jData["iDepth"].get<_uint>();
	Desc.iLevel = jData["iLevel"].get<_uint>();
	CStringHelper::ConvertUTFToWide(jData["szUITag"].get<string>().c_str(), szText);
	Desc.szUITag = szText;
	CStringHelper::ConvertUTFToWide(jData["szLayerTag"].get<string>().c_str(), szText);
	Desc.szLayerTag = szText;
	CStringHelper::ConvertUTFToWide(jData["szProtoTag"].get<string>().c_str(), szText);
	Desc.szProtoTag = szText;

	if (jData.contains("TextureDesc"))
	{
		CUIBase::UI_TEXTURE_DESC TextureDesc{};
		Json jDesc = jData["TextureDesc"];
		TextureDesc.iPass = jDesc["iPass"].get<_uint>();
		TextureDesc.iTextureIndex = jDesc["iTextureIndex"].get<_uint>();
		CStringHelper::ConvertUTFToWide(jDesc["szTextureComTag"].get<string>().c_str(), szText);
		TextureDesc.szTextureComTag = szText;
		CStringHelper::ConvertUTFToWide(jDesc["szProtoTag"].get<string>().c_str(), szText);
		TextureDesc.szProtoTag = szText;

		Desc.Set_UI_Texture_Desc(TextureDesc);
	}

	if (jData.contains("TextDesc"))
	{
		CUIBase::UI_TEXT_DESC TextDesc{};
		Json jDesc = jData["TextDesc"];
		//CStringHelper::ConvertUTFToWide(jDesc["szText"].get<string>().c_str(), szText);
		//TextDesc.szText = szText;
		TextDesc.szText = UTF8ToWString(jDesc["szText"].get<string>().c_str());
		TextDesc.vColor = {
			jDesc["vColor"][0].get<_float>(),
			jDesc["vColor"][1].get<_float>(),
			jDesc["vColor"][2].get<_float>(),
			jDesc["vColor"][3].get<_float>()
		};

		Desc.Set_UI_Text_Desc(TextDesc);
	}

	CGameObject* pUI = nullptr;

	if (FAILED(Add_UserInterface(Desc.iLevel, Desc.szProtoTag.c_str(), Desc.szLayerTag.c_str(), Desc.szUITag.c_str(), &pUI, &Desc)))
		return;

	CUIBase* pCreatedObj = dynamic_cast<CUIBase*>(pUI);

	if (!pCreatedObj)
		return;

	if (pCreatedObj)
	{
		pCreatedObj->Set_Parent(pUIParent);
		pUIParent->Add_Child(pUI);
	}

	if (jData.contains("Children"))
	{
		for (const auto& pChild : jData["Children"])
		{
			Load_Hierarchy(pCreatedObj, pChild);
		}
	}
}

string CUIHUD::WStringToUTF8(const _wstring& wstr)
{
	if (wstr.empty()) return {};

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string result(size_needed - 1, 0); // null 제외
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size_needed, nullptr, nullptr);
	return result;
}

_wstring CUIHUD::UTF8ToWString(const string& str)
{
	if (str.empty()) return {};

	_uint size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	_wstring result(size_needed - 1, 0); // null 제외
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size_needed);
	return result;
}

CUIHUD* CUIHUD::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIHUD* pInstance = new CUIHUD(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIHUD::Free()
{
	__super::Free();
}
