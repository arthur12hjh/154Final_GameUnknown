#include "pch.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"
#include "CameraBone_Player.h"
#include "Nayitba.h"

#include "Trail.h"
#include "TrailEffect.h"
#include "Effect.h"

#include "Camera.h"

CCameraBone_Player::CCameraBone_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Parts{ pDevice, pContext }
{
}

CCameraBone_Player::CCameraBone_Player(const CCameraBone_Player& Prototype)
	: CPlayer_Parts{ Prototype }
{
}

HRESULT CCameraBone_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCameraBone_Player::Initialize(void* pArg)
{
	CAMERABONE_DESC* pDesc = static_cast<CAMERABONE_DESC*>(pArg);

	m_pParentTransformCom = pDesc->pParentTransform;
	m_pSocketMatrix = pDesc->pSocketMatrix;
	m_pCharacter = pDesc->pCharacter;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_fRotateX = 5.6f;
	//m_fRotateY = -0.5f;
	//m_fRotateZ = 13.5f;
	//
	//m_vLerpStep[0] = XMVectorSet(5.24f, -6.15f, 10.82f, 1.f);
	//m_vLerpStep[1] = XMVectorSet(8.4f, -6.15f, 10.82f, 1.f);
	//m_vLerpStep[2] = XMVectorSet(-4.24f, -6.15f, 10.82f, 1.f);

	strcpy_s(m_szAnimationTag, "CameraBone_Idle");
	m_pModelCom->Set_Animation(m_szAnimationTag, TRUE, 1.f, 1.f);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(5.6f, -0.5f, 13.5f, 1.f));
	m_pTransformCom->Rotation(XMConvertToRadians(0.f), XMConvertToRadians(-120.f), XMConvertToRadians(0.f));

	return S_OK;
} 

void CCameraBone_Player::Priority_Update(_float fTimeDelta)
{
	m_fCurrentAnimationTime += fTimeDelta;

	if (m_vPositionTracks.empty() && m_vRotationTracks.empty())
	{
		return;
	}

	_vector vPosition = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vRotation = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	// ㅋㅋ Camera_Action 코드 그대로 복붙해야지~~~~~~~~~~~~~~~~~~~~~ 날먹개꿀띠
	// 먼저 Position부터 보정해주자
	if (!m_vPositionTracks.empty())
	{
		while (m_iPositionTrackIndex + 1 < m_vPositionTracks.size() &&
			m_fCurrentAnimationTime >= m_vPositionTracks[m_iPositionTrackIndex].fTrackPosition)
		{
			++m_iPositionTrackIndex;
		}

		// 중괄호 치는 이유? 아래 Rotation도 있는데 변수명 겹쳐쓸거라 사고날 수도 있음 ;; 예방 차원 ㅇㅇ ;;
		{
			_float fRatio = Clamp(Get_PositionTrackRatio(), 0.f, 1.f);
			_float3 vBeforePosition = Get_BeforePosition();

			// m_vPositionTracks[m_iPositionTrackIndex].vTrackValue 바탕으로 보간
			// 에르미트 보간 공식에 따라 기울기값으로 계산한다.
			_vector vHermiteStart = XMVectorSet(m_vPositionTracks[m_iPositionTrackIndex].fTangentStart, m_vPositionTracks[m_iPositionTrackIndex].fTangentStart, m_vPositionTracks[m_iPositionTrackIndex].fTangentStart, 0.f);
			_vector vHermiteEnd = XMVectorSet(m_vPositionTracks[m_iPositionTrackIndex].fTangentEnd, m_vPositionTracks[m_iPositionTrackIndex].fTangentEnd, m_vPositionTracks[m_iPositionTrackIndex].fTangentEnd, 0.f);
			_vector vCurrentHermite = XMVectorHermite(XMLoadFloat3(&vBeforePosition), vHermiteStart, XMLoadFloat3(&m_vPositionTracks[m_iPositionTrackIndex].vTrackValue), vHermiteEnd, fRatio);

			vPosition = XMVectorSetW(vCurrentHermite, 1.f);

			m_pTransformCom->Set_State(STATE::POSITION, vPosition);
		}
	}

	// 다음은 Rotation
	if (!m_vRotationTracks.empty())
	{
		while (m_iRotationTrackIndex + 1 < m_vRotationTracks.size() &&
			m_fCurrentAnimationTime >= m_vRotationTracks[m_iRotationTrackIndex].fTrackPosition)
		{
			++m_iRotationTrackIndex;
		}

		{
			_float fRatio = Clamp(Get_RotationTrackRatio(), 0.f, 1.f);
			_float3 vBeforeRotation = Get_BeforeRotation();

			// m_vPositionTracks[m_iPositionTrackIndex].vTrackValue 바탕으로 보간
			// 에르미트 보간 공식에 따라 기울기값으로 계산한다.
			_vector vHermiteStart = XMVectorSet(m_vRotationTracks[m_iRotationTrackIndex].fTangentStart, m_vRotationTracks[m_iRotationTrackIndex].fTangentStart, m_vRotationTracks[m_iRotationTrackIndex].fTangentStart, 0.f);
			_vector vHermiteEnd = XMVectorSet(m_vRotationTracks[m_iRotationTrackIndex].fTangentEnd, m_vRotationTracks[m_iRotationTrackIndex].fTangentEnd, m_vRotationTracks[m_iRotationTrackIndex].fTangentEnd, 0.f);
			_vector vCurrentHermite = XMVectorHermite(XMLoadFloat3(&vBeforeRotation), vHermiteStart, XMLoadFloat3(&m_vRotationTracks[m_iRotationTrackIndex].vTrackValue), vHermiteEnd, fRatio);

			vRotation = XMVectorSetW(vCurrentHermite, 0.f);

			m_pTransformCom->Rotation(XMConvertToRadians(vRotation.m128_f32[0]), XMConvertToRadians(vRotation.m128_f32[1]), XMConvertToRadians(vRotation.m128_f32[2]));
		}
	}

}

