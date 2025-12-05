#include "Client_Defines.h"
#include "ClientStruct.h"

#include "Prob_Interaction.h"

NS_BEGIN(Engine)
class CModel;
class CCollider;
NS_END

NS_BEGIN(Client)
class CLift_Platform;

class CLift_Controller final : public CProb_Interaction
{
public:
    enum class LIFT_ANIM_STATE { LIFT_ANIM_PULL, LIFT_ANIM_PUSH, LIFT_ANIM_END };
    enum class LIFT_CONTROLL_STATE { LIFT_UP, LIFT_DOWN, LIFT_END };

private:
    CLift_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CLift_Controller(const CLift_Controller& Prototype);
    virtual ~CLift_Controller() = default;

public:
    virtual HRESULT                     Initialize_Prototype() override;
    virtual HRESULT                     Initialize(void* pArg) override;

    virtual void                        Priority_Update(_float fTimeDelta) override;
    virtual void                        Update(_float fTimeDelta) override;
    virtual void                        Late_Update(_float fTimeDelta) override;

    virtual HRESULT                     Render() override;
    void                                SetControllPlatform(CLift_Platform* pControllPlatform);

private:
    LIFT_ANIM_STATE                     m_eCurState = { LIFT_ANIM_STATE::LIFT_ANIM_END };
    LIFT_CONTROLL_STATE                 m_eControllState = { LIFT_CONTROLL_STATE::LIFT_END };

    _bool                               m_bIsControllLift = { false };
    CLift_Platform*                     m_pLiftPlatform = { nullptr };
    CModel*                             m_pModelCom = { nullptr };
    CRigidBody*                         m_pRigidBodyCom = { nullptr }; 
private:
    HRESULT                             Ready_Components(const _tchar* pComponentTag);
    HRESULT                             Bind_ShaderResources();

    virtual HRESULT					    Begin_OverlapCallBack() override;
    virtual HRESULT					    End_OverlapCallBack() override;
    virtual void					    Excute_CallBack(CGameObject* pActionObject) override;

    void                                ResetAction(_bool bIsForce = false);

public:
    static CLift_Controller*            Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*                Clone(void* pArg) override;
    virtual void                        Free() override;
};

NS_END