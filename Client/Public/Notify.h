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
                       ATTACK_INTERACTION, SET_TRANSFORM, ACTIVE_PARTOBJECT_COLLISION,
                       HIT_REACTION, SPAWN_OBJECT, SET_RATIO, CHANGE_COLOR, SET_VISIBLITY,
                       SHOOT_PROJECTILE, PLAY_CINEMATIC, CAMERA_SHAKE, END };

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

    //���� Collision�� Ȱ��ȭ�ϴ� Notify.
    HRESULT Notify_Active_Collision(const ANIM_NOTIFY& AnimNotify);
    //��Ʈ������Ʈ�� �ݶ��̴��� Ȱ��ȭ �ϱ� ���� Notify
    HRESULT Notify_Active_PartObject_Collision(const ANIM_NOTIFY& AnimNotify);

    HRESULT Notify_Set_Transform(const ANIM_NOTIFY& AnimNotify);
    HRESULT Notify_Hit_Reaction(const ANIM_NOTIFY& AnimNotify);
    HRESULT Notify_Spawn_Object(const ANIM_NOTIFY& AnimNotify);
    HRESULT Notify_Shoot_Projectile(const ANIM_NOTIFY& AnimNotify);
    HRESULT Notify_Play_Cinematic(const ANIM_NOTIFY& AnimNotify);

    HRESULT Notify_Camera_Shake(const ANIM_NOTIFY& AnimNotify);
    
public:
	static CNotify* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

NS_END

/*
    �ִϸ��̼� ��Ƽ���� Args
    **Play_SFX (����Ʈ)**
    - szNotifyArg01   : ������Ÿ�� �±� (ex : "Prototype_Component_Effect_Slash")
    - szNotifyArg08   : ��Ʈ �𵨸� (���� �� Body�� �ڵ� ����)
    - szSocketTag     : ����Ʈ ���� ��, ������ų ���ϸ�Ʈ���� �±� None : �Ⱥ��̰�, �ȵ���. Transform : �Ⱥ��̰�, ����. ������ : ���̰�, ����
    - vNotifyPosition : ������ ���� ��ġ ( y += 0.1 ������� )
    - vNotifyRotation : ������ �߰� ȸ����
    - vNotifyScale    : ������ �߰� ũ�Ⱚ

    **Active_SFX (����Ʈ)** :: �𵨿� Clone�Ǿ��ִ� ��ü Ȱ��ȭ�ϴ� �Լ�. 
    - szNotifyArg01   : ���εǾ��ִ� ���ڿ� �±�. (������ �� ��ü������ ����������ϰ�, �������̵�������Ѵ�.)
    - szNotifyArg08   : ��Ʈ �𵨸� (���� �� Body���� Ž��)
    - fNumData01      : Ȱ�� ���ӽð�
    - szSocketTag     : ����Ʈ ���� ��, ������ų ���ϸ�Ʈ���� �±� None : �Ⱥ��̰�, �ȵ���. Transform : �Ⱥ��̰�, ����. ������ : ���̰�, ����
    - vNotifyPosition : ������ ���� ��ġ
    - vNotifyRotation : ������ �߰� ȸ����
    - vNotifyScale    : ������ �߰� ũ�Ⱚ

    **Play_Sound**
    - szNotifyArg01   : ���� �±�
    - szNotifyArg08   : ��Ʈ �𵨸� (���� �� Body���� Ž��)
    - fNumData01      : ����
    - iNumData01      : ä�� (0 : EFFECT, 1 : BGM) << ���߿� �ٲ����
    - szSocketTag     : ���⼺ ���� ���� �� ����� ��ġ
    - vNotifyPosition : ������ ���� ��ġ
    - vNotifyRotation : ������ �߰� ȸ����

    **Active_Collision ( ���� ������ )**
    // �ݸ����� ���� Or �ݸ��� On
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

    **Active_Collision ( ���� ������ )**
    // �ݸ����� ���� Or �ݸ��� On
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
    - szNotifyArg01   : ����
    - szNotifyArg02   : ����
    - szSocketTag     : ����
    - bIsLocalPos     : ����
    - vNotifyPosition : ��ü Look �������� �̵� �� ��ġ
    - vNotifyRotation : ��ü Look �������� ȸ�� �� ��ġ

    **Set_DynamicTransform**
    - szNotifyArg01   : ���� �ð�
    - szNotifyArg02   : ���� ����
    - szSocketTag     : ���� �� �ش� ���� ��Ʈ����. _get�� �� CCharacter���� �޾ƿ� ��
    - bIsLocalPos     : vNotifyPosition, vNotifyRotation�� ���� ������ ����������, ���� ������ �������� ��
    - vNotifyPosition : �ð�(szNotifyArg01)���� �̵� �� ��ġ
    - vNotifyRotation : �ð�(szNotifyArg01)���� ȸ�� �� ��ġ

    **Set_DeltaTimeSpeed - �� �ִϸ��̼� �������� ��ŸŸ�� ���� ���� ����. ���� �ִϸ��̼� ��� �� �ʱ�ȭ **
    - szNotifyArg01   : ���� �ð�
    - szNotifyArg02   : ����
    - szSocketTag     : ����
    - bIsLocalPos     : ����
    - vNotifyPosition : ����
    - vNotifyRotation : ����

    **Adjust_Light - ��� �̱���. **
    - szNotifyArg01   : ����
    - szNotifyArg02   : ����
    - szSocketTag     : ����
    - bIsLocalPos     : vNotifyPosition, vNotifyRotation�� ���� ������ ����������, ���� ������ �������� ��
    - vNotifyPosition : ������ ���� ��ġ
    - vNotifyRotation : ������ �߰� ȸ����

    **Play_ScreenSFX - ��� �̱���**
    - szNotifyArg01   : ����
    - szNotifyArg02   : ����
    - szSocketTag     : ����
    - bIsLocalPos     : vNotifyPosition, vNotifyRotation�� ���� ������ ����������, ���� ������ �������� ��
    - vNotifyPosition : ������ ���� ��ġ
    - vNotifyRotation : ������ �߰� ȸ����

    **Undefined - ��� �̱���. ���ǵ��� ���� ��Ƽ���̵��� �ǻ翡 ���� ����**
    - szNotifyArg01   : ����
    - szNotifyArg02   : ����
    - szSocketTag     : ����
    - bIsLocalPos     : vNotifyPosition, vNotifyRotation�� ���� ������ ����������, ���� ������ �������� ��
    - vNotifyPosition : ������ ���� ��ġ
    - vNotifyRotation : ������ �߰� ȸ����


*/