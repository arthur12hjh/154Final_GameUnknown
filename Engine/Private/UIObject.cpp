#include "UIObject.h"

#include "GameInstance.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CUIObject::CUIObject(const CUIObject& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CUIObject::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CUIObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    _uint2 ScreenSize = m_pGameInstance->GetScreenSize();
    UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);   

    /* 뷰스페이스 상에 네모를 어떻게 배치시킬건지를 설정하는거다. */
    m_pTransformCom->Set_Scale(pDesc->fSizeX, pDesc->fSizeY, 1.f);
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(pDesc->fX - ScreenSize.x * 0.5f, -pDesc->fY + ScreenSize.y * 0.5f, 0.f, 1.f));

    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ScreenSize.x, ScreenSize.y, 0.f, 1.f));

    return S_OK;
}

void CUIObject::Priority_Update(_float fTimeDelta)
{
}

void CUIObject::Update(_float fTimeDelta)
{
}

void CUIObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIObject::Render()
{
    return S_OK;
}

void CUIObject::SetZOrder(_uint iZOrder)
{
    m_iZOrder = iZOrder;
}

void CUIObject::ComputeTransform(_vector vPosition)
{
    _matrix     ParentWorldMatrix{}, WorldMatrix{};
    ParentWorldMatrix = XMLoadFloat4x4(m_pParent->GetTransform()->Get_WorldMatrixPtr());

    switch (m_eDrawType)
    {
    case CUIObject::DRAW_TPYE::WORLD:
        m_pTransformCom->Set_State(STATE::POSITION, vPosition);
        break;
    case CUIObject::DRAW_TPYE::SCREEN:
    {
        _uint2 vHalfScreen = m_pGameInstance->GetHalfScreenSize();
        vPosition.m128_f32[0] = vPosition.m128_f32[0] - vHalfScreen.x;
        vPosition.m128_f32[1] = -vPosition.m128_f32[1] + vHalfScreen.y;
        m_pTransformCom->Set_State(STATE::POSITION, vPosition);
    }
        break;
    }

    WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    XMStoreFloat4x4(&m_CombinedMatrix, WorldMatrix * ParentWorldMatrix);
}

void CUIObject::Free()
{
    __super::Free();

}
