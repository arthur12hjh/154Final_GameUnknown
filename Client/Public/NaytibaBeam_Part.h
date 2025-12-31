#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CVIBuffer;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Client)
class CNaytibaBeam_Part final : public CPartObject
{
public:
	typedef struct tagBeam_Desc : public PARTOBJECT_DESC
	{
		const _float4x4*			pSocketMatrix = { nullptr };
	}BEAM_DESC;

private:
	CNaytibaBeam_Part(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNaytibaBeam_Part(const CNaytibaBeam_Part& Prototype);
	virtual ~CNaytibaBeam_Part() = default;

public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;

	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;

	virtual HRESULT						Render() override;

private:
	const _float4x4*					m_pSocketMatrix = { nullptr };

private:
	HRESULT								Ready_Components();
	HRESULT								Bind_ShaderResources();

public:
	static CNaytibaBeam_Part*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END