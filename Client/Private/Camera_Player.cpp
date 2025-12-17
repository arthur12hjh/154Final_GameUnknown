#include "pch.h"
#include "Camera_Player.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"
#include "CameraBone_Player.h"

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

    CPlayer* pPlayer = m_pGameManager->GetGameCharacter();
    
    m_fRotateX = 0.f;
    m_fRotateY = 90.f;

    m_fDistance = 8.f;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (nullptr != pPlayer)
        m_pTransformCom->Set_State(STATE::POSITION, pPlayer->Get_Position());

    Safe_Release(pPlayer);

    return S_OK;
}

void CCamera_Player::Priority_Update(_float fTimeDelta)
{

    /*
    1. �÷��̾� � ���� ������ Pivot Position�� ���صΰ�,
    2. ī�޶��� ��ġ�� PivotPosition ������ ���� ���͸� ���� �Ķ��� ��(View Point)�� ����ž�.
    3. ī�޶�� View Point�κ��� ���� �Ÿ���ŭ �Ÿ��� �ΰ� �ٶ󺼰ž�.
    4. View Point�� Pitch�� XM_PIDIV2�� ������� ���� �־����Ŷ�, ī�޶� ���� �̿� ���߾� �÷��̾ ��������ž�
    */

    _float fMouseMoveX = (_float)m_pGameInstance->GetMouseAxis(0) / g_iWinSizeX;
    _float fMouseMoveY = (_float)m_pGameInstance->GetMouseAxis(1) / g_iWinSizeY;

    // 1) ���� ���� (Yaw / Pitch �и�)
    m_fYaw += XMConvertToRadians(fMouseMoveX * 90.f);
    m_fYaw = XMScalarModAngle(m_fYaw);

    m_fPitch += XMConvertToRadians(fMouseMoveY * 45.f);

    // ��� �ִ밢��  
    _float fPitchLimit = XM_PIDIV2 - 0.2f;
    if (m_fPitch > fPitchLimit) m_fPitch = fPitchLimit;
    if (m_fPitch < -fPitchLimit) m_fPitch = -fPitchLimit;

    _float3 vViewPoint = {};

    float fPitchRatio = fabs(m_fPitch) / fPitchLimit;
	fPitchRatio = Clamp(fPitchRatio, 0.f, 1.f);
    vViewPoint.y += Lerp(0.f, 5.f, fPitchRatio);

    // ī�޶�� ViewPoint �Ÿ�.
    _float fCamDist = m_fDistance;
    
    // �÷��̾� ��ġ �޾ƿ���.
    _float3 vPlayerPos = {};
    if (nullptr == m_pPlayerTransform)
        return;

    XMStoreFloat3(&vPlayerPos, m_pPlayerTransform->Get_State(STATE::POSITION));
    // �ǹ� ��ġ�� �޾ƿ´�.
    _float3 vPivotPos = {};
    //XMStoreFloat3(&vPivotPos, XMLoadFloat3(&vPlayerPos) + XMVector3TransformNormal(XMLoadFloat3(&m_vPivot), XMLoadFloat4x4(m_pPlayerTransform->Get_WorldMatrixPtr())));
    XMStoreFloat3(&vPivotPos, XMLoadFloat3(&vPlayerPos) +XMLoadFloat3(&m_vPivot));
    vPivotPos.x += fPitchRatio * -1.6f;
    vPivotPos.y += fPitchRatio * 2.f;

    // ViewPoint�� ���ϱ� ���� ���⺤��
    _vector vDirection = XMMatrixRotationRollPitchYaw(m_fPitch, m_fYaw, 0.f).r[2];

    XMStoreFloat3(&vViewPoint, XMLoadFloat3(&vPivotPos) + vDirection * 0.3f);

    if (fPitchRatio > 0.5)
        fCamDist -= (Clamp((fPitchRatio - 0.5f) / 0.5f, 0.f, 1.f) * (fCamDist * 0.5f));

    _float3 vCamPos = {};
    XMStoreFloat3(&vCamPos, XMLoadFloat3(&vViewPoint) - vDirection * (fCamDist));

    //m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vCamPos), 1.f));
    m_pTransformCom->Chase_Lerp(XMLoadFloat3(&vCamPos), fTimeDelta, 1.f);
    m_pTransformCom->LookAt_Lerp(XMVectorSetW(XMLoadFloat3(&vPivotPos), 1.f), 0.5f, 1.f);

    memcpy(&m_BeforeMatrix, m_pTransformCom->Get_WorldMatrixPtr(), sizeof(_float4x4));

    if (false == m_pGameInstance->IsMainCamera(this))
        return;

    if (m_bIsTransition)
    {
        m_fTransitionLerpTime.x += fTimeDelta;

        // Smooth Lerp
        _matrix StartMat = XMLoadFloat4x4(&m_PreLerpMatrix);
        _matrix EndMat = XMLoadFloat4x4(&m_BeforeMatrix);

        _float fRatio = m_fTransitionLerpTime.x / m_fTransitionLerpTime.y;
        _vector vPosition = XMVectorLerp(StartMat.r[3], EndMat.r[3], fRatio);
        _vector vLookPos = vPosition + XMVectorLerp(StartMat.r[2], EndMat.r[2], fRatio);

        m_pTransformCom->Set_State(STATE::POSITION, vPosition);
        m_pTransformCom->LookAt(vLookPos);

        __super::Bind_Matrices(fTimeDelta);

        if (m_fTransitionLerpTime.x >= m_fTransitionLerpTime.y)
        {
            m_bIsTransition = FALSE;
        }

        return;
    }

    __super::Bind_Matrices(fTimeDelta);
}

void CCamera_Player::Update(_float fTimeDelta)
{
    //ī�޶� ����ŷ�� ���ؼ� �θ� Update ȣ��. 
    //__super::Update(fTimeDelta);
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


/*
    m_pTransformCom->LookAt_Lerp(
        m_pPlayerTransform->Get_State(STATE::POSITION) +
        XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::LOOK)) * 1.f +
        XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::UP)) * 1.f, 0.175f);

    _float fMouseMoveX = (_float)m_pGameInstance->GetMouseAxis(0) / g_iWinSizeX;
    _float fMouseMoveY = (_float)m_pGameInstance->GetMouseAxis(1) / g_iWinSizeY;

    // ȸ���� ���Ϳ� ����
    _vector  StartVector = XMVectorSet(0.f, 5.f, -6.0f, 0.f);

    m_fRotateX += XMConvertToRadians(fMouseMoveX * 90.f);
    m_fRotateX = XMScalarModAngle(m_fRotateX);

    m_fRotateY += XMConvertToRadians(fMouseMoveY * 45.f);
    m_fRotateY = XMScalarModAngle(m_fRotateY);

    // ���� ���� ( -90  ~ +90 + ���⿡ ĳ������ Look ���ͱ���. )
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
*/