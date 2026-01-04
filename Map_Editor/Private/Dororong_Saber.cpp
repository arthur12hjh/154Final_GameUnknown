#include "pch.h"
#include "Dororong_Saber.h"
#include "GameInstance.h"

CDororong_Saber::CDororong_Saber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

CDororong_Saber::CDororong_Saber(const CDororong_Saber& Prototype) :
    CGameObject(Prototype)
{
}

HRESULT CDororong_Saber::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDororong_Saber::Initialize(void* pArg)
{

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CDororong_Saber::Priority_Update(_float fTimeDelta)
{
}

void CDororong_Saber::Update(_float fTimeDelta)
{
    
}

void CDororong_Saber::Late_Update(_float fTimeDelta)
{

}

HRESULT CDororong_Saber::Render()
{
    return S_OK;
}

CDororong_Saber* CDororong_Saber::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDororong_Saber* pDororong_Saber = new CDororong_Saber(pDevice, pContext);
    if (FAILED(pDororong_Saber->Initialize_Prototype()))
    {
        Safe_Release(pDororong_Saber);
        MSG_BOX("Create Fail : Dororong_Saber");
    }
    return pDororong_Saber;
}

CGameObject* CDororong_Saber::Clone(void* pArg)
{
    CDororong_Saber* pDororong_Saber = new CDororong_Saber(*this);
    if (FAILED(pDororong_Saber->Initialize(pArg)))
    {
        Safe_Release(pDororong_Saber);
        MSG_BOX("Create Fail : Dororong_Saber");
    }
    return pDororong_Saber;
}

void CDororong_Saber::Free()
{
    __super::Free();

    Safe_Release(m_pCollider);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
