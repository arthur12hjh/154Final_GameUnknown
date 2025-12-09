#pragma once

#include "Client_Defines.h"
#include "GameHUD.h"

NS_BEGIN(Engine)
class CHUDLayer;
class CTexture;
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CUIBase;
class CUIAnimManager;

class CUIHUD final : public CGameHUD
{
private:
	CUIHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUIHUD() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;

	unordered_map<_wstring, CHUDLayer*> Get_Layers() { return m_pLayers; }

	HRESULT Save_Data(_wstring szLayerTag);
	HRESULT Load_Data(_wstring szLayerTag);

	void Set_Show_Debug_Rect(_bool bShow) {
		m_bShowDebugRect = bShow;
	}

	_bool Get_Show_Debug_Rect() { return m_bShowDebugRect;}

	CUIAnimManager* Get_AnimMgr() { return m_pUIAnimMgr; }

	void Anim_Play(_wstring szLayerTag, _wstring szUITag, _wstring szAnimTag, _float fDelay = 0.f);
	void Anim_Stop(_wstring szLayerTag, _wstring szUITag);
	_bool Check_AnimFinish(_wstring szLayerTag, _wstring szUITag, _wstring szAnimTag);

	// World UI
	HRESULT Register_WorldUI(const _wstring& szPoolTag, const _wstring& szUITag,
								_uint count, _uint level = 0,
								const _wstring& layerTag = TEXT("World_Layer"));

	CUIBase* Rent_WorldUI(const _wstring& szPoolTag,
							CGameObject* pParent,
							const _float3* vTargetPos = nullptr,
							_bool bBillboard = true);

	void Return_WorldUI(CUIBase*& pUI);

	// Interaction UI
	HRESULT Add_InteractionUI(_int iIdx);
	void Remove_InteractionUI(CUIBase* pUI);

	void Set_Boss_Desc(const NAYTIBA_NETWORK_DESC* pNetworkDesc, const NAYTIBA_DESC* pNaytibaDesc);
	CUIBase* Get_UIObject(_wstring szLayerTag, _wstring szUITag);

	// 한글 때문에 만듦
	string WStringToUTF8(const _wstring& wstr);
	_wstring UTF8ToWString(const string& str);

private:
	void Save_Hierarchy(CUIBase* pUI, Json& OutData, _bool bIsRoot);
	void Load_Hierarchy(CUIBase* pUIParent, Json jData);

	void Reset_WorldUI_State(CUIBase* pUI);

private:
	

	_bool m_bShowDebugRect{ true };

	CUIAnimManager* m_pUIAnimMgr{ nullptr };

	unordered_map<_wstring, vector<CUIBase*>> m_WorldUIs{};
	vector<CUIBase*> m_InteractionUIs{};

	CGameInstance* m_pGameInstance{ nullptr };

public:
	static CUIHUD* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END