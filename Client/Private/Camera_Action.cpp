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
    if (false == m_pGameInstance->IsMainCamera(this))
        return;

    Update_Camera();

    __super::Bind_Matrices(fTimeDelta);

    m_fCurrentAnimationTime += fTimeDelta;

    if (m_fCurrentAnimationTime >= m_fAnimationPlayTime)
    {
        CCamera* pCamera = m_pGameInstance->GetCamrea(TEXT("PlayerCamera"));
        pCamera->Set_MainCamera(TEXT("PlayerCamera"));
        Safe_Release(pCamera);
    }
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
    m_fAnimationPlayTime = -1.f;

    m_pCameraAnimationData = nullptr;

    if (-1 == iCameraAnimationIndex)
    {
        return;
    }

    m_pCameraAnimationData = CGameManager::GetInstance()->Find_CameraAnimationData(iCameraAnimationIndex);

    if (nullptr == m_pCameraAnimationData)
    {
        return;
    }
    m_vFOVTracks.clear();
    m_vPivotTracks.clear();
	m_vPositionTracks.clear();
    m_vRotationTracks.clear();

    CAMERA_INFO CameraInfo;

    CameraInfo.fFov = XMConvertToRadians(m_pCameraAnimationData->fBaseFOV);

    SetCameraInfo(CameraInfo, 0001);

    m_vCameraPivot = m_pCameraAnimationData->vBaseCameraPivot;
    for (const auto& pFOVTrack : m_pCameraAnimationData->FOVTrackList)
    {
		m_fAnimationPlayTime = max(m_fAnimationPlayTime, pFOVTrack.fTrackPosition);
        m_vFOVTracks.push_back(pFOVTrack);
    }

    for (const auto& pPivotTrack : m_pCameraAnimationData->PivotTrackList)
    {
		m_fAnimationPlayTime = max(m_fAnimationPlayTime, pPivotTrack.fTrackPosition);
        m_vPivotTracks.push_back(pPivotTrack);
    }

    for (const auto& pPositionTrack : m_pCameraAnimationData->BonePositionTrackList)
    {
		m_fAnimationPlayTime = max(m_fAnimationPlayTime, pPositionTrack.fTrackPosition);
        m_vPositionTracks.push_back(pPositionTrack);
    }

    for (const auto& pRotationTrack : m_pCameraAnimationData->BoneRotationTrackList)
    {
		m_fAnimationPlayTime = max(m_fAnimationPlayTime, pRotationTrack.fTrackPosition);
        m_vRotationTracks.push_back(pRotationTrack);
    }

    if (m_pCameraAnimationData->iCameraAnimationFlag & iFLAG_CAMERA_LOOKAT_PIVOT)
    {
        m_CameraSource.eType = CAMERA_SOURCE_TYPE::LOOK_PIVOT;
    }
    else if (m_pCameraAnimationData->iCameraAnimationFlag & iFLAG_CAMERA_SOURCE_WORLD)
    {
        m_CameraSource.eType = CAMERA_SOURCE_TYPE::WORLD;
    }
    else
    {
        m_pCameraBone->Initilaize_CameraAnimationData(*m_pCameraAnimationData);

        m_CameraSource.eType = CAMERA_SOURCE_TYPE::BONE;
    }

    Update_Camera();

}

void CCamera_Action::Update_Camera()
{
    _vector vFOVPosition;

    // 우리가 채널형식으로 정의할건 m_vFOVTracks와 m_vPivotTracks이다
    // 근데 말이죵, CAMERA_SOURCE_TYPE이 WORLD면 BonePosition이랑 BoneRotation을 들고와서 내꺼로 써먹는당
    // 먼저 FOV부터
    if (!m_vFOVTracks.empty())
    {
        _float3 vFOV = Update_CameraTrack(
            m_vFOVTracks,
            m_fCurrentAnimationTime,
            _float3(m_pCameraAnimationData->fBaseFOV,
                m_pCameraAnimationData->fBaseFOV,
                m_pCameraAnimationData->fBaseFOV)
        );

        CAMERA_INFO CameraInfo{};
        CameraInfo.fFov = XMConvertToRadians(vFOV.x);
        SetCameraInfo(CameraInfo, 0001);
    }

    // 피벗이사가지없는련
    _float3 vPivot = _float3(0.f, 0.f, 0.f);
    if (m_pCameraAnimationData != nullptr)
        vPivot = m_pCameraAnimationData->vBaseCameraPivot;

    if (!m_vPivotTracks.empty())
    {
        vPivot = Update_CameraTrack(
            m_vPivotTracks,
            m_fCurrentAnimationTime,
            m_pCameraAnimationData
            ? m_pCameraAnimationData->vBaseCameraPivot
            : _float3(0.f, 0.f, 0.f)
        );
    }

    _matrix TransformMatrix = Calculate_CombinedMatrix();

    m_pTransformCom->Set_State(STATE::POSITION, TransformMatrix.r[3]);

    if (m_CameraSource.eType == CAMERA_SOURCE_TYPE::BONE)
        m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION) + XMLoadFloat3(&vPivot));
    else if (m_CameraSource.eType == CAMERA_SOURCE_TYPE::LOOK_PIVOT)
    {
        m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&vPivot), 1.f));
    }
    else
    {
        m_pTransformCom->Set_State(STATE::RIGHT, TransformMatrix.r[0]);
        m_pTransformCom->Set_State(STATE::UP, TransformMatrix.r[1]);
        m_pTransformCom->Set_State(STATE::LOOK, TransformMatrix.r[2]);
    }
}

