#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer;
class CLight;
class CGameInstance;

class CLight_Manager final : public CBase
{
private:
#ifdef _DEBUG
	CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
#elif
	CLight_Manager();
#endif

	virtual ~CLight_Manager() = default;

public:
#ifdef _DEBUG
	HRESULT					Initialize();
	void					Debug_LightRender();
#endif
	
	const LIGHT_DESC*		Get_LightDesc(_uint iIndex) const;

public:
	HRESULT					Add_Light(const LIGHT_DESC& LightDesc);
	
	void					Remove_Light(CLight* pLight);
	HRESULT					Render_Lights(class CShader* pShader, class CVIBuffer* pVIBuffer);

private:
#ifdef _DEBUG
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*			m_pContext = nullptr;
	CGameInstance*					m_pGameInstance = nullptr;

	CVIBuffer*						m_pVIBuffer = nullptr;
	CTexture*						m_pTexture = nullptr;
	CShader*						m_pShader = nullptr;
#endif
	
	list<CLight*>					m_Lights;

public:
#ifdef _DEBUG
	static CLight_Manager*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
#elif
	static CLight_Manager*			Create();
#endif // _DEBUG

	virtual void					Free() override;
};

NS_END