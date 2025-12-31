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