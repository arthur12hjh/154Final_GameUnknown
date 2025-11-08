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
private:
	CImGuiManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImGuiManager() = default;

public:
	HRESULT									Initialize();

	void									Update(_float fTimeDeleta);
	HRESULT									Render();

	
	void									SetImGuiObjectVisiblilty(const WCHAR* ImGuiTag, VISIBILITY eVisiblility);

private :
	ID3D11Device*							m_pDevice = { nullptr };
	ID3D11DeviceContext*					m_pContext = { nullptr };
	CGameInstance*							m_pGameInstance = { nullptr };

	map<const _wstring, CGameObject*>		m_ImGuis;

private :
	HRESULT									ADD_ImGuiObject();
	CGameObject*							Find_ImGuiObject(const WCHAR* ImGuiTag);

public:
	static CImGuiManager*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void							Free() override;

};
NS_END