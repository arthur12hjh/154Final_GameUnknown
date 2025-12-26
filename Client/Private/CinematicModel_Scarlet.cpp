#include "pch.h"

#include "CinematicModel_Scarlet.h"
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
#include "NayitbaPartBody.h"
#include "NaytibaLeftWeaponPart.h"
#include "NaytibaRightWeaponPart.h"

#include "Player.h"
#include "PlayerCCTHitReporter.h"
#include "PlayerBehaviorCallback.h"
#include "PlayerCCTQueryFilterCallback.h"


CCinematicModel_Scarlet::CCinematicModel_Scarlet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCinematicObject{ pDevice, pContext }
{
}

CCinematicModel_Scarlet::CCinematicModel_Scarlet(const CCinematicModel_Scarlet& Prototype)
	: CCinematicObject{ Prototype }
{
}

void CCinematicModel_Scarlet::Active_SFX(const _wstring& strPartTag, const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)
{
	// 어떤 방식으로 짜야하지?
	// 1) strPartTag가 empty일 경우, CinematicModel_Scarlet에서 탐색
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

void CCinematicModel_Scarlet::Activate_PartObject_Collider(const _wstring& strPartTag, const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef)
{
	auto pPartObject = Find_PartObject(strPartTag);
	if (nullptr == pPartObject)
		return;

	pPartObject->Activate_PartObject_Collider(strColliderTag, NotifyRef);
}

HRESULT CCinematicModel_Scarlet::CallNotify(_uint iNotiType, const AnimNotify* pNotify)
{
	CNotify::NOTIFY_TYPE NotiType = CNotify::NOTIFY_TYPE(iNotiType);

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Initialize(void* pArg)
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

void CCinematicModel_Scarlet::Priority_Update(_float fTimeDelta)
{
	if (m_bIsActive == FALSE)
		return;

	m_pTransformCom->Update_PreWorldMatrix();

	__super::Priority_Update(fTimeDelta);
}

void CCinematicModel_Scarlet::Update(_float fTimeDelta)
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
	case 20: // 홍련 전투 조우
		Play_Cinematic_Scarlet_Battle_Enter(fTimeDelta);
		break;
	case 21: // 홍련 전투 페이즈 전환
		Play_Cinematic_Scarlet_Battle_PhaseChange(fTimeDelta);
		break;
	case 22: // 홍련 전투 마무리
		Play_Cinematic_Scarlet_Battle_Finish(fTimeDelta);
		break;
	}

	//m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CCinematicModel_Scarlet::Late_Update(_float fTimeDelta)
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

HRESULT CCinematicModel_Scarlet::Render()
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Render();

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Render_Shadow()
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Render_Shadow();

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::ActiveCinematicObject(const CINEMATIC_NODE_DESC& CinematicNodeDesc)
{
	//m_bIsActive = TRUE;

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::PlayCinematicObject(const CINEMATIC_NODE_DESC& CinematicNodeDesc)
{
	m_bIsActive = TRUE;

	if (m_iCinematicCode != CinematicNodeDesc.iActiveIndex)
		m_iCinematicCode = CinematicNodeDesc.iActiveIndex;

	switch (m_iCinematicCode)
	{
	case 20: // 홍련 전투 조우
		Initialize_Cinematic_Scarlet_Battle_Enter();
		break;
	case 21: // 홍련 전투 페이즈 전환
		Initialize_Cinematic_Scarlet_Battle_PhaseChange();
		break;
	case 22: // 홍련 전투 마무리
		Initialize_Cinematic_Scarlet_Battle_Finish();
		break;
	}

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Ready_Components()
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

HRESULT CCinematicModel_Scarlet::Ready_PartObjects()
{
	CNayitbaPartBody::NAYITBA_PART_BODY_DESC BodyDesc = { };
	BodyDesc.pParentTransform = m_pTransformCom;
	BodyDesc.vScale = { 1.f, 1.f, 1.f };
	BodyDesc.szBodyModel = TEXT("Prototype_Component_Model_Scarlet_Body");
	BodyDesc.fSpeedPerSec = 5.f;
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba_Body"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	Import_ModelPtr();

	CNaytibaLeftWeaponPart::WEAPON_DESC LWeaponDesc = { };
	LWeaponDesc.pParentTransform = m_pTransformCom;
	LWeaponDesc.pSocketMatrix = m_pBodyModelCom->Get_BoneMatrixPtr("Weapon");
	LWeaponDesc.vScale = { 1.f, 1.f, 1.f };
	//LWeaponDesc.vRotation = {(90.f), (-90.f), (0.f), 1.f};
	//LWeaponDesc.vRotation = {XMConvertToRadians(90.f), XMConvertToRadians(-90.f), XMConvertToRadians(0.f), 1.f};;
	swprintf_s(LWeaponDesc.szWeaponModelPrototype, 256, TEXT("Prototype_Component_Model_Scarlet_Weapon"));
	LWeaponDesc.fSpeedPerSec = 5.f;
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba_Left_Weapon"), TEXT("Part_WeaponL"), &LWeaponDesc)))
		return E_FAIL;

	CNaytibaRightWeaponPart::WEAPON_DESC RWeaponDesc = { };
	RWeaponDesc.pParentTransform = m_pTransformCom;
	RWeaponDesc.pSocketMatrix = m_pBodyModelCom->Get_BoneMatrixPtr("SC_AssistWeapon");
	RWeaponDesc.vScale = { 1.f, 1.f, 1.f };
	//RWeaponDesc.vRotation = { (90.f), (-81.5f), (0.f), 1.f };
	//RWeaponDesc.vRotation = { XMConvertToRadians(90.f), XMConvertToRadians(-81.5f), XMConvertToRadians(0.f), 1.f };
	swprintf_s(RWeaponDesc.szWeaponModelPrototype, 256, TEXT("Prototype_Component_Model_Scarlet_Scabbard"));
	RWeaponDesc.fSpeedPerSec = 5.f;
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba_Right_Weapon"), TEXT("Part_WeaponR"), &RWeaponDesc)))
		return E_FAIL;

	m_pNotifyCom->Set_ModelCom(m_pBodyModelCom);

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Initialize_Cinematic_Scarlet_Battle_Enter()
{
	m_bIsActive = TRUE;
	m_iAnimationSequence = 0;
	m_fMoveTime = 0.f;
	m_pBodyModelCom->Set_Animation("MV_Nikke_Scarlet_Entrance_Scarlet_01", FALSE, 1.f);
	m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(258.315f, 21.822f, 331.387f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(258.315f, 21.822f, 332.387f, 1.f));

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Initialize_Cinematic_Scarlet_Battle_PhaseChange()
{
	m_bIsActive = TRUE;
	m_iAnimationSequence = 0;
	m_fMoveTime = 0.f;
	//m_pBodyModelCom->Set_Animation("MV_Nikke_Scarlet_Phase2_seq_Scarlet_ANI01", FALSE, 3.f, 0.12f, false, -1.f, 30.f);
	m_pBodyModelCom->Set_Animation("MV_Nikke_Scarlet_Phase2_seq_Scarlet_ANI01", FALSE, 3.f);
	m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(251.874f, 8.f, 234.907f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(252.874f, 8.f, 234.907f, 1.f));

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Initialize_Cinematic_Scarlet_Battle_Finish()
{
	m_bIsActive = TRUE;
	m_iAnimationSequence = 0;
	m_fMoveTime = 0.f;
	m_pBodyModelCom->Set_Animation("MV_Nikke_Scarlet_QTE_Step1_Scarlet_01", FALSE, 2.f, 0.f, FALSE, 1400.f);
	m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(251.874f, 8.f, 234.907f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(252.874f, 8.f, 234.907f, 1.f));

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Play_Cinematic_Scarlet_Battle_Enter(_float fTimeDelta)
{
	m_fMoveTime += fTimeDelta;
	_bool isFinished = Play_Animation(fTimeDelta, m_pTransformCom, 1.f);

	if (m_iAnimationSequence == 1)
	{
		_uint iCurrentKeyFrameIndex = m_pBodyModelCom->Get_AnimationKeyFrameIndex();

		if (iCurrentKeyFrameIndex > 1129
			&& iCurrentKeyFrameIndex <= 1169)
		{
			_float fRatio = (iCurrentKeyFrameIndex - 1129) / 40;
			_vector vPosition = XMVectorLerp(XMVectorSet(257.977f, 4.039f, 256.289f, 1.f), XMVectorSet(257.977f, 8.f, 256.289f, 1.f), fRatio);

			m_pTransformCom->Set_State(STATE::POSITION, vPosition);
		}
	}

	if (isFinished)
	{
		++m_iAnimationSequence;
		if (m_iAnimationSequence == 2)
		{
			m_bIsActive = FALSE;
			m_iCinematicCode = -1;
		}
		else if (m_iAnimationSequence == 1)
		{
			m_pBodyModelCom->Set_Animation("MV_Nikke_Scarlet_Entrance_Scarlet_03", FALSE, 1.f, 0.f, FALSE);	
			m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(257.977f, 4.039f, 256.289f, 1.f));
			m_pTransformCom->LookAt(XMVectorSet(257.977f, 4.039f, 255.289f, 1.f));
		}

	}

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Play_Cinematic_Scarlet_Battle_PhaseChange(_float fTimeDelta)
{
	/*
		34.5초 술 건네기
		45초 마시기
		48초 다마심
		56초 엔딩
	*/

	m_fMoveTime += fTimeDelta;
	_bool isFinished = Play_Animation(fTimeDelta, m_pTransformCom, 1.f);

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
			m_pBodyModelCom->Set_Animation("MV_Nikke_Scarlet_Phase2_seq_Scarlet_ANI02", FALSE, 3.f, 0.12f, FALSE);
		}
		else if (m_iAnimationSequence == 2)
		{
			m_pBodyModelCom->Set_Animation("MV_Nikke_Scarlet_Phase2_seq_Scarlet_ANI03", FALSE, 3.f, 0.12f, FALSE, 850.f);
		}
		else if (m_iAnimationSequence == 3)
		{
			m_pBodyModelCom->Set_Animation("MV_Nikke_Scarlet_Phase2_seq_Scarlet_ANI04", FALSE, 2.f, 0.12f, FALSE, -1.f, 243.f);
			m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(251.874f, 8.f, 230.907f, 1.f));
			m_pTransformCom->LookAt(XMVectorSet(251.874f, 8.f, 233.907f, 1.f));
		}

	}

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Play_Cinematic_Scarlet_Battle_Finish(_float fTimeDelta)
{
	m_fMoveTime += fTimeDelta;
	_bool isFinished = Play_Animation(fTimeDelta, m_pTransformCom, 1.f);

	if (isFinished)
	{
		++m_iAnimationSequence;
		if (m_iAnimationSequence == 3)
		{
			m_bIsActive = FALSE;
			m_iCinematicCode = -1;
		}
		else if (m_iAnimationSequence == 1)
		{
			m_pBodyModelCom->Set_Animation("MV_Nikke_Scarlet_QTE_AfterBattle_Scarlet_ANI01", FALSE, 2.f, 0.f, FALSE);
			m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(248.529f, 8.f, 219.061f, 1.f));
			m_pTransformCom->LookAt(XMVectorSet(248.529f, 8.f, 219.061f, 1.f));
		}
		else if (m_iAnimationSequence == 2)
		{
			m_pBodyModelCom->Set_Animation("MV_Nikke_Scarlet_QTE_AfterBattle_Scarlet_ANI02", FALSE, 2.f, 0.f, FALSE);
		}

	}

	return S_OK;
}

void CCinematicModel_Scarlet::Reset_CinematicChanges()
{
	if (m_bIsPrevActivated == TRUE
		&& m_bIsActive == FALSE)
	{
		m_iCinematicCode = -1;
		m_pGameInstance->Active_DoF(false, 0.f);
	}

	m_bIsPrevActivated = m_bIsActive;
}

CCinematicModel_Scarlet* CCinematicModel_Scarlet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCinematicModel_Scarlet* pInstance = new CCinematicModel_Scarlet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCinematicModel_Scarlet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinematicModel_Scarlet::Clone(void* pArg)
{
	CCinematicModel_Scarlet* pInstance = new CCinematicModel_Scarlet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCinematicModel_Scarlet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinematicModel_Scarlet::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
