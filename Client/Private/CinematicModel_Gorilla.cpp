#include "pch.h"

#include "CinematicModel_Gorilla.h"
#include "NayitbaPartBody.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Effect.h"
#include "Notify.h"

#include "Player.h"
#include "PlayerCCTHitReporter.h"
#include "PlayerBehaviorCallback.h"
#include "PlayerCCTQueryFilterCallback.h"


CCinematicModel_Gorilla::CCinematicModel_Gorilla(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCinematicObject{ pDevice, pContext }
{
}

CCinematicModel_Gorilla::CCinematicModel_Gorilla(const CCinematicModel_Gorilla& Prototype)
	: CCinematicObject{ Prototype }
{
}

void CCinematicModel_Gorilla::Active_SFX(const _wstring& strPartTag, const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)
{
	// 어떤 방식으로 짜야하지?
	// 1) strPartTag가 empty일 경우, CinematicModel_Gorilla에서 탐색
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

void CCinematicModel_Gorilla::Activate_PartObject_Collider(const _wstring& strPartTag, const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef)
{
	auto pPartObject = Find_PartObject(strPartTag);
	if (nullptr == pPartObject)
		return;

	pPartObject->Activate_PartObject_Collider(strColliderTag, NotifyRef);
}

HRESULT CCinematicModel_Gorilla::CallNotify(_uint iNotiType, const AnimNotify* pNotify)
{
	CNotify::NOTIFY_TYPE NotiType = CNotify::NOTIFY_TYPE(iNotiType);

	return S_OK;
}

HRESULT CCinematicModel_Gorilla::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCinematicModel_Gorilla::Initialize(void* pArg)
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
	//Find_PartObject(TEXT("Part_Body"))->Find_Component(TEXT())

	return S_OK;
}

void CCinematicModel_Gorilla::Priority_Update(_float fTimeDelta)
{
	Reset_CinematicChanges();

	if (m_bIsActive == FALSE)
		return;

	m_pTransformCom->Update_PreWorldMatrix();

	__super::Priority_Update(fTimeDelta);
}

void CCinematicModel_Gorilla::Update(_float fTimeDelta)
{
	if (m_bIsActive == FALSE)
		return;

	__super::Update(fTimeDelta);

	switch (m_iCinematicCode)
	{
	case 0: // 고릴라 만남 시네마틱
		Play_Cinematic_GorillaMeet(fTimeDelta);
		break;
	case 1: // 고릴라 처형 시네마틱
		Play_Cinematic_GorillaFinish(fTimeDelta);
		break;
	}

	//m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CCinematicModel_Gorilla::Late_Update(_float fTimeDelta)
{
	if (m_bIsActive == FALSE)
	{
		return;
	}

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	//m_pGameInstance->ADD_Collider(m_pColliderCom);

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	__super::Late_Update(fTimeDelta);

}

HRESULT CCinematicModel_Gorilla::Render()
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Render();

	return S_OK;
}

HRESULT CCinematicModel_Gorilla::Render_Shadow()
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Render_Shadow();

	return S_OK;
}

HRESULT CCinematicModel_Gorilla::ActiveCinematicObject(const CINEMATIC_NODE_DESC& CinematicNodeDesc)
{
	//m_bIsActive = TRUE;

	return S_OK;
}

HRESULT CCinematicModel_Gorilla::PlayCinematicObject(const CINEMATIC_NODE_DESC& CinematicNodeDesc)
{
	m_bIsActive = TRUE;

	if (m_iCinematicCode != CinematicNodeDesc.iActiveIndex)
		m_iCinematicCode = CinematicNodeDesc.iActiveIndex;

	switch (m_iCinematicCode)
	{
	case 0: // 고릴라 만남 시네마틱
		Initialize_Cinematic_GorillaMeet();
		break;
	case 1: // 고릴라 처형 시네마틱
		Initialize_Cinematic_GorillaFinish();
		break;
	}

	return S_OK;
}

HRESULT CCinematicModel_Gorilla::Ready_Components()
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

HRESULT CCinematicModel_Gorilla::Ready_PartObjects()
{
	CNayitbaPartBody::NAYITBA_PART_BODY_DESC BodyDesc = { };
	BodyDesc.pParentTransform = m_pTransformCom;
	BodyDesc.vScale = { 1.f, 1.f, 1.f };
	BodyDesc.szBodyModel = TEXT("Prototype_Component_Model_Cinematic_Gorilla");
	BodyDesc.fSpeedPerSec = 5.f;
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba_Body"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;
	Import_ModelPtr();

	m_pNotifyCom->Set_ModelCom(m_pBodyModelCom);

	return S_OK;
}

HRESULT CCinematicModel_Gorilla::Initialize_Cinematic_GorillaMeet()
{
	m_bIsActive = TRUE;
	m_iAnimationSequence = 0;
	m_fMoveTime = 0.f;
	m_pBodyModelCom->Set_Animation("M_Gorilla_S12_Crush", FALSE, 1.f, 0.12f, TRUE, 37.f, 0.f, TRUE, TRUE);
	m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(760.197f, 46.912f, 700.319f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(741.9f, 46.912f, 637.93f, 1.f));

	return S_OK;
}

/// <고릴라 애니메이션>
/// M_Gorilla_S12_Crush
/// MV_Quest_Sub_033_Gorilla_NA05_01
/// M_Gorilla_S20_ParryMode
/// M_Gorilla_Finish02_nonbreak_v03
/// </고릴라 애니메이션>

