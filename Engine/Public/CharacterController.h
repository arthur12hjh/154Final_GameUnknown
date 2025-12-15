#pragma once

#include "Component.h"

NS_BEGIN(Engine)

/*Physx 캐릭터 컨트롤러 래핑용.*/
/*
캐릭터 컨트롤러는 콜라이더 회전이 불가능하고 Axis Aligned 형태로 제공됩니다.
이거 건드리려면 답없으니까 리지드 바디로 넘어가세요.

몬스터나 플레이어 같은 녀석들은 여기에 붙이세요.
*/

/* 이쪽 건드릴거 있으면 서민석한테 디코 주시면 됩니다. */
/* DxCollision 과 같이 쓸지는 고민중임.. */

class ENGINE_DLL CCharacterController final : public CComponent
{
public:
	// 볼록다각형은 로직이 많이 달라서 일단 보류.
	enum class CCT_SHAPE { BOX, CAPSULE, END };
public:
	typedef struct tagCharacterControllerDesc
	{
		CCT_SHAPE		eCharacterControllerType = { CCT_SHAPE::END };
		_float3			vSize = { 1.f, 1.f, 1.f };
		// 정마찰계수(미끄러지기 전의 마찰 계수. 높게 잡을수록 붙어있는 느낌이 강하다고 함.)
		// 동마찰계수 (이미 미끄러질떄의 마찰계수. 정마찰계수보다 조금 낮게한다고 함.), 
		// 반발계수 (탄성)
		_float3			vMaterial = { 0.5f, 0.5f, 0.6f };
		/* 시작할때 세팅할 위치. 반드시 채워줘야돼요. */
		_float4			vStartPos = { 0.f, 0.f, 0.f, 1.f };
		class CCTHitReporter* pHitReporter = { nullptr };
		class CCTBehaviorCallback* pBehaviorCallback = { nullptr };
		class CCTQueryFilterCallback* pQueryFilterCallback = { nullptr };

		PxUserData		tUserData = {};
		// 자신의 충돌쌍
		PxU32 iCollisionGroup = { PHYSX_CCT };
		// 자신이 충돌을 수행할 그룹.
		PxU32 iCollisionMask = { 0xFFFFFFFF };
	} CCT_DESC;

private:
	CCharacterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacterController(const CCharacterController& Prototype);
	virtual ~CCharacterController() = default;

public:
	PxController* Get_PxController() { return m_pController; }
	PxShape*	  Get_PxShape();
	PxRigidActor* Get_PxActor();
	void		  Set_Gravity(_bool bFlag, _float fVelocity);
 	_bool		  Get_Gravity() { return m_isGravity; }
	void		  Set_Riding(_bool isRiding, class CTransform* pTransform = nullptr, class CRigidBody* pRigidBody = nullptr);
	void		  Set_Active(_bool bFlag) { m_tUserData.isActive = bFlag; }
	void		  Set_Position(_vector vPosition);
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	/* 컨트롤러 트랜스폼 업데이트. 소유 객체한테서 매프레임 받아와야함 */
	void Update_PrePxPosition(class CTransform* pOwnerTransform);
	void Update_PxPosition(_float fTimeDelta, class CTransform* pOwnerTransform);
	void Update_ControllerTransform(_float fTimeDelta, class CTransform* pOwnerTransform);
	_vector Calc_Gravity(_float fTimeDelta);
private:
	/* RigidActor -> RigidBody & RigidStatic */
	/* RigidBody -> RigidDynamic & RigidArticulationLink */
	class CCTHitReporter* m_pHitReporter = { nullptr };
	class CCTBehaviorCallback* m_pBehaviorCallback = { nullptr };
	class CCTQueryFilterCallback* m_pQueryFilterCallback = { nullptr };
	class CCTFilterCallback* m_pCCTFilterCallback = { nullptr };

	PxControllerManager* m_pPxCCTManager = { nullptr };
	PxPhysics*			 m_pPxPhysics = { nullptr };
	PxController*		 m_pController = { nullptr };
	PxMaterial*			 m_pMaterial = { nullptr };

	CCT_SHAPE			 m_eShape = {};
	_float3				 m_vSize = { -1.f, -1.f, -1.f };
	PxVec3				 m_vPrePosition = { 0.f, 0.f, 0.f };
	PxVec3				 m_vPosition = { 0.f, 0.f, 0.f };
	PxUserData			 m_tUserData = {};


	_bool				 m_isGravity = { false };
	_float				 m_fGravityTimeAcc = { 0.f }; 
	_float				 m_fJumpVelocity = 0.f; // 현재 Y 속도 (점프 시 +, 낙하 시 -)
	_float				 m_fGravity = 30.f; // 중력 가속도 (아래로)

	_bool				 m_isRiding = { false };
	class CRigidBody*	 m_pRidingTarget = { nullptr };
	class CTransform*	 m_pRidingTargetTransform = { nullptr };
	PxVec3				 m_vHitPosition = {};

private:
	HRESULT Ready_CapsuleController(CCT_DESC* pDesc);
	HRESULT Ready_BoxController(CCT_DESC* pDesc);

public:
	static	CCharacterController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void		Free() override;
};

NS_END

/*메모용.*/
/*
캐릭터 컨트롤러는 매 프레임 위치 갱신만 해주면 됨.

리지드 바디는 매 프레임 위치 갱신해주고, 충돌 처리가 끝난 뒤 다시 원본 객체에 위치 갱신해주는 과정이 필요.
*/