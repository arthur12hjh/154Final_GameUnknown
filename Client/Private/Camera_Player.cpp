#include "pch.h"
#include "Camera_Player.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"
#include "Nayitba.h"
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
    m_bIsPositionInitialized = FALSE;
    m_fDistance = 13.f;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_fDefaultOnFov = m_pCameraInfo.fFov;
    if (nullptr != pPlayer)
        m_pTransformCom->Set_State(STATE::POSITION, pPlayer->Get_Position());

    Safe_Release(pPlayer);

    return S_OK;
}

void CCamera_Player::Priority_Update(_float fTimeDelta)
{
    fTimeDelta = min(fTimeDelta, 0.016f);

    if (m_bIsPositionInitialized == FALSE)
    {
        Set_CameraDestination();

		m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(reinterpret_cast<const _float4*>(&m_BeforeMatrix.m[3])));

        __super::Bind_Matrices(fTimeDelta);
        m_bIsPositionInitialized = TRUE;
        return;
    }

    if (false == m_pGameInstance->IsMainCamera(this))
    {
        //Set_CameraDestination();
        return;
    }

    if(!m_bIsTransition)
    {
        _bool bLockOn = false;
        if (m_pGameManager->Get_LockonTarget())
        {
            auto eMonsterType = m_pGameManager->Get_LockonTarget()->GetStaticMonsterData()->eNaytiba_Type;
            if (NAYTIBA_TYPE::ELITE <= eMonsterType && m_pGameManager->Get_Lockon())
                bLockOn = true;
        }   

        if (bLockOn)
        {
            LockOn_CamAction(fTimeDelta);
        }
        else
        {
            if (m_pCameraInfo.fFov != m_fDefaultOnFov)
                m_pCameraInfo.fFov = m_fDefaultOnFov;

            Default_CamAction(fTimeDelta);
        }
       
        __super::Bind_Matrices(fTimeDelta);
    }
    else
    {
        //m_bIsTransition = FALSE;

        Set_CameraDestination();

        Transition_Camera(fTimeDelta);
        return;
    }
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

void CCamera_Player::Set_Pivot(_vector vPivot)
{
	XMStoreFloat3(&m_vPivot, vPivot);   
}

void CCamera_Player::Transition_Camera(_float fTimeDelta)
{
    m_fTransitionLerpTime.x += fTimeDelta;

    // Smooth Lerp
    _matrix StartMatrix = XMLoadFloat4x4(&m_PreLerpMatrix);
    _matrix EndMatrix = XMLoadFloat4x4(&m_BeforeMatrix);

    _float fRatio = m_fTransitionLerpTime.x / m_fTransitionLerpTime.y;
    _vector vPosition = XMVectorLerp(StartMatrix.r[3], EndMatrix.r[3], fRatio);
    _vector vLookPos = vPosition + XMVectorLerp(StartMatrix.r[2], EndMatrix.r[2], fRatio);

    m_pTransformCom->Set_State(STATE::POSITION, vPosition);
    m_pTransformCom->LookAt(vLookPos);

    __super::Bind_Matrices(fTimeDelta);

    if (m_fTransitionLerpTime.x >= m_fTransitionLerpTime.y)
    {
        m_bIsTransition = FALSE;
    }
}

void CCamera_Player::Set_CameraDestination()
{
    // 1. Player WorldMatrix를 받는다.
    // 2. _vector vPosition을 만들어서, 플레이어 WorldMatrix에서 PlayerLook방향으로 뺀만큼 뽑아온다.
    // 3. 
    _matrix PlayerWorldMatrix = XMLoadFloat4x4(m_pPlayerTransform->Get_WorldMatrixPtr());

    
    // 1) ���� ���� (Yaw / Pitch �и�)
    _vector vPlayerLook = XMVector3Normalize(PlayerWorldMatrix.r[2]);

    m_fYaw = atan2f(
        XMVectorGetX(vPlayerLook),
        XMVectorGetZ(vPlayerLook)
    );

    m_fPitch = 0.f; // 컷신 종료 기본값`

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
    XMStoreFloat3(&vPivotPos, XMLoadFloat3(&vPlayerPos) + XMLoadFloat3(&m_vPivot));
    vPivotPos.x += fPitchRatio * -1.6f;
    vPivotPos.y += fPitchRatio * 2.f;

    XMStoreFloat3(&vViewPoint, XMLoadFloat3(&vPivotPos) + vPlayerLook * 0.3f);

    if (fPitchRatio > 0.5)
        fCamDist -= (Clamp((fPitchRatio - 0.5f) / 0.5f, 0.f, 1.f) * (fCamDist * 0.5f));

    _float3 vCamPos = {};
    XMStoreFloat3(&vCamPos, XMLoadFloat3(&vViewPoint) - vPlayerLook * (fCamDist));

    _vector		vLook = XMVectorSetW(XMLoadFloat3(&vPivotPos), 1.f) - XMVectorSetW(XMLoadFloat3(&vCamPos), 1.f);

    if (XMVector3Equal(vLook, XMVectorZero()))
        return;

    _vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    _vector		vUp = XMVector3Cross(vLook, vRight);

    XMStoreFloat4x4(
        &m_BeforeMatrix,
        XMMatrixSet(
            XMVectorGetX(vRight), XMVectorGetY(vRight), XMVectorGetZ(vRight), 0.f,
            XMVectorGetX(vUp), XMVectorGetY(vUp), XMVectorGetZ(vUp), 0.f,
            XMVectorGetX(vLook), XMVectorGetY(vLook), XMVectorGetZ(vLook), 0.f,
            vCamPos.x, vCamPos.y, vCamPos.z, 1.f));
}

