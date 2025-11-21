#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CModel;
class CCollider;
class CNavigation;
class CRigidBody;
class CCharacterController;
NS_END

/*
이 객체 건드리고싶으면 서민석한테 디코 ㄱㄱ
*/

NS_BEGIN(Client)

class CShaderTestModel final : public CGameObject
{
private:
	CShaderTestModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShaderTestModel(const CShaderTestModel& Prototype);
	virtual ~CShaderTestModel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	virtual HRESULT Render_MotionBlur() override;
private:
	CCharacterController* m_pCCT = { nullptr };
	CModel*				  m_pModelCom = { nullptr };
	CShader*			  m_pShaderCom = { nullptr };
	_uint				  m_iShaderPassIdx = { 2 };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CShaderTestModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END