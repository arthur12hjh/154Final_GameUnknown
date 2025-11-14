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
	void Kill_Character();

private:
	void Update_ToolBar();
	void Update_ToolBar_LoadCharacter();
	void Update_ToolBar_Save_Animation();
	void Update_ToolBar_Editor_Preferences();

	void Update_AnimationList();

	void Update_KeyFrameTool();
	void Update_TimeLine();


private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	class CGameInstance*				m_pGameInstance = { nullptr };
	class CTool_Manager*				m_pTool_Manager = { nullptr };

	CGameObject*						m_pSelectedObject = { nullptr };
	vector<CAnimation*>*				m_pAnimationList = { nullptr };

	_bool								m_bIsActive;
	_bool								m_bIsActiveTrigger;

public:
	static CImGui_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();

};

NS_END