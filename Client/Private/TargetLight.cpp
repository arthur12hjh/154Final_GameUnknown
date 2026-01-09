#include "pch.h"
#include "TargetLight.h"

#include "GameInstance.h"
#include "Light.h"
#include "CinematicObject.h"

CTargetLight::CTargetLight()  
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

void CTargetLight::Set_Active(VISIBILITY eVisibility)
{
    if (VISIBILITY::HIDDEN == eVisibility)
        m_isActive = false;
    else
        m_isActive = true;

    m_pLightCom->SetVisibility(eVisibility);
}

void CTargetLight::Set_Diffuse(_vector vDiffuse)
{
    m_pLightCom->Set_Diffuse(vDiffuse);
}

void CTargetLight::Set_Range(_float fRange)
{
    m_pLightCom->Set_Range(fRange);
}

HRESULT CTargetLight::Initialize(void* pArg)
{
    TARGETLIGHT_DESC* pDesc = static_cast<TARGETLIGHT_DESC*>(pArg);

    if (FAILED(Setting_Desc(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CTargetLight::Chase_Target()
{
    if (false == m_isActive)
        return;

    switch (m_isCinematic)
    {
    case true :
        //(XMMatrixTranslation(0.f, 2.f, 2.f) * 
        m_pLightCom->Set_Position((static_cast<CCinematicObject*>(m_pTarget)->Get_CinematicWorldPos()).r[3]
            + XMVectorSet(0.f, 8.f, 0.f, 0.f));
        break;

    case false:
        m_pLightCom->Set_Position(m_pTarget->GetTransform()->Get_State(STATE::POSITION) + XMVector3Normalize(m_pTarget->GetTransform()->Get_State(STATE::LOOK) + XMVectorSet(0.f, 1.f, 0.f, 0.f)) * 3.4f
            + XMVectorSet(0.f, 7.f, 0.f, 0.f));
        break;

    default:
        return;
    }

}

HRESULT CTargetLight::Setting_Desc(TARGETLIGHT_DESC* pDesc)
{   
    m_pGameInstance->Add_Light(pDesc->tLightDesc, &m_pLightCom);
    m_pTarget = pDesc->pTarget;
    m_isCinematic = pDesc->isCinematic;

    return S_OK;
}

CTargetLight* CTargetLight::Create(void* pArg)
{
    CTargetLight* pInstance = new CTargetLight();

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Create Failed : TargetLight");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTargetLight::Free()
{
    __super::Free();

    Safe_Release(m_pLightCom);
    Safe_Release(m_pGameInstance);
}
