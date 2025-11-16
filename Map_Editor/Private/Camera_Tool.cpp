#include "pch.h"
#include "Camera_Tool.h"

#include "GameInstance.h"

const WCHAR* m_szCinemaComponentName = TEXT("Prototype_Component_CinemaComponent");

CCamera_Tool::CCamera_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    m_pDevice(pDevice),
    m_pContext(pContext),
    m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(pDevice);
    Safe_AddRef(pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CCamera_Tool::Initialize()
{
    return S_OK;
}

void CCamera_Tool::Priority_Update(_float fTimeDelta)
{
}

void CCamera_Tool::Update(_float fTimeDelta)
{
}

void CCamera_Tool::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Tool::Render()
{
    ImGui::Begin("Camera Editor");
    
    ImGui::Separator();
    ImGui::DragFloat3("Roation", m_vRotation);
    ImGui::DragFloat3("Translate", m_vTranslate);

    ImGui::Text("CameraInfo");
    ImGui::Separator();
    ImGui::InputFloat("Fov", &m_fFov);
    ImGui::InputFloat("Far", &m_fFar);
    ImGui::InputFloat("Near", &m_fNear);

    //여기서 내가 선택한 컴포넌트로 만들자.
    ImGui::End();
    return S_OK;
}

HRESULT CCamera_Tool::Save_Camera_Action()
{
    return S_OK;
}

HRESULT CCamera_Tool::Load_Camera_Actions()
{
    return S_OK;
}

CCamera_Tool* CCamera_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Tool* pCamera_Tool = new CCamera_Tool(pDevice, pContext);
    if (FAILED(pCamera_Tool->Initialize()))
    {
        Safe_Release(pCamera_Tool);
        MSG_BOX("Create Fail : Camera Tool");
    }
    return pCamera_Tool;
}

void CCamera_Tool::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
