#include "GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject& Prototype)
	: m_pDevice{ Prototype.m_pDevice }
	, m_pContext{ Prototype.m_pContext }
	, m_pGameInstance{ Prototype.m_pGameInstance }
	, m_isDead { Prototype.m_isDead }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{	

	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Initialize(pArg)))
		return E_FAIL;

	m_pCullingCollider = COBBCollider::Create(m_pDevice, m_pContext);
	if (nullptr == m_pCullingCollider)
		return E_FAIL;

	COBBCollider::OBB_COLLIDER_DESC CollisionDesc = {};
	CollisionDesc.vSize = { 1.f, 1.f, 1.f };
	if (FAILED(m_pCullingCollider->Initialize(&CollisionDesc)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
		m_pParent = pDesc->pParent;

		if (pDesc->bIsApplyTransform)
		{
			m_pTransformCom->Set_Scale(XMLoadFloat3(&pDesc->vScale));
			m_pTransformCom->Set_Rotation(XMLoadFloat4(&pDesc->vRotation), pDesc->bIsQuaternion);
			m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&pDesc->vPosition));
		}
	}

	m_Components.emplace(g_strTransformTag, m_pTransformCom);

	Safe_AddRef(m_pTransformCom);

	return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{
}

void CGameObject::Update(_float fTimeDelta)
{
}

void CGameObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

CComponent* CGameObject::Find_Component(const _wstring& strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);
	if(iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Set_Team(OBJECT_TEAM eTeam)
{
	m_eTeam = eTeam;
}

void CGameObject::SetParent(CGameObject* pParent)
{
	m_pParent = pParent;
}

CGameObject* CGameObject::GetParent()
{
	return m_pParent;
}

_bool CGameObject::IsFrustomCulling()
{
	return m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider);
}

_float CGameObject::Get_Depth()
{
	_vector vCamPos = XMLoadFloat4(m_pGameInstance->Get_CamPosition());
	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);

	return XMVectorGetX(XMVector3Length(vCamPos - vPos));
}

void CGameObject::Set_Occlusion_Culling_Result(_bool isVisible)
{
	/*if (m_eVisibility == VISIBILITY::VISIBLE)
	{
		if (!isVisible)
			m_eVisibility = VISIBILITY::HIDDEN;
	}
	else if (m_eVisibility == VISIBILITY::HIDDEN)
	{
		if (isVisible)
			m_eVisibility = VISIBILITY::VISIBLE;
	}*/

	m_eVisibility = isVisible ? VISIBILITY::VISIBLE : VISIBILITY::HIDDEN;
}

HRESULT CGameObject::Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg)
{
	if (nullptr != Find_Component(strComponentTag))
		return E_FAIL;

	CComponent*			pComponent = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);
	pComponent->SetOwner(this);
	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	return S_OK;
}

void CGameObject::Free()
{
	__super::Free();

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCullingCollider);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
