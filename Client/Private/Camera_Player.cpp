#include "pch.h"
#include "Camera_Player.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"

CCamera_Player::CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
    , m_pGameManager { CGameManager::GetInstance() }
{
}

CCamera_Player::CCamera_Player(const CCamera_Player& Prototype)
    : CCamera{ Prototype }
    , m_pGameManager { CGameManager::GetInstance() }
{
}

HRESULT CCamera_Player::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Player::Initialize(void* pArg)
{
    //CAMERA_PLAYER_DESC* pDesc = static_cast<CAMERA_PLAYER_DESC*>(pArg);
    //m_fMouseSensor = pDesc->fMouseSensor;

    m_fMouseSensor = 0.2f;
    m_pPlayerTransform = m_pGameManager->Get_PlayerDesc()->pPlayerTransform;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CCamera_Player::Priority_Update(_float fTimeDelta)
{
    if (false == m_pGameInstance->IsMainCamera(this))
        return;

    m_pTransformCom->LookAt_Lerp(
        m_pPlayerTransform->Get_State(STATE::POSITION) +
        XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::LOOK)) * 1.f +
        XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::UP)) * 1.f, 0.175f);

    _float fMouseMoveX = (_float)m_pGameInstance->GetMouseAxis(0) / g_iWinSizeX;
    _float fMouseMoveY = (_float)m_pGameInstance->GetMouseAxis(1) / g_iWinSizeY;

    // 회전할 벡터와 각도
    _vector  StartVector = XMVectorSet(0.f, 5.f, -6.0f, 0.f);

    m_fRotateX += XMConvertToRadians(fMouseMoveX * 90.f);
    m_fRotateX = XMScalarModAngle(m_fRotateX);

    m_fRotateY += XMConvertToRadians(fMouseMoveY * 45.f);
    m_fRotateY = XMScalarModAngle(m_fRotateY);

    // 라디안 제한 ( -90  ~ +90 + 여기에 캐릭터의 Look 벡터까지. )
    _float fLimit = XMConvertToRadians(15.f);

    if (m_fRotateY > fLimit)
        m_fRotateY = fLimit;

    if (m_fRotateY < -fLimit)
        m_fRotateY = -fLimit;

    _vector		vQuternion = XMQuaternionRotationRollPitchYaw(m_fRotateY, m_fRotateX, 0.f);

    _matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuternion);
    _vector     vCamPos = XMVector3Rotate(StartVector, vQuternion);
    _float      fLength = XMVectorGetX(XMVector3Length(vCamPos));

    m_pTransformCom->Chase_Lerp(m_pPlayerTransform->Get_State(STATE::POSITION) + vCamPos, fTimeDelta * 0.9f, 0.f);


    __super::Bind_Matrices();
}

void CCamera_Player::Update(_float fTimeDelta)
{
}

void CCamera_Player::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Player::Render()
{
    return S_OK;
}


CCamera_Player* CCamera_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Player* pInstance = new CCamera_Player(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCamera_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Player::Clone(void* pArg)
{
    CCamera_Player* pInstance = new CCamera_Player(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CCamera_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Player::Free()
{
    __super::Free();
}
