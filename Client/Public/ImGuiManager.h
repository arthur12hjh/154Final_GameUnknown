#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;

NS_END

NS_BEGIN(Client)
class CImGuiManager : public CBase
{
	DECLARE_SINGLETON(CImGuiManager);

private :
	CImGuiManager() = default;
	virtual ~CImGuiManager() = default;

public:
	HRESULT									Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void									Update(_float fTimeDeleta);
	HRESULT									Render();

	CGameObject*							Find_ImGuiObject(const WCHAR* ImGuiTag);
	void									SetImGuiObjectVisiblilty(const WCHAR* ImGuiTag, VISIBILITY eVisiblility);

	void									SetLevelFreeCamera();

private :
	ID3D11Device*							m_pDevice = { nullptr };
	ID3D11DeviceContext*					m_pContext = { nullptr };
	CGameInstance*							m_pGameInstance = { nullptr };

	map<const _wstring, CGameObject*>		m_ImGuis;

private :
	HRESULT									ADD_ImGuiObject();
	
public:
	virtual void							Free() override;

};
NS_END