#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CTexture;
class CComponent;
class CLight;
class CGameInstance;

class CLight_Manager final : public CBase
{
private:
#ifdef _DEBUG
	CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
#else
	CLight_Manager();
#endif
	virtual ~CLight_Manager() = default;

public:
#ifdef _DEBUG
	HRESULT					Initialize();
	void					Debug_LightRender();
#endif

public:
	HRESULT					Add_Light(const LIGHT_DESC& LightDesc, CLight* pOutLight);
	CLight*					Find_Light(_uint iIndex);
	void					Clear_DeadLight();

	HRESULT					Render_Lights(class CShader* pShader, class CVIBuffer* pVIBuffer);
	const	list<CLight*>*	GetAllLight() { return &m_Lights; }

private:
#ifdef _DEBUG
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*			m_pContext = nullptr;
	CGameInstance*					m_pGameInstance = nullptr;

	CComponent*						m_pVIBuffer[2] = { nullptr , nullptr };
	CTexture*						m_pTexture = nullptr;
	CShader*						m_pShader[2] = { nullptr , nullptr };
#endif
	list<CLight*>					m_Lights;

public:
#ifdef _DEBUG
	static CLight_Manager*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
#else
	static CLight_Manager*			Create();
#endif // _DEBUG

	virtual void					Free() override;
};

NS_END