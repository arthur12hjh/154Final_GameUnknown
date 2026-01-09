#include "pch.h"
#include "SoundTriggerBox.h"

CSoundTriggerBox::CSoundTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

CSoundTriggerBox::CSoundTriggerBox(const CSoundTriggerBox& Prototype) :
    CGameObject(Prototype)
{
}

HRESULT CSoundTriggerBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSoundTriggerBox::Initialize(void* pArg)
{
    return S_OK;
}

void CSoundTriggerBox::Priority_Update(_float fTimeDelta)
{
}

void CSoundTriggerBox::Update(_float fTimeDelta)
{
}

void CSoundTriggerBox::Late_Update(_float fTimeDelta)
{
}

HRESULT CSoundTriggerBox::Render()
{
    return S_OK;
}

HRESULT CSoundTriggerBox::Ready_Components(const TRIGGER_BOX_DESC& pDesc)
{
    return S_OK;
}

void CSoundTriggerBox::Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
}

void CSoundTriggerBox::OverlappingEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
}

void CSoundTriggerBox::End_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
}

CSoundTriggerBox* CSoundTriggerBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSoundTriggerBox* SoundTriggerBox = new CSoundTriggerBox(pDevice, pContext);
    if (FAILED(SoundTriggerBox->Initialize_Prototype()))
    {
        Safe_Release(SoundTriggerBox);
        MSG_BOX("Create Fail : Trigger Box");
    }
    return SoundTriggerBox;
}

CGameObject* CSoundTriggerBox::Clone(void* pArg)
{
    CSoundTriggerBox* SoundTriggerBox = new CSoundTriggerBox(*this);
    if (FAILED(SoundTriggerBox->Initialize(pArg)))
    {
        Safe_Release(SoundTriggerBox);
        MSG_BOX("Clone Fail : Trigger Box");
    }
    return SoundTriggerBox;
}

void CSoundTriggerBox::Free()
{
    __super::Free();
}
