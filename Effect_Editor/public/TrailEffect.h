#pragma once

#include "Tool_Effect_Defines.h"
#include "GameObject.h"
#include "TrailData.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CShader;
class CTrail;
NS_END

NS_BEGIN(Tool_Effect)

class CTrailEffect final : public CGameObject
{
public:
private:
	CTrailEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrailEffect(const CTrailEffect& Prototype);
	virtual ~CTrailEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	HRESULT Render(class CTrailData* pTrailData);
	void Refresh();
	_float	Get_Speed() { return m_fSpeed; }
	void	Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; }
private:
	CShader* m_pShaderCom = { nullptr };
	CTrail*		m_pTrail = { nullptr };
	CTrail* m_pTrail2[3] = {nullptr};
	_float		m_fSpeed = { 1.f };
	_float		m_fTime = {};
	_uint	m_iSelect = {};
	RENDER	m_eRender = {};

private:
	HRESULT							Ready_Components();
	HRESULT							Bind_ShaderResources(CTrailData::TRAIL_DATA tData);

public:
	static CTrailEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END