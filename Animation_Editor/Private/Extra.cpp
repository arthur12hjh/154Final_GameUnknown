#include "pch.h"
#include "Extra.h"

#include "Body_Extra.h"
#include "Facial_Extra.h"
#include "Weapon.h"
#include "GameInstance.h"

CExtra::CExtra(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEntity{ pDevice, pContext }
{
}

CExtra::CExtra(const CExtra& Prototype)
	: CEntity{ Prototype }
{
}

HRESULT CExtra::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CExtra::Initialize(void* pArg)
{
	EXTRA_DESC* pDesc = static_cast<EXTRA_DESC*>(pArg);
	m_szModelTag = pDesc->szModelTag;
	m_bIsFacial = pDesc->isFacial;

	CGameObject::GAMEOBJECT_DESC	Desc{};
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fSpeedPerSec = 10.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CExtra::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CExtra::Update(_float fTimeDelta)
{
Progress:
	__super::Update(fTimeDelta);
}

void CExtra::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->ADD_Collider(m_pColliderCom);
#endif
}

HRESULT CExtra::Render()
{



	return S_OK;
}

void CExtra::Set_Animation(const _char* szAnimationTag)
{
	static_cast<CModel*>(m_pPart_Body->Find_Component(TEXT("Com_Model")))->Set_Animation(szAnimationTag);
}

void CExtra::Set_Animation(_uint iAnimationIndex)
{
	static_cast<CModel*>(m_pPart_Body->Find_Component(TEXT("Com_Model")))->Set_AnimationIndex(iAnimationIndex);
}

HRESULT CExtra::Ready_Components()
{
	/* Com_Collider_AABB */
	CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};

	AABBDesc.vSize = _float3(0.8f, 1.3f, 0.8f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CExtra::Ready_PartObjects()
{
	if (m_bIsFacial == FALSE)
	{
		CBody_Extra::BODY_EXTRA_DESC BodyDesc{};
		BodyDesc.pParentTransform = m_pTransformCom;
		BodyDesc.szModelTag = m_szModelTag;

		/* Part_Body */
		if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Body_Extra"),
			TEXT("Part_Body"), &BodyDesc)))
			return E_FAIL;
	

		m_pPart_Body = dynamic_cast<CBody_Extra*>(Find_PartObject(TEXT("Part_Body")));
	}
	else
	{
		CFacial_Extra::FACIAL_EXTRA_DESC FacialDesc{};
		FacialDesc.pParentTransform = m_pTransformCom;
		FacialDesc.szModelTag = m_szModelTag;

		/* Part_Body */
		if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Facial_Extra"),
			TEXT("Part_Body"), &FacialDesc)))
			return E_FAIL;


		m_pPart_Body = nullptr;
	}

	
	return S_OK;
}

CExtra* CExtra::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CExtra* pInstance = new CExtra(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CExtra");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CExtra::Clone(void* pArg)
{
	CExtra* pInstance = new CExtra(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CExtra");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExtra::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);

}
