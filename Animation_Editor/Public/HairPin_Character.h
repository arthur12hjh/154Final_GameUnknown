#pragma once

#include "Animation_Editor_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Animation_Editor)

class CHairPin_Character final : public CPartObject
{
public:
	typedef struct tagHairPin_Character_Desc : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr };
	}HAIRPIN_CHARACTER_DESC;
private:
	CHairPin_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHairPin_Character(const CHairPin_Character& Prototype);
	virtual ~CHairPin_Character() = default;


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

private:
	const _float4x4* m_pSocketMatrix = { nullptr };

	_float3						m_vRotationQuaternion;
	_tchar						m_szRotationAngle[MAX_PATH];


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CHairPin_Character* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END