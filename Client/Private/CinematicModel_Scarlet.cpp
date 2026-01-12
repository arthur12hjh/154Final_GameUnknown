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
#include "NaytibaFace.h"

#include "StringHelper.h"
#include "NayitbaPartBody.h"
#include "NaytibaLeftWeaponPart.h"
#include "NaytibaRightWeaponPart.h"
#include "CinematicPartBody.h"
#include "CinematicPartFacial.h"

#include "Player.h"
#include "PlayerCCTHitReporter.h"
#include "PlayerBehaviorCallback.h"
#include "PlayerCCTQueryFilterCallback.h"
#include "CinematicModel_Dororong.h"


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

	m_pBottle->Set_Render(FALSE);
	m_pGlass->Set_Render(FALSE);

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
	case 3: // 홍련 첫 만남
		Play_Cinematic_Scarlet_FirstMeet(fTimeDelta);
		break;
	case 10: // 홍련 이별
		Play_Cinematic_Scarlet_GoodBye(fTimeDelta);
		break;
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
	//m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
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
	//for (auto& pPartObject : m_PartObjects)
	//	pPartObject.second->Render_Shadow();

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
	case 3: // 홍련 첫 만남
		Initialize_Cinematic_Scarlet_FirstMeet();
		break;
	case 10: // 홍련 이별
		Initialize_Cinematic_Scarlet_GoodBye();
		break;
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

