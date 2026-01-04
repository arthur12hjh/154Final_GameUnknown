#pragma once
#include "Maptool_Defines.h"
#include "Dororong_Saber.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CVIBuffer_Cube;
class CCollider;
NS_END

NS_BEGIN(Tool_Map)
class CPad : public CDororong_Saber
{
private:
	CPad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPad(const CPad& Prototype);
	virtual ~CPad() = default;

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
	static CPad* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END