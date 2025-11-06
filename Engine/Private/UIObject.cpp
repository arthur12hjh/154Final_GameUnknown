#include "UIObject.h"

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

    D3D11_VIEWPORT       ViewportDesc{};
    _uint                iNumViewports = { 1 };

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);   

    /* 뷰스페이스 상에 네모를 어떻게 배치시킬건지를 설정하는거다. */
    m_pTransformCom->Set_Scale(pDesc->fSizeX, pDesc->fSizeY, 1.f);
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(pDesc->fX - ViewportDesc.Width * 0.5f, -pDesc->fY + ViewportDesc.Height * 0.5f, 0.f, 1.f));

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

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

void CUIObject::Free()
{
    __super::Free();

}
