#pragma once

#include "Component.h"

NS_BEGIN(Engine)

/*Physx 리지드 바디 래핑용.*/
/* 이쪽 건드릴거 있으면 서민석한테 디코 주시면 됩니다. */
/* DxCollision 과 같이 쓸지는 고민중임.. */

/*
씬에서 제거하려면 Get_RigidBody해서 가져온담에 씬 내에서 제거..
*/

class ENGINE_DLL CRigidBody final : public CComponent
{
public:
	//kinetic은 DYNAMIC의 flag로서 설정됨. 키네마틱 어중간하게 쓸거면 캐릭터 컨트롤러 쓰세요.
	enum class RIGIDBODY_TYPE	{ DYNAMIC, KINEMATIC, STATIC, END };
	// 볼록다각형은 로직이 많이 달라서 일단 보류.
	enum class RIGIDBODY_SHAPE	{ BOX, SPHERE, CAPSULE, PLANE, CONVEX, END };
public:
	typedef struct tagRigidBodyDesc
	{
		RIGIDBODY_TYPE	eRigidBodyType = { RIGIDBODY_TYPE::END };
		RIGIDBODY_SHAPE eRigidBodyShape = { RIGIDBODY_SHAPE::END };
		_float3			vSize = { 1.f, 1.f, 1.f };
		// 정마찰계수(미끄러지기 전의 마찰 계수. 높게 잡을수록 붙어있는 느낌이 강하다고 함.)
		// 동마찰계수 (이미 미끄러질떄의 마찰계수. 정마찰계수보다 조금 낮게한다고 함.), 
		// 반발계수 (탄성)
		_float3			vMaterial = { 0.5f, 0.5f, 0.6f };
		/* 시작할때 세팅할 월드 매트릭스. 반드시 채워줘야돼요. */
		_float4x4		StartWorldMatrix = {};
	} RIGIDBODY_DESC;

private:
	CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidBody(const CRigidBody& Prototype);
	virtual ~CRigidBody() = default;

public:
	PxRigidActor* Get_PxRigidBody() { return m_pPxRigidBody; }
	PxTransform*  Get_PxTransform() { return m_pPxTransform; }
	void		  Set_PxTransform(PxTransform& Transform) { *m_pPxTransform = Transform; }
	
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	/* 리지드 바디 트랜스폼 업데이트. 소유 객체한테서 매프레임 받아와야함. */
	void Update_PxTransform(_fmatrix vWorldMatrix);

private:
	/* RigidActor -> RigidBody & RigidStatic */
	/* RigidBody -> RigidDynamic & RigidArticulationLink */
	PxPhysics*		m_pPxPhysics = { nullptr };
	PxRigidActor*	m_pPxRigidBody = { nullptr };
	PxTransform*	m_pPxTransform = { nullptr };
	PxMaterial*		m_pMaterial = { nullptr };
	PxShape*		m_pShape = { nullptr };

	RIGIDBODY_TYPE	m_eType = {};
	RIGIDBODY_SHAPE m_eShape = {};
	_float3			m_vSize = { -1.f, -1.f, -1.f };

private:
	HRESULT Ready_PxTransform(RIGIDBODY_DESC* pDesc);
	HRESULT Ready_PxMaterial(RIGIDBODY_DESC* pDesc);
	HRESULT Ready_PxShape(RIGIDBODY_DESC* pDesc);
	HRESULT Ready_PxRigidBody(RIGIDBODY_DESC* pDesc);

public:
	static	CRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void		Free() override;
};

NS_END

/*메모용.*/
/*
캐릭터 컨트롤러는 매 프레임 위치 갱신만 해주면 됨.

리지드 바디는 매 프레임 위치 갱신해주고, 충돌 처리가 끝난 뒤 다시 원본 객체에 위치 갱신해주는 과정이 필요.
*/