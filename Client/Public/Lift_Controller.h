#include "Client_Defines.h"
#include "ClientStruct.h"

#include "Prob_Interaction.h"

NS_BEGIN(Engine)
class CModel;
class CCollider;
NS_END

NS_BEGIN(Client)

class CLift_Controller final : public CProb_Interaction
{
public:
    enum LIFT_STATE
    {
        LIFT_PULL, LIFT_PUSH, LIFT_END
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

private:
    LIFT_STATE                          m_eCurState = { LIFT_STATE::LIFT_END };

    CModel*                             m_pModelCom = { nullptr };
    CCollider*                          m_pColliderCom = { nullptr };

private:
    HRESULT                             Ready_Components(const _tchar* pComponentTag);
    HRESULT                             Bind_ShaderResources();

    void                                ResetAction(_bool bIsForce = false);

public:
    static CLift_Controller*            Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*                Clone(void* pArg) override;
    virtual void                        Free() override;
};

NS_END