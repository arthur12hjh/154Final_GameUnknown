#include "pch.h"
#include "Camera_Action.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"
#include "CameraBone_Player.h"

CCamera_Action::CCamera_Action(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
    , m_pGameManager{ CGameManager::GetInstance() }
{
}

CCamera_Action::CCamera_Action(const CCamera_Action& Prototype)
    : CCamera{ Prototype }
    , m_pGameManager{ CGameManager::GetInstance() }
{
}

HRESULT CCamera_Action::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Action::Initialize(void* pArg)
{
    m_pPlayerTransform = m_pGameManager->Get_PlayerDesc()->pPlayerTransform;

    CPlayer* pPlayer = m_pGameManager->GetGameCharacter();

    if (nullptr != pPlayer)
    {
        m_pCameraBone = dynamic_cast<CCameraBone_Player*>(pPlayer->Get_PartObject(TEXT("Part_CameraBone")));

        m_pBoneModelCom = dynamic_cast<CModel*>(m_pCameraBone->Find_Component(TEXT("Com_Model")));
        m_pBoneCombinedMatrix = m_pCameraBone->Get_CombinedMatrixPtr();
        m_pSocketMatrix = m_pBoneModelCom->Get_BoneMatrixPtr("Camera_Bone");
    }

    Safe_Release(pPlayer);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Initialize_CameraAnimationData();

    return S_OK;
}

void CCamera_Action::Priority_Update(_float fTimeDelta)
{
    m_fCurrentAnimationTime += fTimeDelta;

    _vector vFOVPosition;
    _vector vPivotPosition = XMVectorSet(0.f, 0.f, 0.f, 0.f);
    
    // 우리가 채널형식으로 정의할건 m_vFOVTracks와 m_vPivotTracks이다
    // 먼저 FOV부터
    if (!m_vFOVTracks.empty())
    {
        while (m_iFOVTrackIndex + 1 < m_vFOVTracks.size() &&
            m_fCurrentAnimationTime >= m_vFOVTracks[m_iFOVTrackIndex].fTrackPosition)
        {
            ++m_iFOVTrackIndex;
        }

        // 중괄호 치는 이유? 아래 Pivot도 있는데 변수명 겹쳐쓸거라 사고날 수도 있음 ;; 예방 차원 ㅇㅇ ;;
        {
            _float fRatio = Clamp(Get_FOVTrackRatio(), 0.f, 1.f);
            _float3 vBeforeFOV = Get_BeforeFOV();


            // m_vFOVTracks[m_iFOVTrackIndex].vTrackValue 바탕으로 보간
            // 에르미트 보간 공식에 따라 기울기값으로 계산한다.
            _vector vHermiteStart = XMVectorSet(m_vFOVTracks[m_iFOVTrackIndex].fTangentStart, m_vFOVTracks[m_iFOVTrackIndex].fTangentStart, m_vFOVTracks[m_iFOVTrackIndex].fTangentStart, 0.f);
            _vector vHermiteEnd = XMVectorSet(m_vFOVTracks[m_iFOVTrackIndex].fTangentEnd, m_vFOVTracks[m_iFOVTrackIndex].fTangentEnd, m_vFOVTracks[m_iFOVTrackIndex].fTangentEnd, 0.f);
            _vector vCurrentHermite = XMVectorHermite(XMLoadFloat3(&vBeforeFOV), vHermiteStart, XMLoadFloat3(&m_vFOVTracks[m_iFOVTrackIndex].vTrackValue), vHermiteEnd, fRatio);

            vFOVPosition = XMVectorSetW(vCurrentHermite, 0.f);

        }

        CAMERA_INFO CameraInfo;

        CameraInfo.fFov = XMConvertToRadians(vFOVPosition.m128_f32[0]);

        SetCameraInfo(CameraInfo, 0001);

    }

    // 다음은 Pivot
    if (!m_vPivotTracks.empty())
    {
        while (m_iPivotTrackIndex + 1 < m_vPivotTracks.size() &&
            m_fCurrentAnimationTime >= m_vPivotTracks[m_iPivotTrackIndex].fTrackPosition)
        {
            ++m_iPivotTrackIndex;
        }

        {
            _float fRatio = Clamp(Get_PivotTrackRatio(), 0.f, 1.f);
            _float3 vBeforePivot = Get_BeforePivot();

            // m_vFOVTracks[m_iFOVTrackIndex].vTrackValue 바탕으로 보간
            // 에르미트 보간 공식에 따라 기울기값으로 계산한다.
            _vector vHermiteStart = XMVectorSet(m_vPivotTracks[m_iPivotTrackIndex].fTangentStart, m_vPivotTracks[m_iPivotTrackIndex].fTangentStart, m_vPivotTracks[m_iPivotTrackIndex].fTangentStart, 0.f);
            _vector vHermiteEnd = XMVectorSet(m_vPivotTracks[m_iPivotTrackIndex].fTangentEnd, m_vPivotTracks[m_iPivotTrackIndex].fTangentEnd, m_vPivotTracks[m_iPivotTrackIndex].fTangentEnd, 0.f);
            _vector vCurrentHermite = XMVectorHermite(XMLoadFloat3(&vBeforePivot), vHermiteStart, XMLoadFloat3(&m_vPivotTracks[m_iPivotTrackIndex].vTrackValue), vHermiteEnd, fRatio);

            vPivotPosition = XMVectorSetW(vCurrentHermite, 0.f);
        }
    }

   

    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
    _matrix TransformMatrix;
    for (size_t i = 0; i < 3; i++)
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

    TransformMatrix =
        SocketMatrix * XMLoadFloat4x4(m_pBoneCombinedMatrix);

    m_pTransformCom->Set_State(STATE::RIGHT, TransformMatrix.r[0]);
    m_pTransformCom->Set_State(STATE::UP, TransformMatrix.r[1]);
    m_pTransformCom->Set_State(STATE::LOOK, TransformMatrix.r[2]);
    m_pTransformCom->Set_State(STATE::POSITION, TransformMatrix.r[3]);

    m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION) + vPivotPosition);

    if (false == m_pGameInstance->IsMainCamera(this))
        return;

    __super::Bind_Matrices(fTimeDelta);
}

