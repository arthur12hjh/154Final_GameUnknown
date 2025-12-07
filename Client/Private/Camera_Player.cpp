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

    if(nullptr != pPlayer)
        m_pCameraBone = dynamic_cast<CCameraBone_Player*>(pPlayer->Get_PartObject(TEXT("Part_CameraBone")));

    if (nullptr != m_pCameraBone)
    {
        Safe_Release(pPlayer);
    }

    m_fRotateX = 0.f;
    m_fRotateY = 90.f;

    m_eCameraState = CAMERA_STATE::TRANSFORM;
    m_fDistance = 10.f;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CCamera_Player::Priority_Update(_float fTimeDelta)
{

    //if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_P))
    //    m_fRotateX += fTimeDelta * 30.f;
    //if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_O))
    //    m_fRotateX -= fTimeDelta * 30.f;
    //
    //if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_L))
    //    m_fRotateY += fTimeDelta * 30.f;
    //if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_K))
    //    m_fRotateY -= fTimeDelta * 30.f;

    //if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_P))
    //    m_fLookMagnification += 1.f;
    //if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_O))
    //    m_fLookMagnification -= 1.f;
    //
    //if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_L))
    //    m_fUpMagnification += 1.f;
    //if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_K))
    //    m_fUpMagnification -= 1.f;


    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_N))
        m_eCameraState = CAMERA_STATE::FOLLOW;

    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_M))
        m_eCameraState = CAMERA_STATE::TRANSFORM;

    /*
    1. 플레이어 어께 위에 빨간색 Pivot Position을 정해두고,
    2. 카메라의 위치와 PivotPosition 사이의 방향 벡터를 구해 파란색 점(View Point)를 만들거야. 
    3. 카메라는 View Point로부터 일정 거리만큼 거리를 두고 바라볼거야. 
    4. View Point는 Pitch가 XM_PIDIV2에 가까워질 수록 멀어질거라, 카메라 역시 이에 맞추어 플레이어가 가까워질거야
    */
    if (CAMERA_STATE::TRANSFORM == m_eCameraState)
    {
        _float fMouseMoveX = (_float)m_pGameInstance->GetMouseAxis(0) / g_iWinSizeX;
        _float fMouseMoveY = (_float)m_pGameInstance->GetMouseAxis(1) / g_iWinSizeY;

        // 1) 각도 갱신 (Yaw / Pitch 분리)
        m_fYaw += XMConvertToRadians(fMouseMoveX * 90.f);
        m_fYaw = XMScalarModAngle(m_fYaw);

        m_fPitch += XMConvertToRadians(fMouseMoveY * 45.f);

        // 상단 최대각도  
        _float fPitchLimit = XM_PIDIV2 - 0.2f;
        if (m_fPitch > fPitchLimit) m_fPitch = fPitchLimit;
        if (m_fPitch < -fPitchLimit) m_fPitch = -fPitchLimit;

        // 플레이어 위치 받아오기.
        _float3 vPlayerPos = {};
        XMStoreFloat3(&vPlayerPos, m_pPlayerTransform->Get_State(STATE::POSITION));
        
        // 피벗 위치를 받아온다.
        _float3 vPivotPos = {};
        XMStoreFloat3(&vPivotPos, XMLoadFloat3(&vPlayerPos) + XMVectorSet(1.f, 5.f, 0.f, 0.f));

        // ViewPoint를 구하기 위한 방향벡터
        _vector vDirection = XMMatrixRotationRollPitchYaw(m_fPitch, m_fYaw, 0.f).r[2];

        _float3 vViewPoint = {};
        XMStoreFloat3(&vViewPoint, XMLoadFloat3(&vPivotPos) + vDirection * 0.3f);

        float fPitchRatio = fabs(m_fPitch) / fPitchLimit;
        vViewPoint.y += Lerp(0.f, 5.f, fPitchRatio);

        // 카메라와 ViewPoint 거리. 얼마가 나으려나~?
        _float fCamDist = m_fDistance;

        _float3 vCamPos = {};
        XMStoreFloat3(&vCamPos, XMLoadFloat3(&vViewPoint) - vDirection * fCamDist);

        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vCamPos), 1.f));
        m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&vPivotPos), 1.f));

    }
    else if (CAMERA_STATE::FOLLOW == m_eCameraState)
    {
        // 반드시 카메라 최종 수정 때 수정할 것!! 현재는 테스트 + 위치잡기 중
        CModel* pCameraBoneModel = dynamic_cast<CModel*>(m_pCameraBone->Find_Component(TEXT("Com_Model")));
        const _float4x4* pCameraBoneTransform = m_pCameraBone->Get_CombinedMatrixPtr( );
        
        _matrix SocketMatrix = XMLoadFloat4x4(pCameraBoneModel->Get_BoneMatrixPtr("Camera_Bone"));
        _matrix TransformMatrix;
        for (size_t i = 0; i < 3; i++)
            SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

        TransformMatrix = 
            SocketMatrix * XMLoadFloat4x4(pCameraBoneTransform);

        m_pTransformCom->Set_State(STATE::RIGHT, TransformMatrix.r[0]);
        m_pTransformCom->Set_State(STATE::UP, TransformMatrix.r[1]);
        m_pTransformCom->Set_State(STATE::LOOK, TransformMatrix.r[2]);
        m_pTransformCom->Set_State(STATE::POSITION, TransformMatrix.r[3]);
    
        m_pTransformCom->LookAt(
            m_pPlayerTransform->Get_State(STATE::POSITION)  +
            XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::LOOK)) * m_fLookMagnification +
            XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::UP)) * m_fUpMagnification);
    }

    if (false == m_pGameInstance->IsMainCamera(this))
        return;

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


/*
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
*/