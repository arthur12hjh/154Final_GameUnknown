#include "pch.h"
#include "UIHUD.h"
#include "HUDLayer.h"

#include "JsonParser.h"
#include "StringHelper.h"
#include "Interaction_Component.h"

#include "UIBase.h"
#include "UIAnimManager.h"
#include "GameInstance.h"
#include "Camera.h"

#include "UIBossVitalWrapper.h"
#include "UIWorldWrapper.h"
#include "UISimpleKey.h"

CUIHUD::CUIHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameHUD{ pDevice, pContext }
{
}

HRESULT CUIHUD::Initialize()
{
	m_pUIAnimMgr = CUIAnimManager::Create();
	
	if (!m_pUIAnimMgr)
		return E_FAIL;

	m_pGameInstance = CGameInstance::GetInstance();
	
	m_pUIAnimMgr->Load_Anim_Files();

	return S_OK;
}

void CUIHUD::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_pUIAnimMgr->Update(fTimeDelta);

	for(auto& pPool : m_WorldUIs)
	{
		for (auto& pObj : pPool.second)
		{
			CUIBase* pUIBase = dynamic_cast<CUIBase*>(pObj);
			if (pUIBase && pUIBase->GetVisibility() == VISIBILITY::VISIBLE)
			{
				pUIBase->Priority_Update(fTimeDelta);
				pUIBase->Update(fTimeDelta);
				pUIBase->Late_Update(fTimeDelta);
			}
		}
	}

	if (m_InteractionUIs.size() != m_pGameInstance->GetAllInteraction()->size())
	{
		_int count = m_InteractionUIs.size() - m_pGameInstance->GetAllInteraction()->size();

		for (_int i = 0; i < abs(count); ++i)
		{
			if (count < 0)
				Add_InteractionUI(m_InteractionUIs.size());
			else
				m_InteractionUIs[(m_InteractionUIs.size() - 1) - i]->SetVisibility(VISIBILITY::HIDDEN);
		}
	}

	_int count = 0;

	for (auto& pInteractionCom : *m_pGameInstance->GetAllInteraction())
	{
		dynamic_cast<CUIWorldWrapper*>(m_InteractionUIs[count])->Set_InteractionCom(pInteractionCom); 
		dynamic_cast<CUIWorldWrapper*>(m_InteractionUIs[count])->SetParent(pInteractionCom->GetOwner());
		m_InteractionUIs[count]->Set_TargetPos(&pInteractionCom->Get_CenterPos());
		m_InteractionUIs[count]->SetVisibility(VISIBILITY::VISIBLE);
		++count;
	}

	for (auto& pInteractionUI : m_InteractionUIs)
	{
		pInteractionUI->Priority_Update(fTimeDelta);
		pInteractionUI->Update(fTimeDelta);
		pInteractionUI->Late_Update(fTimeDelta);
	}
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

			if (!pUI->GetParent())
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

	CJsonParser::SaveJsonData(szPath, jUIObjects);
	
	MSG_BOX("저장 성공");

	return S_OK;
}

void CUIHUD::Set_Boss_Desc(const NAYTIBA_NETWORK_DESC* pNetworkDesc, const NAYTIBA_DESC* pNaytibaDesc)
{
	auto pLayer = m_pLayers.find(TEXT("Layer_Boss"));
	if (pLayer == m_pLayers.end())
		return;

	auto pUIObjects = pLayer->second->Get_UserInterfaces();
	auto pUI = pUIObjects->find(TEXT("Boss_Vital_Wrapper"));

	CUIBossVitalWrapper* pVitalWrapper = dynamic_cast<CUIBossVitalWrapper*>(pUI->second);
	if (!pVitalWrapper)
		return;

	pVitalWrapper->Set_Boss_Desc(pNetworkDesc, pNaytibaDesc);
	for (auto& pChild : *pVitalWrapper->Get_Children())
		pVitalWrapper->Update_Children(pChild);
	
}

CUIBase* CUIHUD::Get_UIObject(_wstring szLayerTag, _wstring szUITag)
{
	auto pLayer = m_pLayers.find(szLayerTag);

	if (pLayer == m_pLayers.end())
		return nullptr;

	auto pObj = pLayer->second->Get_UserInterfaces()->find(szUITag);

	CUIBase* pUI = dynamic_cast<CUIBase*>(pObj->second);

	return pUI;
}

