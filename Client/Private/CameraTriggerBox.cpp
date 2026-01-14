#include "pch.h"
#include "CameraTriggerBox.h"

#include "GameInstance.h"
#include "Character.h"
#include "Camera_Player.h"

CCameraTriggerBox::CCameraTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CGameObject(pDevice, pContext)
{
}

CCameraTriggerBox::CCameraTriggerBox(const CCameraTriggerBox& Prototype) :
	CGameObject(Prototype)
{
}

HRESULT CCameraTriggerBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCameraTriggerBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CAMERA_TRIGGER_BOX_DESC* pDesc = static_cast<CAMERA_TRIGGER_BOX_DESC*>(pArg);
	if (FAILED(Ready_Components(*pDesc)))
		return E_FAIL;

	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	m_pCamera = static_cast<CCamera_Player*>(m_pGameInstance->GetMainCamera());

	return S_OK;
}

void CCameraTriggerBox::Priority_Update(_float fTimeDelta)
{
}

void CCameraTriggerBox::Update(_float fTimeDelta)
{
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_L))
	{
		m_bIsRender = !m_bIsRender;
	}
}

void CCameraTriggerBox::Late_Update(_float fTimeDelta)
{
	//if (m_pGameInstance->isIn_WorldFrustum(m_pColliderCom))
	m_pGameInstance->ADD_Collider(m_pColliderCom);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CCameraTriggerBox::Render()
{
	if (m_bIsRender == true)
	{
		COBBCollider* pObbCollider = static_cast<COBBCollider*>(m_pColliderCom);
		pObbCollider->Render_Face(_float4(1.f, 1.f, 0.f, 0.f));
	}

	return S_OK;
}

HRESULT CCameraTriggerBox::Ready_Components(const CAMERA_TRIGGER_BOX_DESC& pDesc)
{
	/* Com_Collider_OBB */
	COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};
	OBBDesc.vSize = _float3(1.f, 1.f, 1.f);
	OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
		return E_FAIL;

	// 충돌 끝낫을때 이벤트는 사용해야하는 때가 오면 그때 만들게요
	m_pColliderCom->BindOverlappingEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { OverlappingEvent(vHitPoint, vHitDir, pHitActor); });
	m_pColliderCom->BindEndOverlapEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { End_OverlapEvent(vHitPoint, vHitDir, pHitActor); });

	m_pColliderCom->SetColliderHitType(HIT_TYPE::STATIC);

	// 일단은 플레이어만 충돌처리 피직스 오브젝트 들어가면 그녀석들 충돌처리
	// 공통으로 할수있는 Tag 만들어서 하기
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::STATIC);
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::INTERACTION);
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::SENCE);
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::OBJECT);
	static_cast<COBBCollider*>(m_pCullingCollider)->SetCollision(_float3(0.f, pDesc.vScale.y, 0.f), {}, pDesc.vScale);
	return S_OK;
}

void CCameraTriggerBox::Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{

}

void CCameraTriggerBox::OverlappingEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	auto pCharacter = dynamic_cast<CCharacter*>(pHitActor);
	if (pCharacter)
	{
		if (m_pCamera)
		{
			m_pCamera->Set_Distance(6.f);
		}
	}

}

void CCameraTriggerBox::End_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	auto pCharacter = dynamic_cast<CCharacter*>(pHitActor);
	if (pCharacter)
	{
		if (m_pCamera)
		{
			m_pCamera->Set_Distance(13.f);
		}
	}
}

CCameraTriggerBox* CCameraTriggerBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCameraTriggerBox* CameraTriggerBox = new CCameraTriggerBox(pDevice, pContext);
	if (FAILED(CameraTriggerBox->Initialize_Prototype()))
	{
		Safe_Release(CameraTriggerBox);
		MSG_BOX("Create Fail : Trigger Box");
	}
	return CameraTriggerBox;
}

CGameObject* CCameraTriggerBox::Clone(void* pArg)
{
	CCameraTriggerBox* CameraTriggerBox = new CCameraTriggerBox(*this);
	if (FAILED(CameraTriggerBox->Initialize(pArg)))
	{
		Safe_Release(CameraTriggerBox);
		MSG_BOX("Clone Fail : Trigger Box");
	}
	return CameraTriggerBox;
}

void CCameraTriggerBox::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pCamera);
}