HRESULT CCinematicModel_Scarlet::Set_Cinematic_Object(const ANIM_NOTIFY& NotifyReference)
{
	_TCHAR szPartTag[MAX_PATH];
	CStringHelper::ConvertUTFToWide(NotifyReference.szNotifyArg01.c_str(), szPartTag);
	
	auto pPartObject = static_cast<CCinematicPartBody*>(Find_PartObject(szPartTag));

	pPartObject->Reset_SocketMatrix(const_cast<_float4x4*>(m_pBodyModelCom->Get_BoneMatrixPtr(NotifyReference.szNotifyArg02.c_str())));

	pPartObject->GetTransform()->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&NotifyReference.vNotifyPosition), 1.f));
	pPartObject->GetTransform()->Rotation(XMConvertToRadians(NotifyReference.vNotifyRotation.x),
		XMConvertToRadians(NotifyReference.vNotifyRotation.y),
		XMConvertToRadians(NotifyReference.vNotifyRotation.z));

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
	m_pNotifyCom->Set_ModelCom(m_pBodyModelCom);	

	CCinematicPartFacial::FACIAL_CINEMATIC_DESC FaceDesc = { };
	FaceDesc.pParentTransform = m_pTransformCom;
	FaceDesc.pSocketMatrix = nullptr;
	FaceDesc.vScale = { 1.f, 1.f, 1.f };
	FaceDesc.isAnim = TRUE;
	FaceDesc.szModelTag = TEXT("Prototype_Component_Model_Scarlet_Face_Morph");
	FaceDesc.pBody = m_pBodyModelCom;
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CinematicPartFacial"), TEXT("Part_Face"), &FaceDesc)))
		return E_FAIL;

	m_pFacial = static_cast<CModel*>(static_cast<CCinematicPartFacial*>(Find_PartObject(TEXT("Part_Face")))->Find_Component(TEXT("Com_Model")));

	CCinematicPartBody::BODY_CINEMATIC_DESC	WeaponDesc{};
	WeaponDesc.szModelTag = TEXT("Prototype_Component_Model_Scarlet_Weapon");
	WeaponDesc.pSocketMatrix = m_pBodyModelCom->Get_BoneMatrixPtr("Weapon");
	WeaponDesc.isSetTransform = TRUE;
	WeaponDesc.isAnim = FALSE;
	WeaponDesc.vPartPosition = { 0.f, 0.f, 0.f };
	WeaponDesc.vPartRotation = { 90.f, -90.f, 0.f };
	WeaponDesc.vPartScale = { 1.f, 1.f, 1.f };
	WeaponDesc.pParentTransform = m_pTransformCom;
	WeaponDesc.fSpeedPerSec = 5.f;

	/* Part_Weapon */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CinematicPartBody"),
		TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;

	CCinematicPartBody::BODY_CINEMATIC_DESC	ScabbardDesc{};
	ScabbardDesc.szModelTag = TEXT("Prototype_Component_Model_Scarlet_Scabbard");
	ScabbardDesc.pSocketMatrix = m_pBodyModelCom->Get_BoneMatrixPtr("SC_AssistWeapon");
	ScabbardDesc.isSetTransform = TRUE;
	ScabbardDesc.isAnim = FALSE;
	ScabbardDesc.vPartPosition = { 0.f, 0.f, 0.f };
	ScabbardDesc.vPartRotation = { 90.f, -81.5f, 0.f };
	ScabbardDesc.vPartScale = { 1.f, 1.f, 1.f };
	ScabbardDesc.pParentTransform = m_pTransformCom;
	ScabbardDesc.fSpeedPerSec = 5.f;

	/* Part_Scabbard */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CinematicPartBody"),
		TEXT("Part_Scabbard"), &ScabbardDesc)))
		return E_FAIL;

	CCinematicPartBody::BODY_CINEMATIC_DESC	BottleDesc{};
	BottleDesc.szModelTag = TEXT("Prototype_Component_Model_Bottle_Scarlet");
	BottleDesc.pSocketMatrix = nullptr;
	BottleDesc.isSetTransform = FALSE;
	BottleDesc.isAnim = FALSE;
	BottleDesc.vPartPosition = {0.f, 0.f, 0.12f};
	BottleDesc.vPartRotation = {270.f, 0.f, 0.f};
	BottleDesc.vPartScale = { 1.f, 1.f, 1.f };
	BottleDesc.pParentTransform = m_pTransformCom;
	BottleDesc.fSpeedPerSec = 5.f;

	/* Part_Bottle */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CinematicPartBody"),
		TEXT("Part_Bottle"), &BottleDesc)))
		return E_FAIL;

	m_pBottle = static_cast<CCinematicPartBody*>(Find_PartObject(TEXT("Part_Bottle")));

	CCinematicPartBody::BODY_CINEMATIC_DESC	GlassDesc{};
	GlassDesc.szModelTag = TEXT("Prototype_Component_Model_Glass_Scarlet");
	GlassDesc.pSocketMatrix = nullptr;
	GlassDesc.isSetTransform = FALSE;
	GlassDesc.isAnim = FALSE;
	GlassDesc.vPartPosition = { -0.015f, -0.0f, -0.01f };
	GlassDesc.vPartRotation = { 270.f, 0.f, 0.f };
	GlassDesc.vPartScale = { 1.f, 1.f, 1.f };
	GlassDesc.pParentTransform = m_pTransformCom;
	GlassDesc.fSpeedPerSec = 5.f;

	/* Part_Glass */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CinematicPartBody"),
		TEXT("Part_Glass"), &GlassDesc)))
		return E_FAIL;

	m_pGlass = static_cast<CCinematicPartBody*>(Find_PartObject(TEXT("Part_Glass")));


	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Initialize_Cinematic_Scarlet_FirstMeet()
{
	m_bIsActive = TRUE;
	m_iAnimationSequence = 0;
	m_fMoveTime = 0.f;
	m_pBodyModelCom->Set_Animation("MV_Nikke_ScarletVolt_1stMeet_NA_961_01_05", FALSE, 2.f, 0.f, TRUE);
	m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(741.467f, 2.f, 646.534f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(741.467f, 2.f, 636.534f, 1.f));
	static_cast<CModel*>(static_cast<CCinematicPartFacial*>(Find_PartObject(TEXT("Part_Face")))->Find_Component(TEXT("Com_Model")))->Set_MorphAnimation("ScarletVolt_1stMeet_Scarlet_Curves", TRUE, 1.2f, 0.f, FALSE, -1.f, 26722.f);
	static_cast<CCinematicPartBody*>(Find_PartObject(TEXT("Part_Weapon")))->SetActive(TRUE);
	static_cast<CCinematicPartBody*>(Find_PartObject(TEXT("Part_Scabbard")))->SetActive(TRUE);

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Initialize_Cinematic_Scarlet_GoodBye()
{
	m_bIsActive = TRUE;
	m_iAnimationSequence = 0;
	m_fMoveTime = 0.f;
	m_pBodyModelCom->Set_Animation("MV_Nikke_ScarletVolt_GoodBye_Scarlet_b2", FALSE, 2.f, 0.f, TRUE);
	m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(812.065f, 98.135f, 1517.918f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(812.065f, 98.135f, 1507.918f, 1.f));
	static_cast<CCinematicPartBody*>(Find_PartObject(TEXT("Part_Weapon")))->SetActive(FALSE);
	static_cast<CCinematicPartBody*>(Find_PartObject(TEXT("Part_Scabbard")))->SetActive(FALSE);

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Initialize_Cinematic_Scarlet_Battle_Enter()
{
	m_bIsActive = TRUE;
	m_iAnimationSequence = 0;
	m_fMoveTime = 0.f;
	m_pBodyModelCom->Set_Animation("MV_Nikke_Scarlet_Entrance_Scarlet_01", FALSE, 1.f, 0.f, TRUE);
	m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(258.315f, 21.822f, 331.387f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(258.315f, 21.822f, 332.387f, 1.f));
	m_pBottle->Set_Render(TRUE);
	m_pGlass->Set_Render(TRUE);
	m_bIsDoFActivated = FALSE;
	m_fDoFTimeAcc = 0.f;
	static_cast<CModel*>(static_cast<CCinematicPartFacial*>(Find_PartObject(TEXT("Part_Face")))->Find_Component(TEXT("Com_Model")))->Set_MorphAnimation("Scarlet_Entrance_Scarlet_Curves", TRUE, 1.f, 0.f, FALSE, -1.f, 19000.f);
	static_cast<CCinematicPartBody*>(Find_PartObject(TEXT("Part_Weapon")))->SetActive(TRUE);
	static_cast<CCinematicPartBody*>(Find_PartObject(TEXT("Part_Scabbard")))->SetActive(TRUE);

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Initialize_Cinematic_Scarlet_Battle_PhaseChange()
{
	m_bIsActive = TRUE;
	m_iAnimationSequence = 0;
	m_fMoveTime = 0.f;
	//m_pBodyModelCom->Set_Animation("MV_Nikke_Scarlet_Phase2_seq_Scarlet_ANI01", FALSE, 3.f, 0.12f, false, -1.f, 30.f);
	m_pBodyModelCom->Set_Animation("MV_Nikke_Scarlet_Phase2_seq_Scarlet_ANI01", FALSE, 3.f, 0.f, TRUE);
	m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(251.874f, 8.f, 234.907f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(252.874f, 8.f, 234.907f, 1.f));
	m_pBottle->Set_Render(FALSE);
	m_pGlass->Set_Render(FALSE);
	m_bIsDoFActivated = FALSE;
	m_fDoFTimeAcc = 0.f;

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
	m_pBottle->Set_Render(FALSE);
	m_pGlass->Set_Render(FALSE);
	m_bIsDoFActivated = FALSE;
	m_fDoFTimeAcc = 0.f;

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Play_Cinematic_Scarlet_FirstMeet(_float fTimeDelta)
{
	m_fMoveTime += fTimeDelta;
	_bool isFinished = Play_Animation(fTimeDelta, m_pTransformCom, 1.f);


	_float fCurrentMorphTrackPosition = m_pFacial->Get_fCurrentMorphTrackPosition();

	if (fCurrentMorphTrackPosition >= 32340
		&& fCurrentMorphTrackPosition < 46159)
	{
		m_pFacial->Set_MorphTrackPosition(46159);
	}
	else if (fCurrentMorphTrackPosition >= 56512
		&& fCurrentMorphTrackPosition < 60213)
	{
		m_pFacial->Set_MorphTrackPosition(60213);
	}
	else if (fCurrentMorphTrackPosition >= 78650
		&& fCurrentMorphTrackPosition < 79917)
	{
		m_pFacial->Set_MorphTrackPosition(79917);
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
			m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(740.967f, 2.f, 647.034f, 1.f));
			m_pTransformCom->LookAt(XMVectorSet(740.967f, 2.f, 645.034f, 1.f));
			m_pBodyModelCom->Set_Animation("MV_Nikke_ScarletVolt_1stMeet_NA_961_02", FALSE, 2.f, 0.12f, FALSE, -1.f, 10.f);
		}
		else if (m_iAnimationSequence == 2)
		{
			m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(734.967f, 2.f, 647.834f, 1.f));
			m_pBodyModelCom->Set_Animation("MV_Nikke_ScarletVolt_1stMeet_NA_961_03", FALSE, 2.f, 0.f, FALSE);
			//m_pTransformCom->LookAt(XMVectorSet(741.467f, 2.f, 636.534f, 1.f));
		}
		else if (m_iAnimationSequence == 3)
		{
			m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(734.967f, 2.f, 647.834f, 1.f));
			m_pBodyModelCom->Set_Animation("MV_Nikke_ScarletVolt_1stMeet_NA_961_04", FALSE, 2.f, 0.f, FALSE);
			//m_pTransformCom->LookAt(XMVectorSet(741.467f, 2.f, 636.534f, 1.f));
		}
	}

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Play_Cinematic_Scarlet_GoodBye(_float fTimeDelta)
{
	m_fMoveTime += fTimeDelta;
	_bool isFinished = Play_Animation(fTimeDelta, m_pTransformCom, 1.f);

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
			m_pBodyModelCom->Set_Animation("MV_Nikke_ScarletVolt_GoodBye_Scarlet_c", FALSE, 2.f, 0.12f, FALSE);
		}

	}

	return S_OK;
}

