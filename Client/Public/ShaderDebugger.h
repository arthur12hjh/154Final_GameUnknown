#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CLight;
NS_END

NS_BEGIN(Client)

class CGameManager;
class CImGuiManager;

class CShaderDebugger final : public CGameObject
{
private:
	CShaderDebugger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CShaderDebugger() = default;

public:
	HRESULT		Initialize_Prototype();
	HRESULT		Initialize();
	void		Update(_float fTimeDeleta);
	HRESULT		Render();

private:
	MOTIONBLUR_DESC* m_pMotioBlurDesc = {};
	FOG_DESC*    m_pFogDesc = {};
	BLOOM_DESC*  m_pBloomDesc = {};
	DOF_DESC*    m_pDoFDesc = {};
	SSAO_DESC*   m_pSSAODesc = {};
public:
	static CShaderDebugger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END