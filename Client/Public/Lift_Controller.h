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
    enum class LIFT_CONTROLL_STATE 
    { 
        LIFT_UP,        // 리프트 플렛폼을 올리는 용도
        LIFT_DOWN,      // 리프트 플렛폼을 내리는 용도
        LIFT_END        // 아무상태 아님
    };

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

    // 이거 _bool 값으로 리프트 가운데 설치하는 컨트롤러는 이거 켜줘야합니다.
    _bool                               m_bIsControllLift = { false };

    // 컨트롤러가 이동시킬 리프트의 바닥 이라고 보시면 됩니다.
    // 진성햄이 Platform 이라고해둬서 제가 Platform으로 했어요
    CLift_Platform*                     m_pLiftPlatform = { nullptr };

    CModel*                             m_pModelCom = { nullptr };

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