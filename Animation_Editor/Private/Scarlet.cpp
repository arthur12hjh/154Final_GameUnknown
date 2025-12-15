#include "Scarlet.h"
#include "Scarlet.h"
#include "Scarlet.h"
#include "Scarlet.h"
#include "Scarlet.h"
#include "pch.h"
#include "Scarlet.h"

#include "Body_Scarlet.h"
//#include "Face_Scarlet.h"
//#include "Hair_Scarlet.h"
//#include "Weapon_Scarlet.h"
#include "GameInstance.h"

CScarlet::CScarlet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEntity{ pDevice, pContext }
{
}

CScarlet::CScarlet(const CScarlet& Prototype)
	: CEntity{ Prototype }
{
}

HRESULT CScarlet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CScarlet::Initialize(void* pArg)
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

void CScarlet::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CScarlet::Update(_float fTimeDelta)
{
	//if (GetKeyState(VK_LBUTTON) & 0x8000)
	//{
	//	_float3		vPickPos = {};
	//	if (true == m_pGameInstance->isPicking(&vPickPos))
	//	{
	//		m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPickPos), 1.f));
	//	}
	//}


	//if (GetKeyState(VK_DOWN) & 0x8000)
	//{
	//	m_pTransformCom->Go_Backward(fTimeDelta);
	//}
	//
	//if (GetKeyState(VK_LEFT) & 0x8000)
	//{
	//	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	//}
	//
	//if (GetKeyState(VK_RIGHT) & 0x8000)
	//{
	//	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	//}
	//
	//if (GetKeyState(VK_UP) & 0x8000)
	//{
	//	m_pTransformCom->Go_Straight(fTimeDelta);
	//
	//	if (m_iState & STATE_IDLE)
	//		m_iState ^= STATE_IDLE;
	//
	//	m_iState |= STATE_WALK;
	//}
	//else
	//{
	//	if (m_iState & STATE_ATTACK)
	//		goto Progress;
	//
	//	if (m_iState & STATE_WALK)
	//		m_iState ^= STATE_WALK;
	//
	//	m_iState |= STATE_IDLE;
	//}
	//
	//if (GetKeyState(VK_SPACE) & 0x8000)
	//{
	//	m_iState = STATE::STATE_ATTACK;
	//}

	//	m_pNavigationCom->Compute_Height(m_pTransformCom);

	__super::Update(fTimeDelta);
}

void CScarlet::Late_Update(_float fTimeDelta)
{
	if (m_iState & STATE_ATTACK)
	{
		CBody_Scarlet* pPartBody = static_cast<CBody_Scarlet*>(Find_PartObject(TEXT("Part_Body")));

		if (pPartBody->isFinish_Att())
		{
			m_iState ^= STATE_ATTACK;
		}
	}

	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->ADD_Collider(m_pColliderCom);
#endif
}

HRESULT CScarlet::Render()
{



	return S_OK;
}



void CScarlet::Set_Animation(const _char* szAnimationTag)
{
	static_cast<CModel*>(m_pPart_Body->Find_Component(TEXT("Com_Model")))->Set_Animation(szAnimationTag);
}

void CScarlet::Set_Animation(_uint iAnimationIndex)
{
	static_cast<CModel*>(m_pPart_Body->Find_Component(TEXT("Com_Model")))->Set_AnimationIndex(iAnimationIndex);
}

HRESULT CScarlet::Ready_Components()
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

HRESULT CScarlet::Ready_PartObjects()
{
	CBody_Scarlet::BODY_SCARLET_DESC BodyDesc{};
	BodyDesc.pParentState = &m_iState;
	BodyDesc.pParentTransform = m_pTransformCom;

	/* Part_Body */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Body_Scarlet"),
		TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	m_pPart_Body = dynamic_cast<CBody_Scarlet*>(Find_PartObject(TEXT("Part_Body")));

	//CBody_Scarlet* pPart_Body = dynamic_cast<CBody_Scarlet*>(Find_PartObject(TEXT("Part_Body")));

	//CWeapon::WEAPON_DESC	WeaponDesc{};
	//WeaponDesc.pParentState = &m_iState;
	//WeaponDesc.pSocketMatrix = pPart_Body->Get_BoneMatrixPtr("SWORD");
	//WeaponDesc.pParentTransform = m_pTransformCom;
	//
	///* Part_Weapon */
	//if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Weapon"),
	//	TEXT("Part_Weapon"), &WeaponDesc)))
	//	return E_FAIL;



	return S_OK;
}

CScarlet* CScarlet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CScarlet* pInstance = new CScarlet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CScarlet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CScarlet::Clone(void* pArg)
{
	CScarlet* pInstance = new CScarlet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CScarlet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScarlet::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);

}