HRESULT CCinematicModel_Gorilla::Initialize_Cinematic_GorillaFinish()
{
	m_bIsActive = TRUE;
	m_iAnimationSequence = 0;
	m_fMoveTime = 0.f;
	m_pBodyModelCom->Set_Animation("M_Gorilla_Finish02_nonbreak_v03", FALSE, 1.5f, 0.f, TRUE);
	m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(738.66f, 2.022f, 608.569f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(738.66f, 2.022f, 607.569f, 1.f));

	return S_OK;
}

static _bool isDofActivated = false;
static _float fTimeAcc = { 0.f };

HRESULT CCinematicModel_Gorilla::Play_Cinematic_GorillaMeet(_float fTimeDelta)
{
	m_fMoveTime += fTimeDelta;
	_bool isFinished = Play_Animation(fTimeDelta, m_pTransformCom, 1.f);

	if (m_fMoveTime > 1.2f && m_fMoveTime < 2.8f)
	{
		_float fRatio = (m_fMoveTime - 1.2f) / 1.6f;
		_vector vPosition = XMVectorLerp(XMVectorSet(760.197f, 0.f, 700.319f, 1.f), XMVectorSet(741.9f, 0.f, 637.93f, 1.f), fRatio);
		_float fHighestPoint = 60.f;
		_float fTime = m_fMoveTime - 1.2f;

		_float fPositionY = Lerp(46.912f, 0.5f, fRatio) + 4.f * fHighestPoint * fRatio * (1.f - fRatio);

		vPosition = XMVectorSetY(vPosition, fPositionY);

		m_pTransformCom->Set_State(STATE::POSITION, vPosition);
	}

#pragma region GARA_DOF
	if (false == isDofActivated)
	{
		_float fFocusDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) -
			m_pTransformCom->Get_State(STATE::POSITION)));
		_float fMaxDist = 5.f;
		_float fIntensity = 1.5f;

		m_pGameInstance->Active_DoF(true, 0.2f);
		m_pGameInstance->Set_DoFInfo(fFocusDist, fMaxDist, fIntensity);

		isDofActivated = true;
	}

	if (true == isDofActivated)
	{
		_float fFocusDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) -
			m_pTransformCom->Get_State(STATE::POSITION)));

		m_pGameInstance->Set_DoFInfo(fFocusDist);

		fTimeAcc += fTimeDelta;
	}
#pragma endregion
	if (isFinished)
	{
		++m_iAnimationSequence;
		if (m_iAnimationSequence == 3)
		{
			m_bIsActive = FALSE;
			m_iCinematicCode = -1;
			m_pGameInstance->Active_DoF(false, 0.f);
		}
		else if (m_iAnimationSequence == 1)
		{
			m_pBodyModelCom->Set_Animation("MV_Quest_Sub_033_Gorilla_NA05_01", FALSE, 1.f, 0.12f, TRUE);
		}
		else if (m_iAnimationSequence == 2)
		{
			m_pBodyModelCom->Set_Animation("M_Gorilla_S20_ParryMode", FALSE, 1.f, 0.12f, FALSE, -1.f, 34.f, TRUE, TRUE);
			m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(743.f, 2.94f, 635.f, 1.f));
		}

		isDofActivated = false;
		fTimeAcc = 0.f;
	}

	if (m_fMoveTime > 7.9f)
	{
		m_bIsActive = FALSE;
		m_iCinematicCode = -1;
	}

	return S_OK;
}

HRESULT CCinematicModel_Gorilla::Play_Cinematic_GorillaFinish(_float fTimeDelta)
{
	m_fMoveTime += fTimeDelta;
	_bool isFinished = Play_Animation(fTimeDelta, m_pTransformCom, 1.f);

	_float fCurrentTrackPosition = m_pBodyModelCom->Get_fTrackPosition();

	if (true == m_bIsDoFActivated)
	{
		_float fFocusDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) -
			m_pTransformCom->Get_State(STATE::POSITION)));

		m_pGameInstance->Set_DoFInfo(fFocusDist);

		m_fDoFTimeAcc += fTimeDelta;
	}

	if (fCurrentTrackPosition >= 426.f
		&& fCurrentTrackPosition < 550.f
		&& m_bIsDoFActivated == FALSE)
	{
		_float fFocusDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) -
			m_pTransformCom->Get_State(STATE::POSITION)));
		_float fMaxDist = 5.f;
		_float fIntensity = 1.5f;

		m_pGameInstance->Active_DoF(true, 1.f);
		m_pGameInstance->Set_DoFInfo(fFocusDist, fMaxDist, fIntensity);

		m_bIsDoFActivated = TRUE;

	}else if (fCurrentTrackPosition >= 550.f
		&& m_bIsDoFActivated == TRUE)
	{
		m_pGameInstance->Active_DoF(false, 1.f);
		m_fDoFTimeAcc = 0.f;
		m_bIsDoFActivated = FALSE;
	}

	if (isFinished)
	{
		m_bIsActive = FALSE;
		m_iCinematicCode = -1;
	}

	return S_OK;
}

void CCinematicModel_Gorilla::Reset_CinematicChanges()
{
	if (m_bIsPrevActivated == TRUE
		&& m_bIsActive == FALSE)
	{
		m_iCinematicCode = -1;
		m_pGameInstance->Active_DoF(false, 0.f);
	}

	m_bIsPrevActivated = m_bIsActive;

}

CCinematicModel_Gorilla* CCinematicModel_Gorilla::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCinematicModel_Gorilla* pInstance = new CCinematicModel_Gorilla(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCinematicModel_Gorilla");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinematicModel_Gorilla::Clone(void* pArg)
{
	CCinematicModel_Gorilla* pInstance = new CCinematicModel_Gorilla(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCinematicModel_Gorilla");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinematicModel_Gorilla::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