void CCameraBone_Player::Update(_float fTimeDelta)
{
	if (TRUE == m_isAnimationPlayed)
	{
		_bool isAnimationFinished = m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, 0.f);
		if (TRUE == isAnimationFinished)
		{
			m_isAnimationPlayed = FALSE;

			m_pModelCom->Set_Animation("CameraBone_Idle", TRUE, 1.f, 0.2f);
				
			CCamera* pCamera = m_pGameInstance->GetCamrea(TEXT("PlayerCamera"));
			pCamera->Set_MainCamera(TEXT("PlayerCamera"));
			Safe_Release(pCamera);
		}
	}
}

void CCameraBone_Player::Late_Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	
	for (size_t i = 0; i < 3; i++)	
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	
	//XMStoreFloat4x4(&m_CombinedWorldMatrix,
	//	XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * SocketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
	
	m_ParentTransformMatrix = *m_pParentTransformCom->Get_WorldMatrixPtr();

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * SocketMatrix * XMLoadFloat4x4(&m_ParentTransformMatrix));

	//m_pGameInstance->ADD_Collider(m_pColliderCom);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

}

void CCameraBone_Player::Set_Animation(const _char* szAnimationTag, _bool isLoop, _float fAnimationPlayRate, _float fLerpDuration, _bool bIsRestart, _float fEndTrackPosition, _float fStartTrackPosition, _bool isResetTrackPosition, _bool isRootMotionUpdated)
{
	m_pModelCom->Set_Animation(szAnimationTag, isLoop, fAnimationPlayRate, fLerpDuration, bIsRestart, fEndTrackPosition, fStartTrackPosition, isResetTrackPosition, isRootMotionUpdated);
}

