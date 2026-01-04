#include "pch.h"

#include "CinematicModel_Dororong.h"
#include "Body_Player.h"
#include "Face_Player.h"
#include "Hair_Player.h"
#include "PonyTail_Player.h"
#include "Weapon.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Effect.h"
#include "Notify.h"

#include "StringHelper.h"
#include "CinematicPartBody.h"

#include "Player.h"
#include "PlayerCCTHitReporter.h"
#include "PlayerBehaviorCallback.h"
#include "PlayerCCTQueryFilterCallback.h"


CCinematicModel_Dororong::CCinematicModel_Dororong(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCinematicObject{ pDevice, pContext }
{
}

CCinematicModel_Dororong::CCinematicModel_Dororong(const CCinematicModel_Dororong& Prototype)
	: CCinematicObject{ Prototype }
{
}

void CCinematicModel_Dororong::Active_SFX(const _wstring& strPartTag, const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)
{
	// 어떤 방식으로 짜야하지?
	// 1) strPartTag가 empty일 경우, CinematicModel_Dororong에서 탐색
	//    strPartTag가 있을 경우, strPartTag로 모델을 탐색
	// 2) strObjectTag라는 매핑된 값을 문자열 탐색해서 찾고, Play() 함수를 실행함
	// 3) 그 모델들은 시간이 지난 후 알아서 Stop()

	if (strPartTag.empty())
	{

	}
	else
	{
		Find_PartObject(strPartTag)->Active_SFX(strObjectTag, NotifyReference);
	}

}

void CCinematicModel_Dororong::Activate_PartObject_Collider(const _wstring& strPartTag, const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef)
{
	auto pPartObject = Find_PartObject(strPartTag);
	if (nullptr == pPartObject)
		return;

	pPartObject->Activate_PartObject_Collider(strColliderTag, NotifyRef);
}

HRESULT CCinematicModel_Dororong::CallNotify(_uint iNotiType, const AnimNotify* pNotify)
{
	CNotify::NOTIFY_TYPE NotiType = CNotify::NOTIFY_TYPE(iNotiType);

	return S_OK;
}

HRESULT CCinematicModel_Dororong::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCinematicModel_Dororong::Initialize(void* pArg)
{
	CCinematicObject::CINEMATICOBJECT_DESC* pDesc = (CCinematicObject::CINEMATICOBJECT_DESC*)pArg;

	m_szObjectTag = pDesc->szObjectTag;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	//m_pColliderCom->SetOwner(this);
	m_vRotationQuaternion = _float3(741.8724f, 3.0678f, 596.6966f);

	return S_OK;
}

void CCinematicModel_Dororong::Priority_Update(_float fTimeDelta)
{
	if (m_bIsActive == FALSE)
		return;

	m_pTransformCom->Update_PreWorldMatrix();

	__super::Priority_Update(fTimeDelta);
}

void CCinematicModel_Dororong::Update(_float fTimeDelta)
{
	/*if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_P))
		m_vRotationQuaternion.x += fTimeDelta;
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_O))
		m_vRotationQuaternion.x -= fTimeDelta;

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_L))
		m_vRotationQuaternion.y += fTimeDelta;
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_K))
		m_vRotationQuaternion.y -= fTimeDelta;

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_M))
		m_vRotationQuaternion.z += fTimeDelta;
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_N))
		m_vRotationQuaternion.z -= fTimeDelta;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vRotationQuaternion), 1.f));*/

	if (m_bIsActive == FALSE)
		return;

	__super::Update(fTimeDelta);

	switch (m_iCinematicCode)
	{
	case 2: // 도로롱 조우 시네마틱
		Play_Cinematic_Dororong_Meet(fTimeDelta);
		break;
	}

	//m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CCinematicModel_Dororong::Late_Update(_float fTimeDelta)
{
	if (m_bIsActive == FALSE)
		return;

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	//m_pGameInstance->ADD_Collider(m_pColliderCom);

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	__super::Late_Update(fTimeDelta);
}

HRESULT CCinematicModel_Dororong::Render()
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Render();

	return S_OK;
}

HRESULT CCinematicModel_Dororong::Render_Shadow()
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Render_Shadow();

	return S_OK;
}

HRESULT CCinematicModel_Dororong::ActiveCinematicObject(const CINEMATIC_NODE_DESC& CinematicNodeDesc)
{
	//m_bIsActive = TRUE;

	return S_OK;
}

HRESULT CCinematicModel_Dororong::PlayCinematicObject(const CINEMATIC_NODE_DESC& CinematicNodeDesc)
{
	m_bIsActive = TRUE;

	if (m_iCinematicCode != CinematicNodeDesc.iActiveIndex)
		m_iCinematicCode = CinematicNodeDesc.iActiveIndex;

	switch (m_iCinematicCode)
	{
	case 2: // 도로롱 조우 시네마틱
		Initialize_Cinematic_Dororong_Meet();
		break;
	}

	return S_OK;
}

