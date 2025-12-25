#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CJointChain final : public CComponent
{
private:
	CJointChain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJointChain(const CJointChain& Prototype);
	virtual ~CJointChain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Set_Root(class CRigidBody* pRigidBody);
	HRESULT Add_Joint(class CRigidBody* pRigidBody);
	HRESULT Add_Joint_Local(CRigidBody* pParent, CRigidBody* pChild, const PxTransform& tLocalPose);

	void* Get_RootDesc() { return &m_tRootDesc; }
	void* Get_JointDesc() { return &m_tJointDesc; }
	PxJoint* Get_PxJoint(_uint iIdx) { 
		if (iIdx > m_Joints.size() - 1) 
			return nullptr;
		
		return m_Joints[iIdx];
	}

public:
	void Update(_float fTimeDelta);

private:
	_uint m_iNumJoints = { 0 };
	class CRigidBody* m_pRoot = { nullptr };
	vector<class CRigidBody*> m_RigidBodies = {};
	vector<PxJoint*> m_Joints = {};

	JOINT_CHAIN_DESC m_tRootDesc = {};
	JOINT_CHAIN_DESC m_tJointDesc = {};

public:
	static CJointChain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