_float3 CCamera_Action::Update_CameraTrack(const vector<CAMERA_TRACK_DESC>& vTrackList, float fCurrentTime, const _float3& vBaseValue)
{
    if (vTrackList.empty())
        return vBaseValue;
    
    if (fCurrentTime <= vTrackList.front().fTrackPosition)
        return vTrackList.front().vTrackValue;

    for (size_t i = 0; i + 1 < vTrackList.size(); ++i)
    {
        const CAMERA_TRACK_DESC& CurrentTrack = vTrackList[i];
        const CAMERA_TRACK_DESC& NextTrack = vTrackList[i + 1];

        if (fCurrentTime >= CurrentTrack.fTrackPosition &&
            fCurrentTime <= NextTrack.fTrackPosition)
        {
            float fDenom = (NextTrack.fTrackPosition - CurrentTrack.fTrackPosition);
            if (fDenom <= 0.f)
                return CurrentTrack.vTrackValue;

            float fRatio = (fCurrentTime - CurrentTrack.fTrackPosition) / fDenom;
            fRatio = Clamp(fRatio, 0.f, 1.f);

            // LERP
            if (CurrentTrack.iInterpolationFlag & iFLAG_INTERPOLATION_LERP)
            {
                _float3 vTrackResult;
                XMStoreFloat3(
                    &vTrackResult, XMVectorLerp(XMLoadFloat3(&CurrentTrack.vTrackValue), XMLoadFloat3(&NextTrack.vTrackValue), fRatio));

                return vTrackResult;
            }

            // HERMITE
            _vector vHermiteStart = XMVectorSet(CurrentTrack.fTangentStart, CurrentTrack.fTangentStart, CurrentTrack.fTangentStart, 0.f);
            _vector vHermiteEnd = XMVectorSet(CurrentTrack.fTangentEnd, CurrentTrack.fTangentEnd, CurrentTrack.fTangentEnd, 0.f);
            _vector vHermite = XMVectorHermite(XMLoadFloat3(&CurrentTrack.vTrackValue), vHermiteStart, XMLoadFloat3(&NextTrack.vTrackValue), vHermiteEnd, fRatio);

            _float3 vTrackResult;
            XMStoreFloat3(&vTrackResult, vHermite);
            return vTrackResult;
        }
    }


    return vTrackList.back().vTrackValue;
}

_matrix CCamera_Action::Calculate_CombinedMatrix()
{
    _float3 vPosition = _float3(0.f, 0.f, 0.f);
    _float3 vRotation = _float3(0.f, 0.f, 0.f);

    _matrix matWorld;

    switch (m_CameraSource.eType)
    {
    case CAMERA_SOURCE_TYPE::BONE:

        _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

        for (size_t i = 0; i < 3; i++)
            SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

        return SocketMatrix * XMLoadFloat4x4(m_pBoneCombinedMatrix);

    case CAMERA_SOURCE_TYPE::WORLD:

        vPosition = Update_CameraTrack(
            m_vPositionTracks,
            m_fCurrentAnimationTime,
            m_pCameraAnimationData
            ? m_pCameraAnimationData->vBaseBonePosition
            : _float3(0.f, 0.f, 0.f)
        );


        vRotation = Update_CameraTrack(
            m_vRotationTracks,
            m_fCurrentAnimationTime,
            m_pCameraAnimationData
            ? m_pCameraAnimationData->vBaseBoneRotation
            : _float3(0.f, 0.f, 0.f)
        );


        matWorld = XMMatrixAffineTransformation(
            XMVectorSet(1.f, 1.f, 1.f, 0.f),
            XMVectorZero(),
            XMQuaternionRotationRollPitchYawFromVector(XMVectorSet(XMConvertToRadians(vRotation.x), XMConvertToRadians(vRotation.y), XMConvertToRadians(vRotation.z), 0.f)),
            XMVectorSet(vPosition.x, vPosition.y, vPosition.z, 1.f)
		);

        return matWorld;

    case CAMERA_SOURCE_TYPE::LOOK_PIVOT:

        vPosition = Update_CameraTrack(
            m_vPositionTracks,
            m_fCurrentAnimationTime,
            m_pCameraAnimationData
            ? m_pCameraAnimationData->vBaseBonePosition
            : _float3(0.f, 0.f, 0.f)
        );


        vRotation = Update_CameraTrack(
            m_vRotationTracks,
            m_fCurrentAnimationTime,
            m_pCameraAnimationData
            ? m_pCameraAnimationData->vBaseBoneRotation
            : _float3(0.f, 0.f, 0.f)
        );


        matWorld = XMMatrixAffineTransformation(
            XMVectorSet(1.f, 1.f, 1.f, 0.f),
            XMVectorZero(),
            XMQuaternionRotationRollPitchYawFromVector(XMVectorSet(XMConvertToRadians(vRotation.x), XMConvertToRadians(vRotation.y), XMConvertToRadians(vRotation.z), 0.f)),
            XMVectorSet(vPosition.x, vPosition.y, vPosition.z, 1.f)
        );

        return matWorld;
    }

    return XMMatrixIdentity();
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

