#pragma once

#include "Animation_Editor_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Animation_Editor)

class CFacial_Extra final : public CPartObject
{

public:
	typedef struct tagFacial_Extra_Desc : public CPartObject::PARTOBJECT_DESC
	{
		_wstring szModelTag;
	}FACIAL_EXTRA_DESC;
private:
	CFacial_Extra(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFacial_Extra(const CFacial_Extra& Prototype);
	virtual ~CFacial_Extra() = default;

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
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

private:
	_wstring			m_szModelTag;
	_uint				m_iAnimationIndex;

	_bool				m_isAnimFinish = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CFacial_Extra* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END