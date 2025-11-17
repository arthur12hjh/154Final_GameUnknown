#include "pch.h"
#include "Gigas.h"

#include "Body_Gigas.h"
#include "Weapon.h"
#include "GameInstance.h"

CGigas::CGigas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEntity{ pDevice, pContext }
{
}

CGigas::CGigas(const CGigas& Prototype)
	: CEntity{ Prototype }
{
}

HRESULT CGigas::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGigas::Initialize(void* pArg)
{
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

void CGigas::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CGigas::Update(_float fTimeDelta)
{
Progress:
	__super::Update(fTimeDelta);
}

void CGigas::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->ADD_Collider(m_pColliderCom);
#endif
}

HRESULT CGigas::Render()
{



	return S_OK;
}

void CGigas::Set_Animation(const _char* szAnimationTag)
{
	static_cast<CModel*>(m_pPart_Body->Find_Component(TEXT("Com_Model")))->Set_Animation(szAnimationTag);
}

void CGigas::Set_Animation(_uint iAnimationIndex)
{
	static_cast<CModel*>(m_pPart_Body->Find_Component(TEXT("Com_Model")))->Set_AnimationIndex(iAnimationIndex);
}

HRESULT CGigas::Ready_Components()
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

HRESULT CGigas::Ready_PartObjects()
{
	CBody_Gigas::BODY_GIGAS_DESC BodyDesc{};
	BodyDesc.pParentTransform = m_pTransformCom;

	/* Part_Body */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Body_Gigas"),
		TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	m_pPart_Body = dynamic_cast<CBody_Gigas*>(Find_PartObject(TEXT("Part_Body")));

	return S_OK;
}

CGigas* CGigas::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGigas* pInstance = new CGigas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGigas");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGigas::Clone(void* pArg)
{
	CGigas* pInstance = new CGigas(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGigas");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGigas::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);

}