void CUIHUD::Save_Hierarchy(CUIBase* pUI, Json& OutData, _bool bIsRoot)
{
	if (!pUI)
		return;

	UIBASE_DESC pDesc = pUI->Get_UIBase_Desc();

	_char szText[MAX_PATH]{};

	Json jObj;

	jObj["fAlpha"] = pDesc.fAlpha;
	jObj["fX"] = pDesc.fX;
	jObj["fY"] = pDesc.fY;
	jObj["fOffsetX"] = pDesc.fOffsetX;
	jObj["fOffsetY"] = pDesc.fOffsetY;
	jObj["fSizeX"] = pDesc.fSizeX;
	jObj["fSizeY"] = pDesc.fSizeY;
	jObj["fRotation"] = pDesc.fRotation;
	jObj["iLevel"] = pDesc.iLevel;
	jObj["iDepth"] = pDesc.iDepth;
	jObj["iRenderGroup"] = pDesc.iRenderGroup;
	jObj["iDrawType"] = pDesc.iDrawType;
	jObj["iVisibility"] = pUI->GetVisibility();
	CStringHelper::ConvertWideToUTF(pDesc.szUIID.c_str(), szText);
	jObj["szUIID"] = szText;
	CStringHelper::ConvertWideToUTF(pDesc.szUITag.c_str(), szText);
	jObj["szUITag"] = szText;
	CStringHelper::ConvertWideToUTF(pDesc.szLayerTag.c_str(), szText);
	jObj["szLayerTag"] = szText;
	CStringHelper::ConvertWideToUTF(pDesc.szProtoTag.c_str(), szText);
	jObj["szProtoTag"] = szText;

	if (pDesc.iDrawType == ENUM_CLASS(CUIObject::DRAW_TYPE::WORLD))
	{
		CStringHelper::ConvertWideToUTF(pDesc.szPoolTag.c_str(), szText);
		jObj["szPoolTag"] = szText;
	}

	if (pDesc.Get_ShaderDesc())
	{
		Json jShader;
		if (pDesc.m_tUIShaderDesc.bUseUV)
		{
			jShader["bUseUV"] = pDesc.m_tUIShaderDesc.bUseUV;
			jShader["fUVScaleX"] = pDesc.m_tUIShaderDesc.fUVScaleX;
			jShader["fUVScaleY"] = pDesc.m_tUIShaderDesc.fUVScaleY;
			jShader["fUVOffsetX"] = pDesc.m_tUIShaderDesc.fUVOffsetX;
			jShader["fUVOffsetY"] = pDesc.m_tUIShaderDesc.fUVOffsetY;
		}
		if (pDesc.m_tUIShaderDesc.bUseFillClip)
		{
			jShader["bUseFillClip"] = pDesc.m_tUIShaderDesc.bUseFillClip;
			jShader["fFillAmount"] = pDesc.m_tUIShaderDesc.fFillAmount;
		}
		if (pDesc.m_tUIShaderDesc.bUseTintColor)
		{
 			jShader["bUseTintColor"] = pDesc.m_tUIShaderDesc.bUseTintColor;
			jShader["vTintColor"] = {
				pDesc.m_tUIShaderDesc.vTintColor.x,
				pDesc.m_tUIShaderDesc.vTintColor.y,
				pDesc.m_tUIShaderDesc.vTintColor.z,
				pDesc.m_tUIShaderDesc.vTintColor.w
			};
		}
		if (pDesc.m_tUIShaderDesc.bDiscardBlack)
		{
			jShader["bDiscardBlack"] = pDesc.m_tUIShaderDesc.bDiscardBlack;
		}
		if (pDesc.m_tUIShaderDesc.bUseGlow)
		{
			jShader["bUseGlow"] = pDesc.m_tUIShaderDesc.bUseGlow;
			jShader["fGlowIntensity"] = pDesc.m_tUIShaderDesc.fGlowIntensity;
			jShader["fGlowSpread"] = pDesc.m_tUIShaderDesc.fGlowSpread;
		}
		if (pDesc.m_tUIShaderDesc.bUsePulseEffect)
		{
			jShader["bUsePulseEffect"] = pDesc.m_tUIShaderDesc.bUsePulseEffect;
			jShader["bUseScroll"] = pDesc.m_tUIShaderDesc.bUseScroll;
			jShader["fScrollSpeed"] = pDesc.m_tUIShaderDesc.fScrollSpeed;
			jShader["fPulseTime"] = pDesc.m_tUIShaderDesc.fPulseTime;
			jShader["fPulseSpeed"] = pDesc.m_tUIShaderDesc.fPulseSpeed;
		}
		if (pDesc.m_tUIShaderDesc.bUseScale)
		{
			jShader["bUseScale"] = pDesc.m_tUIShaderDesc.bUseScale;
			jShader["fScale"] = pDesc.m_tUIShaderDesc.fScale;
		}

		jObj["Shader"] = jShader;
	}

	for (auto& AnimTag : pDesc.m_AnimTags)
	{
		Json jAnim;

		CStringHelper::ConvertWideToUTF(AnimTag.first.c_str(), szText);
		jAnim["szAnimTag"] = szText;
		CStringHelper::ConvertWideToUTF(AnimTag.second.c_str(), szText);
		jAnim["szPrefabTag"] = szText;

		jObj["szAnimTags"].push_back(jAnim);
	}

	if (pDesc.m_Events.size() > 0)
	{
		Json jEvents;

		for (auto it = pDesc.m_Events.begin(); it != pDesc.m_Events.end(); ++it)
		{
			const vector<UI_EVENT_DESC>& Events = it->second;

			_char szEventTag[MAX_PATH]{};
			CStringHelper::ConvertWideToUTF(it->first.c_str(), szEventTag);

			Json jEventArray = Json::array();   // ⭐ KEY POINT: 매 키마다 새로운 배열

			for (size_t i = 0; i < Events.size(); ++i)
			{
				Json jEventInfo{};
				const auto& Event = Events[i];

				// SubscribeEventTags
				for (auto& SubscribeEvent : Event.szSubscribeEventTags)
				{
					_char szSubscribeEventTag[MAX_PATH]{};
					CStringHelper::ConvertWideToUTF(SubscribeEvent.c_str(), szSubscribeEventTag);

					jEventInfo["szSubscribeEventTags"].push_back(szSubscribeEventTag);
				}

				CStringHelper::ConvertWideToUTF(Event.szActionTag.c_str(), szText);
				jEventInfo["szActionTag"] = szText;

				CStringHelper::ConvertWideToUTF(Event.szTypeTag.c_str(), szText);
				jEventInfo["szTypeTag"] = szText;

				CStringHelper::ConvertWideToUTF(Event.szArg.c_str(), szText);
				jEventInfo["szArg"] = szText;

				jEventArray.push_back(jEventInfo);  // ⭐ 정상
			}

			jEvents[szEventTag] = jEventArray;   // ⭐ 각 키마다 독립된 배열 저장
		}

		jObj["szEvents"] = jEvents;
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

		TextDesc["szText"] = WStringToUTF8(pDesc.Get_UI_Text_Desc()->szText.c_str());
		TextDesc["fScale"] = pDesc.Get_UI_Text_Desc()->fScale;
		TextDesc["vColor"] = {
			pDesc.Get_UI_Text_Desc()->vColor.x,
			pDesc.Get_UI_Text_Desc()->vColor.y,
			pDesc.Get_UI_Text_Desc()->vColor.z,
			pDesc.Get_UI_Text_Desc()->vColor.w
		};
		//jObj["isHasTextDesc"] = pDesc.m_isHasTextDesc;
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
		UIBASE_DESC Desc{};
		Desc.fAlpha = pUIObject["fAlpha"].get<_float>();
		Desc.fX = pUIObject["fX"].get<_float>();
		Desc.fY = pUIObject["fY"].get<_float>();
		Desc.fSizeX = pUIObject["fSizeX"].get<_float>();
		Desc.fSizeY = pUIObject["fSizeY"].get<_float>();
		Desc.fOffsetX = pUIObject["fOffsetX"].get<_float>();
		Desc.fOffsetY = pUIObject["fOffsetY"].get<_float>();
		Desc.fRotation = pUIObject["fRotation"].get<_float>();
		Desc.iDepth = pUIObject["iDepth"].get<_uint>();
		Desc.iLevel = pUIObject["iLevel"].get<_uint>();
		Desc.iDrawType = pUIObject["iDrawType"].get<_uint>();
		Desc.iVisiblity = pUIObject["iVisibility"].get<_uint>();
		CStringHelper::ConvertUTFToWide(pUIObject["szUIID"].get<string>().c_str(), szText);
		Desc.szUIID = szText;
		CStringHelper::ConvertUTFToWide(pUIObject["szUITag"].get<string>().c_str(), szText);
		Desc.szUITag = szText;
		CStringHelper::ConvertUTFToWide(pUIObject["szLayerTag"].get<string>().c_str(), szText);
		Desc.szLayerTag = szText;
		CStringHelper::ConvertUTFToWide(pUIObject["szProtoTag"].get<string>().c_str(), szText);
		Desc.szProtoTag = szText;

		if (pUIObject.contains("szPoolTag"))
		{
			CStringHelper::ConvertUTFToWide(pUIObject["szPoolTag"].get<string>().c_str(), szText);
			Desc.szPoolTag = szText;
		}

		if (pUIObject.contains("Shader"))
		{
			Desc.m_isHasShaderDesc = true;
			UI_SHADER_DESC ShaderDesc{};

			Json jShader = pUIObject["Shader"];

			if (jShader.contains("bUseUV"))
			{
				ShaderDesc.bUseUV = jShader["bUseUV"].get<_bool>();
				ShaderDesc.fUVScaleX = jShader["fUVScaleX"].get<_float>();
				ShaderDesc.fUVScaleY = jShader["fUVScaleY"].get<_float>();
				ShaderDesc.fUVOffsetX = jShader["fUVOffsetX"].get<_float>();
				ShaderDesc.fUVOffsetY = jShader["fUVOffsetY"].get<_float>();
			}

			if (jShader.contains("bUseFillClip"))
			{
				ShaderDesc.bUseFillClip = jShader["bUseFillClip"].get<_bool>();
				ShaderDesc.fFillAmount = jShader["fFillAmount"].get<_float>();
			}

			if (jShader.contains("bUseTintColor"))
			{
				ShaderDesc.bUseTintColor = jShader["bUseTintColor"].get<_bool>();
				ShaderDesc.vTintColor = { 
					jShader["vTintColor"][0].get<_float>(),
					jShader["vTintColor"][1].get<_float>(),
					jShader["vTintColor"][2].get<_float>(),
					jShader["vTintColor"][3].get<_float>()
				};
			}

			if (jShader.contains("bDiscardBlack"))
			{
				ShaderDesc.bDiscardBlack = jShader["bDiscardBlack"].get<_bool>();
			}

			if (jShader.contains("bUseGlow"))
			{
				ShaderDesc.bUseGlow = jShader["bUseGlow"].get<_bool>();
				ShaderDesc.fGlowIntensity = jShader["fGlowIntensity"].get<_float>();
				ShaderDesc.fGlowSpread = jShader["fGlowSpread"].get<_float>();
			}

			if (jShader.contains("bUseScale"))
			{
				ShaderDesc.bUseScale = jShader["bUseScale"].get<_bool>();
				ShaderDesc.fScale = jShader["fScale"].get<_float>();
			}

			if (jShader.contains("bUsePulseEffect"))
			{
				ShaderDesc.bUsePulseEffect = jShader["bUsePulseEffect"].get<_bool>();
				if (jShader.contains("bUseScroll"))
				{
					ShaderDesc.bUseScroll = jShader["bUseScroll"].get<_bool>();
					ShaderDesc.fScrollSpeed = jShader["fScrollSpeed"].get<_bool>();
				}
				ShaderDesc.fPulseTime = jShader["fPulseTime"].get<_float>();
				ShaderDesc.fPulseSpeed = jShader["fPulseSpeed"].get<_float>();
			}

			Desc.m_tUIShaderDesc = ShaderDesc;
		}

		if (pUIObject.contains("szAnimTags"))
		{
			for (auto& AnimTag : pUIObject["szAnimTags"])
			{
				WCHAR szAnimTag[MAX_PATH]{};
				CStringHelper::ConvertUTFToWide(AnimTag["szAnimTag"].get<string>().c_str(), szAnimTag);
				WCHAR szPrefabTag[MAX_PATH]{};
				CStringHelper::ConvertUTFToWide(AnimTag["szPrefabTag"].get<string>().c_str(), szPrefabTag);

				Desc.m_AnimTags.emplace(szAnimTag, szPrefabTag);
			}
		}

		if (pUIObject.contains("szEvents"))
		{
			Desc.m_Events.clear();
			Json jEvents = pUIObject["szEvents"];

			// eventObj는 {"Hover": [...]} 또는 {"Click": [...]} 형태
			for (auto it = jEvents.begin(); it != jEvents.end(); ++it)
			{
				WCHAR wTrigger[MAX_PATH]{};
				CStringHelper::ConvertUTFToWide(it.key().c_str(), wTrigger);

				const auto& Events = it.value(); // Action 배열

				vector<UI_EVENT_DESC> parsedEvents{};

				for (const auto& Event : Events)
				{
					UI_EVENT_DESC EventDesc{};

					if (Event.contains("szSubscribeEventTags"))
					{
						for (const auto& SubscribeEvent : Event["szSubscribeEventTags"])
						{
							WCHAR szSubEventTag[MAX_PATH]{};
							CStringHelper::ConvertUTFToWide(SubscribeEvent.get<string>().c_str(), szSubEventTag);

							EventDesc.szSubscribeEventTags.push_back(szSubEventTag);
						}
					}

					WCHAR szActionTag[MAX_PATH]{};
					CStringHelper::ConvertUTFToWide(Event["szActionTag"].get<string>().c_str(), szActionTag);
					EventDesc.szActionTag = szActionTag;

					WCHAR szTypeTag[MAX_PATH]{};
					CStringHelper::ConvertUTFToWide(Event["szTypeTag"].get<string>().c_str(), szTypeTag);
					EventDesc.szTypeTag = szTypeTag;

					WCHAR szArg[MAX_PATH]{};
					CStringHelper::ConvertUTFToWide(Event["szArg"].get<string>().c_str(), szArg);
					EventDesc.szArg = szArg;

					parsedEvents.push_back(EventDesc);
				}

				// 최종 저장
				Desc.m_Events.emplace(wTrigger, parsedEvents);
			}
		}

		if (pUIObject.contains("TextureDesc"))
		{
			UI_TEXTURE_DESC TextureDesc{};
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
			UI_TEXT_DESC TextDesc{};
			Json jDesc = pUIObject["TextDesc"];
			//CStringHelper::ConvertUTFToWide(jDesc["szText"].get<string>().c_str(), szText);
			//TextDesc.szText = szText;
			TextDesc.szText = UTF8ToWString(jDesc["szText"].get<string>().c_str());
			TextDesc.fScale = jDesc["fScale"].get<_float>();
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

	UIBASE_DESC Desc{};
	Desc.fAlpha = jData["fAlpha"].get<_float>();
	Desc.fX = jData["fX"].get<_float>();
	Desc.fY = jData["fY"].get<_float>();
	Desc.fSizeX = jData["fSizeX"].get<_float>();
	Desc.fSizeY = jData["fSizeY"].get<_float>();
	Desc.fOffsetX = jData["fOffsetX"].get<_float>();
	Desc.fOffsetY = jData["fOffsetY"].get<_float>();
	Desc.fRotation = jData["fRotation"].get<_float>();
	Desc.iDepth = jData["iDepth"].get<_uint>();
	Desc.iLevel = jData["iLevel"].get<_uint>();
	Desc.iDrawType = jData["iDrawType"].get<_uint>();
	Desc.iVisiblity = jData["iVisibility"].get<_uint>();
	CStringHelper::ConvertUTFToWide(jData["szUIID"].get<string>().c_str(), szText);
	Desc.szUIID = szText;
	CStringHelper::ConvertUTFToWide(jData["szUITag"].get<string>().c_str(), szText);
	Desc.szUITag = szText;
	CStringHelper::ConvertUTFToWide(jData["szLayerTag"].get<string>().c_str(), szText);
	Desc.szLayerTag = szText;
	CStringHelper::ConvertUTFToWide(jData["szProtoTag"].get<string>().c_str(), szText);
	Desc.szProtoTag = szText;

	if (jData.contains("szPoolTag"))
	{
		CStringHelper::ConvertUTFToWide(jData["szPoolTag"].get<string>().c_str(), szText);
		Desc.szPoolTag = szText;
	}

	if (jData.contains("Shader"))
	{
		Desc.m_isHasShaderDesc = true;

		UI_SHADER_DESC ShaderDesc{};

		Json jShader = jData["Shader"];

		if (jShader.contains("bUseUV"))
		{
			ShaderDesc.bUseUV = jShader["bUseUV"].get<_bool>();
			ShaderDesc.fUVScaleX = jShader["fUVScaleX"].get<_float>();
			ShaderDesc.fUVScaleY = jShader["fUVScaleY"].get<_float>();
			ShaderDesc.fUVOffsetX = jShader["fUVOffsetX"].get<_float>();
			ShaderDesc.fUVOffsetY = jShader["fUVOffsetY"].get<_float>();
		}

		if (jShader.contains("bUseFillClip"))
		{
			ShaderDesc.bUseFillClip = jShader["bUseFillClip"].get<_bool>();
			ShaderDesc.fFillAmount = jShader["fFillAmount"].get<_float>();
		}

		if (jShader.contains("bUseTintColor"))
		{
			ShaderDesc.bUseTintColor = jShader["bUseTintColor"].get<_bool>();
			ShaderDesc.vTintColor = {
				jShader["vTintColor"][0].get<_float>(),
				jShader["vTintColor"][1].get<_float>(),
				jShader["vTintColor"][2].get<_float>(),
				jShader["vTintColor"][3].get<_float>()
			};
		}

		if (jShader.contains("bDiscardBlack"))
		{
			ShaderDesc.bDiscardBlack = jShader["bDiscardBlack"].get<_bool>();
		}

		if (jShader.contains("bUseScale"))
		{
			ShaderDesc.bUseScale = jShader["bUseScale"].get<_bool>();
			ShaderDesc.fScale = jShader["fScale"].get<_float>();
		}

		if (jShader.contains("bUseGlow"))
		{
			ShaderDesc.bUseGlow = jShader["bUseGlow"].get<_bool>();
			ShaderDesc.fGlowIntensity = jShader["fGlowIntensity"].get<_float>();
			ShaderDesc.fGlowSpread = jShader["fGlowSpread"].get<_float>();
		}

		if (jShader.contains("bUsePulseEffect"))
		{
			ShaderDesc.bUsePulseEffect = jShader["bUsePulseEffect"].get<_bool>();
			if (jShader.contains("bUseScroll"))
			{
				ShaderDesc.bUseScroll = jShader["bUseScroll"].get<_bool>();
				ShaderDesc.fScrollSpeed = jShader["fScrollSpeed"].get<_bool>();
			}
			ShaderDesc.fPulseTime = jShader["fPulseTime"].get<_float>();
			ShaderDesc.fPulseSpeed = jShader["fPulseSpeed"].get<_float>();
		}

		Desc.m_tUIShaderDesc = ShaderDesc;
	}

	if (jData.contains("szAnimTags"))
	{
		for (auto& AnimTag : jData["szAnimTags"])
		{
			WCHAR szAnimTag[MAX_PATH]{};
			CStringHelper::ConvertUTFToWide(AnimTag["szAnimTag"].get<string>().c_str(), szAnimTag);
			WCHAR szPrefabTag[MAX_PATH]{};
			CStringHelper::ConvertUTFToWide(AnimTag["szPrefabTag"].get<string>().c_str(), szPrefabTag);

			Desc.m_AnimTags.emplace(szAnimTag, szPrefabTag);
		}
	}

	if (jData.contains("szEvents"))
	{	
		Desc.m_Events.clear();
		Json jEvents = jData["szEvents"];

		for (auto it = jEvents.begin(); it != jEvents.end(); ++it)
		{
			WCHAR wTrigger[MAX_PATH]{};
			CStringHelper::ConvertUTFToWide(it.key().c_str(), wTrigger);

			const auto& Events = it.value(); // Action 배열

			vector<UI_EVENT_DESC> parsedEvents{};

			for (const auto& Event : Events)
			{
				UI_EVENT_DESC EventDesc{};

				if (Event.contains("szSubscribeEventTags"))
				{
					for (const auto& SubscribeEvent : Event["szSubscribeEventTags"])
					{
						WCHAR szSubEventTag[MAX_PATH]{};
						CStringHelper::ConvertUTFToWide(SubscribeEvent.get<string>().c_str(), szSubEventTag);

						EventDesc.szSubscribeEventTags.push_back(szSubEventTag);
					}
				}

				WCHAR szActionTag[MAX_PATH]{};
				CStringHelper::ConvertUTFToWide(Event["szActionTag"].get<string>().c_str(), szActionTag);
				EventDesc.szActionTag = szActionTag;

				WCHAR szTypeTag[MAX_PATH]{};
				CStringHelper::ConvertUTFToWide(Event["szTypeTag"].get<string>().c_str(), szTypeTag);
				EventDesc.szTypeTag = szTypeTag;

				WCHAR szArg[MAX_PATH]{};
				CStringHelper::ConvertUTFToWide(Event["szArg"].get<string>().c_str(), szArg);
				EventDesc.szArg = szArg;

				parsedEvents.push_back(EventDesc);
			}

			// 최종 저장
			Desc.m_Events.emplace(wTrigger, parsedEvents);
		}
	}

	if (jData.contains("TextureDesc"))
	{
		UI_TEXTURE_DESC TextureDesc{};
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
		UI_TEXT_DESC TextDesc{};
		Json jDesc = jData["TextDesc"];
		TextDesc.szText = UTF8ToWString(jDesc["szText"].get<string>().c_str());
		TextDesc.fScale = jDesc["fScale"].get<_float>();
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

	//if (Desc.iDrawType == ENUM_CLASS(CUIObject::DRAW_TYPE::SCREEN))
	{
		pCreatedObj->SetParent(pUIParent);
		pUIParent->Add_Child(pCreatedObj);
	}

	if (jData.contains("Children"))
	{
		for (const auto& pChild : jData["Children"])
		{
			Load_Hierarchy(pCreatedObj, pChild);
		}
	}
}

void CUIHUD::Reset_WorldUI_State(CUIBase* pUI)
{
	if (!pUI) return;
	// 알파/애니/이벤트 등의 런타임 상태 초기화
	UIBASE_DESC d = pUI->Get_UIBase_OriginDesc();
	d.fAlpha = 1.f;
	// 필요 시 더 초기화…
	pUI->Set_UIBase_Desc(d);

	m_pUIAnimMgr->Anim_Stop(pUI);
}

void CUIHUD::Anim_Play(_wstring szLayerTag, _wstring szUITag, _wstring szAnimTag, _float fDelay)
{
	auto pLayer = m_pLayers.find(szLayerTag);

	if (pLayer == m_pLayers.end())
		return;

	auto pObj = pLayer->second->Get_UserInterfaces()->find(szUITag);

	CUIBase* pUI = dynamic_cast<CUIBase*>(pObj->second);

	if (pUI == nullptr)
		return;

	auto AnimTag = pUI->Get_UIBase_Desc().m_AnimTags.find(szAnimTag);

	if (AnimTag == pUI->Get_UIBase_Desc().m_AnimTags.end())
		return;

	m_pUIAnimMgr->Anim_Play(pUI, AnimTag->second, fDelay);
}

void CUIHUD::Anim_Stop(_wstring szLayerTag, _wstring szUITag)
{
	auto pLayer = m_pLayers.find(szLayerTag);

	if (pLayer == m_pLayers.end())
		return;

	auto pObj = pLayer->second->Get_UserInterfaces()->find(szUITag);

	CUIBase* pUI = dynamic_cast<CUIBase*>(pObj->second);

	if (pUI == nullptr)
		return;

	m_pUIAnimMgr->Anim_Stop(pUI);
}

_bool CUIHUD::Check_AnimFinish(_wstring szLayerTag, _wstring szUITag, _wstring szAnimTag)
 {
	auto pLayer = m_pLayers.find(szLayerTag);

	if (pLayer == m_pLayers.end())
		return false;

	auto pObj = pLayer->second->Get_UserInterfaces()->find(szUITag);

	CUIBase* pUI = dynamic_cast<CUIBase*>(pObj->second);

	if (pUI == nullptr)
		return false;

	auto AnimTag = pUI->Get_UIBase_Desc().m_AnimTags.find(szAnimTag);

	if (AnimTag == pUI->Get_UIBase_Desc().m_AnimTags.end())
		return false;

	if(!m_pUIAnimMgr->Get_TargetAnim())
		m_pUIAnimMgr->Set_TargetAnim(pUI, AnimTag->second);

	return m_pUIAnimMgr->Check_Anim_Finish(pUI, AnimTag->second);
}

HRESULT CUIHUD::Register_WorldUI(const _wstring& szPoolTag, const _wstring& szUITag, _uint count, _uint level, const _wstring& layerTag)
{
	auto itLayer = m_pLayers.find(layerTag);
	if (itLayer == m_pLayers.end()) return E_FAIL;

	auto pObj = itLayer->second->Get_UserInterfaces()->find(szUITag);

	if (pObj->second == nullptr)
		return E_FAIL;

 	CUIBase* pUIBase{ dynamic_cast<CUIBase*>(pObj->second) };

	if (!pUIBase)
		return E_FAIL;
	Safe_AddRef(pUIBase);

	auto& pool = m_WorldUIs[szPoolTag];
   	pool.reserve(pool.size() + count);

	for (_uint i = 0; i < count; ++i) {
 		CUIBase* pUI = pUIBase->Clone_UI(this, i);
		
		if (!pUI)
			return E_FAIL;

		Reset_WorldUI_State(pUI);
		pUI->SetVisibility(VISIBILITY::HIDDEN);

		pool.push_back(pUI);
		Safe_AddRef(pUI);
	}
	Safe_Release(pUIBase);

	return S_OK;
}

HRESULT CUIHUD::Add_InteractionUI(_int iIdx)
{
	auto itLayer = m_pLayers.find(TEXT("Layer_World"));
	if (itLayer == m_pLayers.end()) return E_FAIL;

	auto pObj = itLayer->second->Get_UserInterfaces()->find(TEXT("UI_Simple_Interaction"));

	if (pObj->second == nullptr)
		return E_FAIL;

 	CUIBase* pUIBase{ dynamic_cast<CUIBase*>(pObj->second) };

	if (!pUIBase)
		return E_FAIL;
	Safe_AddRef(pUIBase);

	CUIBase* pUI = pUIBase->Clone_UI(this, iIdx);
	Safe_AddRef(pUI);

	m_InteractionUIs.push_back(pUI);

	Safe_Release(pUIBase);

	return S_OK;
}

void CUIHUD::Remove_InteractionUI(CUIBase* pUI)
{
	auto iter = find(m_InteractionUIs.begin(), m_InteractionUIs.end(), pUI);
	if (iter != m_InteractionUIs.end())
	{
		m_InteractionUIs.erase(iter);
		pUI->SetVisibility(VISIBILITY::HIDDEN);
	}
}

CUIBase* CUIHUD::Rent_WorldUI(const _wstring& poolKey, CGameObject* pParent, const _float3* vTargetPos, _bool bBillboard)
{
	auto it = m_WorldUIs.find(poolKey);
	if (it == m_WorldUIs.end() || it->second.empty()) return nullptr;

	// 맨 뒤에서 꺼내기
	CUIBase* pUI = it->second.back();
	it->second.pop_back();

	// 상태 초기화 후 사용할 준비
	Reset_WorldUI_State(pUI);
	pUI->Set_DrawType((CUIObject::DRAW_TYPE)pUI->Get_UIBase_Desc().iDrawType);
	pUI->SetVisibility(VISIBILITY::VISIBLE);

	if (pParent) {
		pUI->SetParent(pParent);
		pUI->Set_TargetPos(vTargetPos);
	}

	return pUI;
}

void CUIHUD::Return_WorldUI(CUIBase*& pUI)
{
	if (!pUI) return;

	// Parent/계층/애니/가시성 원복
	m_pUIAnimMgr->Anim_Stop(pUI);
	pUI->SetVisibility(VISIBILITY::HIDDEN);

	for (auto& pChild : *pUI->Get_Children())
		pUI->Update_Children(pChild);

	pUI->SetParent(nullptr);
	pUI->Set_TargetPos(nullptr );

	_wstring szPoolTag = pUI->Get_UIBase_Desc().szPoolTag;

	m_WorldUIs[szPoolTag].push_back(pUI);
	pUI = nullptr;
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

	Safe_Release(m_pUIAnimMgr);

	for (auto& pPools : m_WorldUIs)
	{
		for (auto& pPool : pPools.second)
			Safe_Release(pPool);
	}

	for (auto& pUI : m_InteractionUIs)
	{
		Safe_Release(pUI);
	}
}

