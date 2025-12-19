#pragma once

#include "Client_Defines.h"
#include "Component.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)

class CNotify final : public CComponent
{
public:
    enum NOTIFY_TYPE { PLAY_SFX, ACTIVE_SFX,
                       PLAY_SOUND, ACTIVE_COLLISION, ACTIVE_PHYSX_COLLISION,
                       SET_TRANSFORM, ACTIVE_PARTOBJECT_COLLISION,
                       HIT_REACTION, SPAWN_OBJECT, SET_RATIO,
                       SHOOT_PROJECTILE, PLAY_CINEMATIC, END };

    typedef struct tagNotifyDesc
    {
        class CCharacter* pCharacter = { nullptr };
    }NOTIFY_DESC;

private:
	CNotify(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNotify(const CNotify& Prototype);
	virtual ~CNotify() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	void Update(_float fTimeDelta);

    void AnimationChanged(const _char* szAnimationTag);
    void Set_ModelCom(class CModel* pModel);

private:
	class CGameManager* m_pGameManager = nullptr;

    class CCharacter* m_pCharacter = { nullptr };
    class CModel* m_pModelCom = ( nullptr );
    priority_queue<ANIM_NOTIFY> m_NotifyQueue;

private:
    HRESULT CallNotify(ANIM_NOTIFY AnimNotify);
    CNotify::NOTIFY_TYPE ClassificationNotify(const string& szNotifyTag);

    HRESULT Notify_Play_SFX(const ANIM_NOTIFY& AnimNotify);
    HRESULT Notify_Active_SFX(const ANIM_NOTIFY& AnimNotify);
    HRESULT Notify_Play_Sound(const ANIM_NOTIFY& AnimNotify);

    //기존 Collision을 활성화하는 Notify.
    HRESULT Notify_Active_Collision(const ANIM_NOTIFY& AnimNotify);
    //파트오브젝트의 콜라이더를 활성화 하기 위한 Notify
    HRESULT Notify_Active_PartObject_Collision(const ANIM_NOTIFY& AnimNotify);

    HRESULT Notify_Set_Transform(const ANIM_NOTIFY& AnimNotify);
    HRESULT Notify_Hit_Reaction(const ANIM_NOTIFY& AnimNotify);
    HRESULT Notify_Spawn_Object(const ANIM_NOTIFY& AnimNotify);
    HRESULT Notify_Shoot_Projectile(const ANIM_NOTIFY& AnimNotify);
    HRESULT Notify_Play_Cinematic(const ANIM_NOTIFY& AnimNotify);
    
public:
	static CNotify* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

NS_END

/*
    애니메이션 노티파이 Args
    **Play_SFX (이펙트)**
    - szNotifyArg01   : 프로토타입 태그 (ex : "Prototype_Component_Effect_Slash")
    - szNotifyArg08   : 파트 모델명 (없을 시 Body에 자동 부착)
    - szSocketTag     : 이펙트 부착 시, 부착시킬 소켓매트릭스 태그 None : 안붙이고, 안따라감. Transform : 안붙이고, 따라감. 나머지 : 붙이고, 따라감
    - vNotifyPosition : 보정된 생성 위치 ( y += 0.1 해줘야함 )
    - vNotifyRotation : 보정된 추가 회전값
    - vNotifyScale    : 보정된 추가 크기값

    **Active_SFX (이펙트)** :: 모델에 Clone되어있는 객체 활성화하는 함수. 
    - szNotifyArg01   : 매핑되어있는 문자열 태그. (무조건 그 객체에서도 매핑해줘야하고, 오버라이딩해줘야한다.)
    - szNotifyArg08   : 파트 모델명 (없을 시 Body에서 탐색)
    - fNumData01      : 활성 지속시간
    - szSocketTag     : 이펙트 부착 시, 부착시킬 소켓매트릭스 태그 None : 안붙이고, 안따라감. Transform : 안붙이고, 따라감. 나머지 : 붙이고, 따라감
    - vNotifyPosition : 보정된 생성 위치
    - vNotifyRotation : 보정된 추가 회전값
    - vNotifyScale    : 보정된 추가 크기값

    **Play_Sound**
    - szNotifyArg01   : 사운드 태그
    - szNotifyArg08   : 파트 모델명 (없을 시 Body에서 탐색)
    - fNumData01      : 볼륨
    - iNumData01      : 채널 (0 : EFFECT, 1 : BGM) << 나중에 바뀔수도
    - szSocketTag     : 방향성 사운드 구현 시 재생할 위치
    - vNotifyPosition : 보정된 생성 위치
    - vNotifyRotation : 보정된 추가 회전값

    **Active_Collision ( 전투 로직용 )**
    // 콜리전을 생성 Or 콜리전 On
	// ANIM_NOTIFY
	// szNotifyTag		=> Notify Event Type
	
	// Create Notify
	// szNotifyArg01	=> ProtoType Name
	// szNotifyArg02	=> Layer Name

	// iNumData1		=>	Skill ID
	// iNumData2		=>	Col Type
	// iNumData3		=>	Hit Box Type
	// iNumData4		=>	Hit Object Type

	// bIsLocalPos		=>	UseNotifyTransform

	// vNotifyScale		=> Hit Box Size
	// vNotifyPosition	=> Hit Box Relative Position
	// vNotifyRotation	=> Hit Box Rotation

    **Active_Collision ( 전투 로직용 )**
    // 콜리전을 생성 Or 콜리전 On
	// ANIM_NOTIFY
	- szNotifyTag		=> Notify Event Type
	
	// Create Notify
	-  szNotifyArg01	=> ProtoType Name
	-  szNotifyArg02	=> Layer Name

	-  iNumData1		=>	Skill ID
	-  iNumData2		=>	Col Type            (AABB, OBB, SPHERE)
	-  iNumData3		=>	Hit Box Type        (ALL, OBJECT, STATIC, PLAYER, MONSTER, SENCE, INTERACTION)
	-  iNumData4		=>	Hit Object Type     (ALL, OBJECT, STATIC, PLAYER, MONSTER, SENCE, INTERACTION)

    P_Eve_Sword_SlotNormal_ChainStab, P_Eve_Sword_Beta_ChargeSlash1_Ex

    Active_Collision
    Prototype_GameObject_AttackHitBox
    Hit_Box_Layer
    1006, 1007
    1
    3

    **Set_Transform**
    - szNotifyArg01   : 없음
    - szNotifyArg02   : 없음
    - szSocketTag     : 없음
    - bIsLocalPos     : 없음
    - vNotifyPosition : 객체 Look 기준으로 이동 할 위치
    - vNotifyRotation : 객체 Look 기준으로 회전 할 위치

    **Set_DynamicTransform**
    - szNotifyArg01   : 지속 시간
    - szNotifyArg02   : 보간 여부
    - szSocketTag     : 부착 시 해당 부착 매트릭스. _get일 시 CCharacter에서 받아올 것
    - bIsLocalPos     : vNotifyPosition, vNotifyRotation을 로컬 상으로 조정해줄지, 월드 상으로 조정해줄 지
    - vNotifyPosition : 시간(szNotifyArg01)까지 이동 할 위치
    - vNotifyRotation : 시간(szNotifyArg01)까지 회전 할 위치

    **Set_DeltaTimeSpeed - 한 애니메이션 내에서도 델타타임 배율 조정 가능. 다음 애니메이션 출력 시 초기화 **
    - szNotifyArg01   : 보정 시간
    - szNotifyArg02   : 없음
    - szSocketTag     : 없음
    - bIsLocalPos     : 없음
    - vNotifyPosition : 없음
    - vNotifyRotation : 없음

    **Adjust_Light - 기능 미구현. **
    - szNotifyArg01   : 없음
    - szNotifyArg02   : 없음
    - szSocketTag     : 없음
    - bIsLocalPos     : vNotifyPosition, vNotifyRotation을 로컬 상으로 조정해줄지, 월드 상으로 조정해줄 지
    - vNotifyPosition : 보정된 생성 위치
    - vNotifyRotation : 보정된 추가 회전값

    **Play_ScreenSFX - 기능 미구현**
    - szNotifyArg01   : 없음
    - szNotifyArg02   : 없음
    - szSocketTag     : 없음
    - bIsLocalPos     : vNotifyPosition, vNotifyRotation을 로컬 상으로 조정해줄지, 월드 상으로 조정해줄 지
    - vNotifyPosition : 보정된 생성 위치
    - vNotifyRotation : 보정된 추가 회전값

    **Undefined - 기능 미구현. 정의되지 않은 노티파이들을 의사에 따라 실행**
    - szNotifyArg01   : 없음
    - szNotifyArg02   : 없음
    - szSocketTag     : 없음
    - bIsLocalPos     : vNotifyPosition, vNotifyRotation을 로컬 상으로 조정해줄지, 월드 상으로 조정해줄 지
    - vNotifyPosition : 보정된 생성 위치
    - vNotifyRotation : 보정된 추가 회전값


*/