void CCamera_Player::Default_CamAction(_float fTimeDelta)
{
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
    XMStoreFloat3(&vPivotPos, XMLoadFloat3(&vPlayerPos) + XMLoadFloat3(&m_vPivot));
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
    m_pTransformCom->Chase_Lerp(XMLoadFloat3(&vCamPos), fTimeDelta, 0.f);
    m_pTransformCom->LookAt_Lerp(XMVectorSetW(XMLoadFloat3(&vPivotPos), 1.f), 0.5f, 1.f);

    //memcpy(&m_BeforeMatrix, m_pTransformCom->Get_WorldMatrixPtr(), sizeof(_float4x4));

}

void CCamera_Player::LockOn_CamAction(_float fTimeDelta)
{
    _vector vTargetPoint = m_pGameManager->Get_LockOnPoint();
    _vector vPlayerCamPos = m_pTransformCom->Get_State(STATE::POSITION);

    // Look 바라보는 방향
    _float fLength = XMVectorGetX(XMVector3Length(vTargetPoint - vPlayerCamPos));
    _vector vTargetLookDir = XMVector3Normalize(vTargetPoint - vPlayerCamPos);

    // 쿼터니언을 사용하기위해 Yaw 부분 계산

    // atan2는 수평면(x, z)에서 Y축이 이루는 각도를 반환한다.
    // 때문에 이를 이용해서 Yaw를 구할수 있다.
    _float fTargetYaw = atan2f(XMVectorGetX(vTargetLookDir), XMVectorGetZ(vTargetLookDir));

    // Pitch는 방향 벡터가 XZ 평면(수평면)과 이루는 기울기 각도이다.
    // 방향 벡터는 정규화되어 있으며(|v| = 1), 단위 구 위의 한 점으로 볼 수 있다.
    //
    // 수직 단면에서 보면 삼각형이 형성되며:
    //  - 빗변: 1 (단위 벡터 길이)
    //  - 세로 성분: y
    //  - 바닥 성분: sqrt(x*x + z*z)
    //
    // 삼각함수 정의에 의해:
    //  sin(Pitch) = y / 1
    //  => y = sin(Pitch)
    //
    // 따라서 Pitch는 Yaw와 독립적으로 다음과 같이 구할 수 있다:
    //  Pitch = asin(y)
    //  또는
    //  Pitch = atan2(y, sqrt(x*x + z*z))
    //
    // 이 방식은 Yaw 값에 영향을 받지 않으며,
    // z 성분이 0인 경우에도 안정적으로 동작한다.
    _float fTargetPitch = -asinf(XMVectorGetY(vTargetLookDir));
    fTargetPitch = Clamp<_float>(fTargetPitch, -0.3f, XM_PIDIV2);

    _vector vScale, vRotation, vTranslation;
    XMMatrixDecompose(&vScale, &vRotation, &vTranslation, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    _vector vTargetRot = XMQuaternionRotationRollPitchYaw(fTargetPitch, fTargetYaw, 0.f);
    _float fQuaternionScalar = XMVectorGetY(XMVector3Dot(vRotation, vTargetRot));

    _float fYawRatio = {};
    if (-0.5f > abs(fQuaternionScalar))
        fYawRatio = 40.f;
    else
        fYawRatio = 15.f;

    fYawRatio = Clamp<_float>(fTimeDelta * fYawRatio, 0.f, 1.f);
    _vector vLerpRoation = XMQuaternionSlerp(vRotation, vTargetRot, fYawRatio);

    // 이때만 위치 보정
    _vector vDir = m_pTransformCom->Get_State(STATE::LOOK) * -(m_fDistance + ( 5.f * fTargetPitch));
    vDir += m_pPlayerTransform->Get_State(STATE::UP) * (m_vPivot.y + 1.5f);
    vDir += m_pPlayerTransform->Get_State(STATE::RIGHT) * (m_vPivot.x + 1.f);

    if (0 > fTargetPitch)
    {
        m_pCameraInfo.fFov = XMConvertToRadians(Lerp<_float>(m_fLockOnFov.x, m_fLockOnFov.y, 1 + fTargetPitch));
    }

    m_pTransformCom->Chase_Lerp(m_pPlayerTransform->Get_State(STATE::POSITION) + vDir, fTimeDelta);
    m_pTransformCom->Set_Rotation(vLerpRoation, true);
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