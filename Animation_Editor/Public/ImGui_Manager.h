#pragma once

#include "Animation_Editor_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CAnimation;
NS_END

NS_BEGIN(Animation_Editor)

class CImGui_Manager final : public CBase
{
private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	HRESULT Render();

	void Set_Active(_bool bIsActive) {
		m_bIsActiveTrigger = bIsActive;
	};

private:
	void ShowGUI();
	void LoadFont();

	void Create_Character(const _wstring& szCharacterTag);
	void Create_Extra(const _wstring& szModelTag);
	void Kill_Character();

private:
	void Update_ToolBar();
	void Update_ToolBar_LoadCharacter();
	void Update_ToolBar_Save_Animation();
	void Update_ToolBar_Editor_Preferences();

	void Update_AnimationList();
	void Update_AnimNotifyList(class CAnimation* pAnimation);

	void Update_KeyFrameTool();
	void Update_TimeLine();
	void Update_TextureMap();
	void Update_EventMaker();


private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	class CGameInstance*				m_pGameInstance = { nullptr };
	class CTool_Manager*				m_pTool_Manager = { nullptr };

	CGameObject*						m_pSelectedObject = { nullptr };
	vector<CAnimation*>*				m_pAnimationList = { nullptr };

	vector<ANIM_NOTIFY>*							m_pCurrentAnimationEventList = { nullptr };
	unordered_map<_wstring, vector<ANIM_NOTIFY>>*	m_pAnimationEventMap = { nullptr };

	ImGuiTextFilter						m_AnimSearchFilter;

	_int								m_iSelectedAnimationIndex = 0;
	_int								m_iBeforeAnimationIndex = -1;

	_int								m_iSelectedEventIndex = -1;
	_int								m_iBeforeEventIndex = -1;

	_int								m_iSelectedMaterial = 0;

	_int								m_iClickedKeyFrame = -1;

	_bool								m_bIsActive;
	_bool								m_bIsActiveTrigger;

	_float								m_fRootMagnification = 0.f;
	_float								m_fTimeRate = 1.f;

public:
	static CImGui_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();

};

NS_END