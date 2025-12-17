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

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pColliderCom->SetOwner(this);

	return S_OK;
}

void CCinematicModel_Gorilla::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->Update_PreWorldMatrix();

	__super::Priority_Update(fTimeDelta);
}

void CCinematicModel_Gorilla::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CCinematicModel_Gorilla::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	m_pGameInstance->ADD_Collider(m_pColliderCom);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	__super::Late_Update(fTimeDelta);
}

HRESULT CCinematicModel_Gorilla::Render()
{
	return S_OK;
}

HRESULT CCinematicModel_Gorilla::Ready_Components()
{
	/* Com_Collider_AABB */
	CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};
	AABBDesc.vSize = _float3(1.5f, 2.f, 1.5f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;

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
