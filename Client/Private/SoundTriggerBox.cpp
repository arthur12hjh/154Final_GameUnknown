#include "pch.h"
#include "SoundTriggerBox.h"

#include "GameInstance.h"
#include "Character.h"

CSoundTriggerBox::CSoundTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

CSoundTriggerBox::CSoundTriggerBox(const CSoundTriggerBox& Prototype) :
    CGameObject(Prototype)
{
}

HRESULT CSoundTriggerBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSoundTriggerBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	SOUNDTRIGGER_BOX_DESC* pDesc = static_cast<SOUNDTRIGGER_BOX_DESC*>(pArg);
	if (FAILED(Ready_Components(*pDesc)))
		return E_FAIL;

	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	m_eGroundSoundType = pDesc->eSoundBoxType;
    return S_OK;
}

void CSoundTriggerBox::Priority_Update(_float fTimeDelta)
{
}

void CSoundTriggerBox::Update(_float fTimeDelta)
{
	/*if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_B))
	{
		m_bIsRender = !m_bIsRender;
	}*/
}

void CSoundTriggerBox::Late_Update(_float fTimeDelta)
{
    //if (m_pGameInstance->isIn_WorldFrustum(m_pColliderCom))
        m_pGameInstance->ADD_Collider(m_pColliderCom);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CSoundTriggerBox::Render()
{
	/*if (m_bIsRender == true)
	{
		if (m_eGroundSoundType == GROUND_SOUND_TYPE::IRON)
		{
			COBBCollider* pObbCollider = static_cast<COBBCollider*>(m_pColliderCom);
			pObbCollider->Render_Face(_float4(1.f, 0.f, 0.f, 1.f));
		}
		else if (m_eGroundSoundType == GROUND_SOUND_TYPE::BRIDGE)
		{
			COBBCollider* pObbCollider = static_cast<COBBCollider*>(m_pColliderCom);
			pObbCollider->Render_Face(_float4(0.f, 1.f, 0.f, 1.f));
		}
		else if (m_eGroundSoundType == GROUND_SOUND_TYPE::CONCRETE)
		{
			COBBCollider* pObbCollider = static_cast<COBBCollider*>(m_pColliderCom);
			pObbCollider->Render_Face(_float4(0.f, 0.f, 1.f, 1.f));
		}
		else if (m_eGroundSoundType == GROUND_SOUND_TYPE::ROCK)
		{
			COBBCollider* pObbCollider = static_cast<COBBCollider*>(m_pColliderCom);
			pObbCollider->Render_Face(_float4(1.f, 0.f, 1.f, 1.f));
		}
		else if (m_eGroundSoundType == GROUND_SOUND_TYPE::CONTAINER)
		{
			COBBCollider* pObbCollider = static_cast<COBBCollider*>(m_pColliderCom);
			pObbCollider->Render_Face(_float4(1.f, 1.f, 0.f, 1.f));
		}

	}*/

    return S_OK;
}

HRESULT CSoundTriggerBox::Ready_Components(const SOUNDTRIGGER_BOX_DESC& pDesc)
{
	/*switch (pDesc.eColType)
	{*/
	//case COLLIDER::AABB:
	//{
	//	/* Com_Collider_AABB */
	//	CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};
	//	AABBDesc.vSize = pDesc.vScale;
	//	AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y, 0.f);

	//	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
	//		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
	//		return E_FAIL;
	//}
	//break;
	//case COLLIDER::OBB:
	//{
	//	/* Com_Collider_AABB */
	//	COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};
	//	OBBDesc.vSize = pDesc.vScale;
	//	OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y, 0.f);

	//	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
	//		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
	//		return E_FAIL;
	//}
	//break;
	//case COLLIDER::SPHERE:
	//{
	//	/* Com_Collider_Sphere */
	//	CSphereCollider::SPHERE_COLLIDER_DESC		SphereDesc{};
	//	SphereDesc.fRadius = pDesc.vScale.x;
	//	SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);

	//	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
	//		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
	//		return E_FAIL;
	//}
	//break;
	//}

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

void CSoundTriggerBox::Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
  
}

void CSoundTriggerBox::OverlappingEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	auto pCharacter = dynamic_cast<CCharacter*>(pHitActor);
	if (pCharacter)
	{
		pCharacter->SetGroundSoundType(m_eGroundSoundType);
	}
}

void CSoundTriggerBox::End_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
    auto pCharacter = dynamic_cast<CCharacter*>(pHitActor);
    if (pCharacter)
    {
        pCharacter->SetGroundSoundType(GROUND_SOUND_TYPE::SAND);
    }
}

CSoundTriggerBox* CSoundTriggerBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSoundTriggerBox* SoundTriggerBox = new CSoundTriggerBox(pDevice, pContext);
    if (FAILED(SoundTriggerBox->Initialize_Prototype()))
    {
        Safe_Release(SoundTriggerBox);
        MSG_BOX("Create Fail : Trigger Box");
    }
    return SoundTriggerBox;
}

CGameObject* CSoundTriggerBox::Clone(void* pArg)
{
    CSoundTriggerBox* SoundTriggerBox = new CSoundTriggerBox(*this);
    if (FAILED(SoundTriggerBox->Initialize(pArg)))
    {
        Safe_Release(SoundTriggerBox);
        MSG_BOX("Clone Fail : Trigger Box");
    }
    return SoundTriggerBox;
}

void CSoundTriggerBox::Free()
{
    __super::Free();

	Safe_Release(m_pColliderCom);
}
