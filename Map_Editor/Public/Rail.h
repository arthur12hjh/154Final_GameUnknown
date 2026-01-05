#pragma once
#include "Maptool_Defines.h"
#include "Dororong_Saber.h"

NS_BEGIN(Engine)

NS_END

NS_BEGIN(Tool_Map)
class CRail : public CDororong_Saber
{
private:
	CRail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRail(const CRail& Prototype);
	virtual ~CRail() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;

	virtual HRESULT			Render() override;

private:
	HRESULT					Ready_Components();
	HRESULT					Bind_ShaderResources();

private:
	_float					m_fTimeArr[3] = { 0.f, 0.f, 0.f };
	_float					m_fColorWeightArr[3] = { 0.f, 0.f, 0.f };
	_bool					m_bIsColorArr[3] = { false, false, false };
	_bool					m_bIsActive[3] = { false, false , false };


	_bool					m_bIsIdx0 = { false };
	_bool					m_bIsIdx1 = { false };
	_bool					m_bIsIdx2 = { false };



public:
	static CRail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END