void CCamera_Action::Update(_float fTimeDelta)
{
}

void CCamera_Action::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Action::Render()
{
    return S_OK;
}

void CCamera_Action::Initialize_CameraAnimationData(_uint iCameraAnimationIndex)
{
    m_fCurrentAnimationTime = 0.f;

    m_iFOVTrackIndex = 0;
    m_iPivotTrackIndex = 0;
    
    m_pCameraAnimationData = nullptr;

    if (-1 == iCameraAnimationIndex)
    {
        return;
    }

    m_pCameraAnimationData = CGameManager::GetInstance()->Find_CameraAnimationData(iCameraAnimationIndex);

    m_vFOVTracks.clear();
    m_vPivotTracks.clear();

    CAMERA_INFO CameraInfo;

    CameraInfo.fFov = XMConvertToRadians(m_pCameraAnimationData->fBaseFOV);

    SetCameraInfo(CameraInfo, 0001);


    m_vCameraPivot = m_pCameraAnimationData->vBaseCameraPivot;
    for (const auto& pFOVTrack : m_pCameraAnimationData->FOVTrackList)
    {
        m_vFOVTracks.push_back(pFOVTrack);
    }

    for (const auto& pPivotTrack : m_pCameraAnimationData->PivotTrackList)
    {
        m_vPivotTracks.push_back(pPivotTrack);
    }

    if (nullptr != m_pCameraBone)
    {
        m_pCameraBone->Initilaize_CameraAnimationData(*m_pCameraAnimationData);
    }
}


_float CCamera_Action::Get_FOVTrackRatio()
{
    if (0 == m_iFOVTrackIndex)
        return m_fCurrentAnimationTime / m_vFOVTracks[m_iFOVTrackIndex].fTrackPosition;

    return (m_fCurrentAnimationTime - m_vFOVTracks[m_iFOVTrackIndex - 1].fTrackPosition)
        / (m_vFOVTracks[m_iFOVTrackIndex].fTrackPosition - m_vFOVTracks[m_iFOVTrackIndex - 1].fTrackPosition);
}

_float CCamera_Action::Get_PivotTrackRatio()
{
    if (0 == m_iPivotTrackIndex)
        return m_fCurrentAnimationTime / m_vPivotTracks[m_iPivotTrackIndex].fTrackPosition;

    return (m_fCurrentAnimationTime - m_vPivotTracks[m_iPivotTrackIndex - 1].fTrackPosition)
        / (m_vPivotTracks[m_iPivotTrackIndex].fTrackPosition - m_vPivotTracks[m_iPivotTrackIndex - 1].fTrackPosition);
}

_float3 CCamera_Action::Get_BeforeFOV()
{
    if (0 == m_iFOVTrackIndex)
    {
        if (nullptr == m_pCameraAnimationData)
            return _float3(60.f, 60.f, 60.f);
        else
            return _float3(m_pCameraAnimationData->fBaseFOV, m_pCameraAnimationData->fBaseFOV, m_pCameraAnimationData->fBaseFOV);
    }

    return m_vFOVTracks[m_iFOVTrackIndex - 1].vTrackValue;
}

_float3 CCamera_Action::Get_BeforePivot()
{
    if (0 == m_iPivotTrackIndex)
    {
        if (nullptr == m_pCameraAnimationData)
            return _float3(0.f, 0.f, 0.f);
        else
            return m_pCameraAnimationData->vBaseCameraPivot;
    }

    return m_vPivotTracks[m_iPivotTrackIndex - 1].vTrackValue;
}

CCamera_Action* CCamera_Action::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Action* pInstance = new CCamera_Action(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCamera_Action");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Action::Clone(void* pArg)
{
    CCamera_Action* pInstance = new CCamera_Action(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CCamera_Action");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Action::Free()
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