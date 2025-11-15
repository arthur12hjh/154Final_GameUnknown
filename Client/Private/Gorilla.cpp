#include "pch.h"
#include "Gorilla.h"

#include "GameInstance.h"
#include "Gorilla_Body.h"
#include "GorillaBehaviorTree.h"

CGorilla::CGorilla(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEntity{ pDevice, pContext }
{
}

CGorilla::CGorilla(const CGorilla& Prototype) 
	: CEntity{ Prototype }
{
}

HRESULT CGorilla::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGorilla::Initialize(void* pArg)
{		
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
		m_pGameInstance->Random(0.f, 10.f), 
		3.f,
		m_pGameInstance->Random(0.f, 10.f),
		1.f
	));

	static_cast<COBBCollider*>(m_pCullingCollider)->SetCollision({}, {}, { 5.f, 8.f, 5.f });
	/*for (size_t i = 0; i < ENUM_CLASS(COLLIDER::END); i++)
		m_pColliderCom[i]->SetOwner(this);*/

	return S_OK;
}

void CGorilla::Priority_Update(_float fTimeDelta)
{
	m_pBody->Priority_Update(fTimeDelta);
}

void CGorilla::Update(_float fTimeDelta)
{
	/*if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F12))
	{
		m_iIndex++;
		if (m_iMaxIndex <= m_iIndex)
			m_iIndex = 0;

		m_pModelCom->Set_AnimationIndex(m_iIndex);
	}
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F11))
	{
		m_iIndex--;
		if (0 > m_iIndex)
			m_iIndex = m_iMaxIndex - 1;

		m_pModelCom->Set_AnimationIndex(m_iIndex);
	}*/
	m_pBody->Update(fTimeDelta);
	m_pBehaviorTree->Update(fTimeDelta);

	//m_pBody->Update(fTimeDelta);

	/*for (size_t i = 0; i < ENUM_CLASS(COLLIDER::END); i++)
	{
		m_pColliderCom[i]->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	}*/
	m_pCullingCollider->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	
}

void CGorilla::Late_Update(_float fTimeDelta)
{
	CCollider* pTargetCollider = { nullptr };
	pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Get_PartObject_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player"), TEXT("Part_Weapon"), TEXT("Com_Collider_OBB")));

	if (true == m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
	{
		m_pBody->Late_Update(fTimeDelta);
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
#ifdef _DEBUG
		/*for (size_t i = 0; i < ENUM_CLASS(COLLIDER::END); i++)
		{
			m_pGameInstance->ADD_Collider(m_pColliderCom[i]);
			m_pGameInstance->Add_DebugComponent(m_pColliderCom[i]);
		}*/
		m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
	}
	
}

HRESULT CGorilla::Render()
{
	

	return S_OK;
}

HRESULT CGorilla::Ready_Components()
{
	/* Part_Model */
	CPartObject::PARTOBJECT_DESC BodyDesc{};
	BodyDesc.pParentTransform = m_pTransformCom;

	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Gorilla_Body"),
		TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	m_pBody = static_cast<CGorilla_Body*>(Find_PartObject(TEXT("Part_Body")));

	m_pBehaviorTree = CGorillaBehaviorTree::Create(m_pDevice, m_pContext);
	if (nullptr == m_pBehaviorTree)
		return E_FAIL;
	m_pBehaviorTree->SetOwner(this);
	m_pBehaviorTree->Initialize(nullptr);

	return S_OK;
}

HRESULT CGorilla::Bind_ShaderResources()
{
	

	return S_OK;
}

CGorilla* CGorilla::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGorilla* pInstance = new CGorilla(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGorilla::Clone(void* pArg)
{
	CGorilla* pInstance = new CGorilla(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGorilla::Free()
{
	__super::Free();

	for (auto& pCollider : m_pColliderCom)
		Safe_Release(pCollider);

	Safe_Release(m_pBehaviorTree);
}
