#pragma once
#include "Maptool_Defines.h"
#include "Dororong_Saber.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Tool_Map)
class CDororongBox : public CDororong_Saber
{
private:
	CDororongBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDororongBox(const CDororongBox& Prototype);
	virtual ~CDororongBox() = default;

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

public:
	static CDororongBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END