HRESULT CCinematicModel_Scarlet::Play_Cinematic_Scarlet_Battle_Enter(_float fTimeDelta)
{
	m_fMoveTime += fTimeDelta;
	_bool isFinished = Play_Animation(fTimeDelta, m_pTransformCom, 1.f);


	if (true == m_bIsDoFActivated)
	{
		_float fFocusDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) -
			m_pTransformCom->Get_State(STATE::POSITION)));

		m_pGameInstance->Set_DoFInfo(fFocusDist);

		m_fDoFTimeAcc += fTimeDelta;
	}

	if (m_iAnimationSequence == 0)
	{
		_float fCurrentTrackPosition = m_pBodyModelCom->Get_fTrackPosition();

		if (fCurrentTrackPosition >= 734.f
			&& fCurrentTrackPosition < 820.f
			&& m_bIsDoFActivated == FALSE)
		{
			_float fFocusDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) -
				m_pTransformCom->Get_State(STATE::POSITION)));
			_float fMaxDist = 3.f;
			_float fIntensity = 1.5f;

			m_pGameInstance->Active_DoF(true, 1.f);
			m_pGameInstance->Set_DoFInfo(fFocusDist, fMaxDist, fIntensity);

			m_bIsDoFActivated = TRUE;

		}
		else if (fCurrentTrackPosition >= 850.f
			&& m_bIsDoFActivated == TRUE)
		{
			m_pGameInstance->Active_DoF(false, 1.f);
			m_fDoFTimeAcc = 0.f;
			m_bIsDoFActivated = FALSE;
		}

	}
	else if (m_iAnimationSequence == 1)
	{
		_float fCurrentTrackPosition = m_pBodyModelCom->Get_fTrackPosition();

		if (fCurrentTrackPosition > 1129.f
			&& fCurrentTrackPosition <= 1169.f)
		{
			_float fRatio = (fCurrentTrackPosition - 1129.f) / 40.f;
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
			m_pBottle->Set_Render(FALSE);
			m_pGlass->Set_Render(FALSE);
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
	_float fCurrentMorphTrackPosition = m_pFacial->Get_fCurrentMorphTrackPosition();


	if (m_fMoveTime > 7.16f)
	{
		m_pFacial->Set_MorphAnimation("MV_Nikke_Scarlet_Phase2_Scarlet_Curves", TRUE, 1.f, 0.f, FALSE, -1.f);
	}


	if (m_iAnimationSequence == 3)
	{
		_float fCurrentTrackPosition = m_pBodyModelCom->Get_fTrackPosition();

		if (fCurrentTrackPosition >= 444.f)
		{
			m_pBottle->SetActive(FALSE);
		}
	}

	if (m_iAnimationSequence == 1)
	{
		_float fCurrentTrackPosition = m_pBodyModelCom->Get_fTrackPosition();

		if (fCurrentTrackPosition >= 1234.f)
		{
			m_pBottle->SetActive(TRUE);
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
			m_pBodyModelCom->Set_Animation("MV_Nikke_Scarlet_Phase2_seq_Scarlet_ANI02", FALSE, 3.f, 0.12f, FALSE);
		}
		else if (m_iAnimationSequence == 2)
		{
			m_pBodyModelCom->Set_Animation("MV_Nikke_Scarlet_Phase2_seq_Scarlet_ANI03", FALSE, 3.f, 0.12f, FALSE, 850.f);
		}
		else if (m_iAnimationSequence == 3)
		{
			m_pBodyModelCom->Set_Animation("MV_Nikke_Scarlet_Phase2_seq_Scarlet_ANI04", FALSE, 2.f, 0.12f, FALSE, -1.f, 213.f);
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

	_float fCurrentMorphTrackPosition = m_pFacial->Get_fCurrentMorphTrackPosition();

	if (m_fMoveTime > 20.4f)
	{
		m_pFacial->Set_MorphAnimation("MV_Nikke_Scarlet_QTE_AfterBattle_Scarlet_Curves", TRUE, 1.f, 0.f, FALSE, -1.f);
	}

	if (fCurrentMorphTrackPosition >= 11160
		&& fCurrentMorphTrackPosition < 15000)
	{
		m_pFacial->Set_MorphTrackPosition(15000);
	}
	else if (fCurrentMorphTrackPosition >= 20107
		&& fCurrentMorphTrackPosition < 21770)
	{
		m_pFacial->Set_MorphTrackPosition(21770);
	}
	else if (fCurrentMorphTrackPosition >= 31389
		&& fCurrentMorphTrackPosition < 34823)
	{
		m_pFacial->Set_MorphTrackPosition(34823);
	}
	else if (fCurrentMorphTrackPosition >= 39774
		&& fCurrentMorphTrackPosition < 43425)
	{
		m_pFacial->Set_MorphTrackPosition(43425);
	}

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
			m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(248.529f, 8.f, 235.061f, 1.f));
			m_pTransformCom->LookAt(XMVectorSet(248.529f, 8.f, 234.061f, 1.f));
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
