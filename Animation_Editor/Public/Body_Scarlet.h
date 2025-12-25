#pragma once

#include "Animation_Editor_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Animation_Editor)

class CBody_Scarlet final : public CPartObject
{

public:
	typedef struct tagBody_Scarlet_Desc : public CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
	}BODY_SCARLET_DESC;
private:
	CBody_Scarlet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Scarlet(const CBody_Scarlet& Prototype);
	virtual ~CBody_Scarlet() = default;

public:
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName);
	_bool isFinish_Att();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:

private:
	const _uint* m_pParentState = { nullptr };

	_uint				m_iAnimationIndex;

	_bool				m_isAnimFinish = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Scarlet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END