HRESULT CCinematicModel_Dororong::Ready_Components()
{
	///* Com_Collider_AABB */
	//CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};
	//AABBDesc.vSize = _float3(1.5f, 2.f, 1.5f);
	//AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);
	//
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
	//	TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CCinematicModel_Dororong::Ready_PartObjects()
{
	CCinematicPartBody::BODY_CINEMATIC_DESC PartBodyDesc = {};
	PartBodyDesc.pParent = this;
	PartBodyDesc.pParentTransform = m_pTransformCom;
	PartBodyDesc.szModelTag = TEXT("Prototype_Component_Model_Cinematic_Dororong");
	PartBodyDesc.vScale = { 1.f, 1.f, 1.f };
	PartBodyDesc.fSpeedPerSec = 5.f;
	PartBodyDesc.fRotationPerSec = XMConvertToRadians(120.f);

	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CinematicPartBody"), TEXT("Part_Body"), &PartBodyDesc)))
		return E_FAIL;

	auto pPartBody = Find_PartObject(TEXT("Part_Body"));
	if (nullptr == pPartBody)
		return E_FAIL;

	Import_ModelPtr();

	return S_OK;
}

HRESULT CCinematicModel_Dororong::Initialize_Cinematic_Dororong_Meet()
{
	m_bIsActive = TRUE;
	m_iAnimationSequence = 0;
	m_fMoveTime = 0.f;
	m_pBodyModelCom->Set_Animation("MV_Nikke_Dororong_1stMeet_dororong_01", FALSE, 1.f, 0.f, TRUE);
	m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(681.378f, 34.203f, 350.178f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(679.378f, 34.203f, 350.178f, 1.f));

	return S_OK;
}

HRESULT CCinematicModel_Dororong::Play_Cinematic_Dororong_Meet(_float fTimeDelta)
{
	m_fMoveTime += fTimeDelta;
	_bool isFinished = Play_Animation(fTimeDelta, m_pTransformCom, 1.f);

	_uint iCurrentKeyFrameIndex = m_pBodyModelCom->Get_AnimationKeyFrameIndex();
	_float fCurrentTrackPosition = m_pBodyModelCom->Get_fTrackPosition();


	if (true == m_bIsDoFActivated)
	{
		_float fFocusDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) -
			m_pTransformCom->Get_State(STATE::POSITION)));

		m_pGameInstance->Set_DoFInfo(fFocusDist);

		m_fDoFTimeAcc += fTimeDelta;
	}

	if (m_iAnimationSequence == 1)
	{
		if (fCurrentTrackPosition >= 45.f
			&& fCurrentTrackPosition <= 60.f)
		{
			_float fRatio = Clamp((fCurrentTrackPosition - 45.f) / 15.f, 0.f, 1.f);
			_vector vPosition = XMVectorLerp(XMVectorSet(679.378f, 34.203f, 350.178f, 1.f), XMVectorSet(679.254f, 34.503f, 357.712f, 1.f), fRatio);

			m_pTransformCom->Set_State(STATE::POSITION, vPosition);
		}
		else if (fCurrentTrackPosition >= 60.f
				&& fCurrentTrackPosition <= 95.f)
		{
			_float fRatio = Clamp((fCurrentTrackPosition - 60.f) / 35.f, 0.f, 1.f);
			_vector vPosition = XMVectorLerp(XMVectorSet(679.254f, 34.503f, 357.712f, 1.f), XMVectorSet(685.190f, 30.696f, 389.852f, 1.f), fRatio);
			_vector vLookAt = XMVectorLerp(XMVectorSet(679.254f, 34.503f, 358.712f, 1.f), XMVectorSet(685.690f, 30.536f, 391.852f, 1.f), fRatio);

			m_pTransformCom->Set_State(STATE::POSITION, vPosition);
			m_pTransformCom->LookAt(vLookAt);
		}

		if (fCurrentTrackPosition >= 80.f
			&& m_bIsDoFActivated == TRUE)
		{
			m_pGameInstance->Active_DoF(false, 0.f);
			m_fDoFTimeAcc = 0.f;
			m_bIsDoFActivated = FALSE;
		}
	}
	

	if (isFinished)
	{
		++m_iAnimationSequence;
		if (m_iAnimationSequence == 4)
		{
			m_bIsActive = FALSE;
			m_iCinematicCode = -1;
		}
		else if (m_iAnimationSequence == 1)
		{
			m_pBodyModelCom->Set_Animation("MV_Nikke_Dororong_1stMeet_dororong_02", FALSE, 1.f, 0.f, FALSE, 95.f);
			m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(679.378f, 34.203f, 350.178f, 1.f));
			m_pTransformCom->LookAt(XMVectorSet(679.378f, 34.203f, 351.178f, 1.f));
		}
		else if (m_iAnimationSequence == 2)
		{
			m_pBodyModelCom->Set_Animation("MV_Nikke_Dororong_1stMeet_dororong_03", FALSE, 1.f, 0.f, FALSE);
		}
		else if (m_iAnimationSequence == 3)
		{
			m_pBodyModelCom->Set_Animation("MV_Nikke_Dororong_1stMeet_dororong_04", FALSE, 2.f, 0.12f, FALSE);
			//m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(251.874f, 8.f, 230.907f, 1.f));
			//m_pTransformCom->LookAt(XMVectorSet(251.874f, 8.f, 233.907f, 1.f));
		}

	}

	return S_OK;
}

void CCinematicModel_Dororong::Reset_CinematicChanges()
{
	if (m_bIsPrevActivated == TRUE
		&& m_bIsActive == FALSE)
	{
		m_iCinematicCode = -1;
		m_pGameInstance->Active_DoF(false, 0.f);
	}

	m_bIsPrevActivated = m_bIsActive;
}

CCinematicModel_Dororong* CCinematicModel_Dororong::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCinematicModel_Dororong* pInstance = new CCinematicModel_Dororong(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCinematicModel_Dororong");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinematicModel_Dororong::Clone(void* pArg)
{
	CCinematicModel_Dororong* pInstance = new CCinematicModel_Dororong(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCinematicModel_Dororong");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinematicModel_Dororong::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
