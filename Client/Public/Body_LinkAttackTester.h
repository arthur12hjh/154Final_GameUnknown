#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Client)

class CBody_LinkAttackTester final : public CPartObject
{

public:
	typedef struct tagBody_LinkAttackTester_Desc : public CPartObject::PARTOBJECT_DESC
	{
	}BODY_LINKATTACKTESTER_DESC;
private:
	CBody_LinkAttackTester(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_LinkAttackTester(const CBody_LinkAttackTester& Prototype);
	virtual ~CBody_LinkAttackTester() = default;

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
	CShader* m_pShaderCom = { nullptr };

private:

	_uint				m_iAnimationIndex;

	_bool				m_isAnimFinish = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_LinkAttackTester* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END