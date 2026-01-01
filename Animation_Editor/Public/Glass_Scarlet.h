#pragma once

#include "Animation_Editor_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Animation_Editor)

class CGlass_Scarlet final : public CPartObject
{
public:
	typedef struct tagGlass_Scarlet_Desc : public CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
		const _float4x4* pSocketMatrix = { nullptr };
	}GLASS_DESC;
private:
	CGlass_Scarlet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGlass_Scarlet(const CGlass_Scarlet& Prototype);
	virtual ~CGlass_Scarlet() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	//CCollider*			m_pColliderCom = { nullptr };

	_float3					m_vRotationQuaternion;
	TCHAR						m_szRotationAngle[MAX_PATH] = {};

private:
	const _uint* m_pParentState = { nullptr };
	const _float4x4* m_pSocketMatrix = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CGlass_Scarlet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END