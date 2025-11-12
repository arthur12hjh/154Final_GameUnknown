#include "pch.h"
#include "UIHUD.h"
#include "HUDLayer.h"

#include "JsonParser.h"
#include "StringHelper.h"

#include "GameInstance.h"

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
	
	Json jUIObjects = Json::array();

	if (pUIObjects)
	{
		for (auto& pUIObj : *pUIObjects)
		{
			CUIBase* pUI = dynamic_cast<CUIBase*>(pUIObj.second);

			Json jObj;

			if (!pUI->Get_Parent())
			{
				Save_Hierarchy(pUI, jObj);
				jUIObjects.push_back(jObj);
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

	return S_OK;
}

void CUIHUD::Save_Hierarchy(CUIBase* pUI, Json& OutData)
{
	if (!pUI)
		return;

	for (auto& child : *pUI->Get_Children())
	{
		CUIBase::UIBASE_DESC pDesc = pUI->Get_UIBase_OriginDesc();

		_char szText[MAX_PATH];

		OutData["fOffsetX"] = pDesc.fOffsetX;
		OutData["fOffsetY"] = pDesc.fOffsetY;
		OutData["iLevel"] = pDesc.iLevel;
		OutData["iDepth"] = pDesc.iDepth;
		OutData["iRenderGroup"] = pDesc.iRenderGroup;
		CStringHelper::ConvertWideToUTF(pDesc.szUITag.c_str(), szText);
		OutData["szUITag"] = szText;
		CStringHelper::ConvertWideToUTF(pDesc.szLayerTag.c_str(), szText);
		OutData["szLayerTag"] = szText;

		if (pDesc.Get_UI_Texture_Desc())
		{
			Json TextureDesc;

			CStringHelper::ConvertWideToUTF(pDesc.Get_UI_Texture_Desc()->szTextureComTag.c_str(), szText);
			TextureDesc["szTextureComTag"] = szText;
			TextureDesc["iTextureIndex"] = pDesc.Get_UI_Texture_Desc()->iTextureIndex;
			TextureDesc["iPass"] = pDesc.Get_UI_Texture_Desc()->iPass;
			OutData["TextureDesc"] = TextureDesc;
		}

		if (pDesc.Get_UI_Text_Desc())
		{
			Json TextDesc;

			CStringHelper::ConvertWideToUTF(pDesc.Get_UI_Text_Desc()->szText.c_str(), szText);

			TextDesc["szText"] = szText;
			TextDesc["vColor"] = {
				pDesc.Get_UI_Text_Desc()->vColor.x,
				pDesc.Get_UI_Text_Desc()->vColor.y,
				pDesc.Get_UI_Text_Desc()->vColor.z,
				pDesc.Get_UI_Text_Desc()->vColor.w
			};
			OutData["TextDesc"] = TextDesc;
		}
	}
}

HRESULT CUIHUD::Load_Data(const _tchar* szFilePath)
{
	

	return S_OK;
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