void CCameraBone_Player::Initilaize_CameraAnimationData(const CAMERA_ANIMATION_DATA& CameraAnimationData)
{
	m_fCurrentAnimationTime = 0.f;

	m_iPositionTrackIndex = 0;
	m_iRotationTrackIndex = 0;

	m_vPositionTracks.clear();
	m_vRotationTracks.clear();

	m_isAnimationPlayed = TRUE;

	m_CameraAnimationData = CameraAnimationData;

	strcpy_s(m_szAnimationTag, CameraAnimationData.szCameraAnimationName);
	
	m_pModelCom->Set_Animation(m_szAnimationTag, FALSE, 1.f, 0.2f, TRUE);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&CameraAnimationData.vBaseBonePosition), 1.f));
	for (const auto& pPositionTrack : CameraAnimationData.BonePositionTrackList)
	{
		m_vPositionTracks.push_back(pPositionTrack);
	}

	for (const auto& pRotationTrack : CameraAnimationData.BoneRotationTrackList)
	{
		m_vRotationTracks.push_back(pRotationTrack);
	}
}

_float CCameraBone_Player::Get_PositionTrackRatio()
{
	if (0 == m_iPositionTrackIndex)
		return m_fCurrentAnimationTime / m_vPositionTracks[m_iPositionTrackIndex].fTrackPosition;

	return (m_fCurrentAnimationTime - m_vPositionTracks[m_iPositionTrackIndex - 1].fTrackPosition)
		/ (m_vPositionTracks[m_iPositionTrackIndex].fTrackPosition - m_vPositionTracks[m_iPositionTrackIndex - 1].fTrackPosition);
}

_float CCameraBone_Player::Get_RotationTrackRatio()
{
	if (0 == m_iRotationTrackIndex)
		return m_fCurrentAnimationTime / m_vRotationTracks[m_iRotationTrackIndex].fTrackPosition;

	return (m_fCurrentAnimationTime - m_vRotationTracks[m_iRotationTrackIndex - 1].fTrackPosition)
		/ (m_vRotationTracks[m_iRotationTrackIndex].fTrackPosition - m_vRotationTracks[m_iRotationTrackIndex - 1].fTrackPosition);
}

_float3 CCameraBone_Player::Get_BeforePosition()
{
	if (0 == m_iPositionTrackIndex)
	{
		return m_CameraAnimationData.vBaseBonePosition;
	}

	return m_vPositionTracks[m_iPositionTrackIndex - 1].vTrackValue;
}

_float3 CCameraBone_Player::Get_BeforeRotation()
{
	if (0 == m_iRotationTrackIndex)
	{
		return m_CameraAnimationData.vBaseBoneRotation;
	}

	return m_vRotationTracks[m_iRotationTrackIndex - 1].vTrackValue;
}

HRESULT CCameraBone_Player::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_CameraBone"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Collider_AABB */
	CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};
	AABBDesc.vSize = _float3(1.5f, 2.f, 1.5f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;

	return S_OK;
}

CCameraBone_Player* CCameraBone_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCameraBone_Player* pInstance = new CCameraBone_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCameraBone_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCameraBone_Player::Clone(void* pArg)
{
	CCameraBone_Player* pInstance = new CCameraBone_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCameraBone_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCameraBone_Player::Free()
{

	__super::Free();

	Safe_Release(m_pColliderCom);
}

/* 예전 디버깅용 가라코드
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_O))
		m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) + XMVectorSet(10.f, 0.f, 0.f, 0.f));
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_I))
		m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) + XMVectorSet(0.f, 10.f, 0.f, 0.f));
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_U))
		m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) + XMVectorSet(0.f, 0.f, 10.f, 0.f));

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_L))
		m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) - XMVectorSet(10.f, 0.f, 0.f, 0.f));
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_K))
		m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) - XMVectorSet(0.f, 10.f, 0.f, 0.f));
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_J))
		m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) - XMVectorSet(0.f, 0.f, 10.f, 0.f));

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_X))
		m_bIsStop ^= TRUE;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fRotateX, m_fRotateY, m_fRotateZ, 1.f));
	m_pTransformCom->Rotation(XMConvertToRadians(m_fRotateX), XMConvertToRadians(m_fRotateY), XMConvertToRadians(m_fRotateZ));
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(1.f, -10.f, 0.f, 1.f));
	m_pTransformCom->Rotation(0.f, -90.f, 0.f);


*/