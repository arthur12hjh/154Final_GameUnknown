#include "Character.h"
#include "Character.h"
#include "Character.h"
#include "Character.h"
#include "Character.h"
#include "pch.h"
#include "Character.h"

#include "Body_Character.h"
#include "Face_Character.h"
#include "Hair_Character.h"
#include "PonyTail_Character.h"
#include "Weapon.h"
#include "GameInstance.h"

CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEntity{ pDevice, pContext }
{
}

CCharacter::CCharacter(const CCharacter& Prototype)
	: CEntity{ Prototype }
{
}

HRESULT CCharacter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCharacter::Initialize(void* pArg)
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

void CCharacter::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CCharacter::Update(_float fTimeDelta)
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

void CCharacter::Late_Update(_float fTimeDelta)
{
	if (m_iState & STATE_ATTACK)
	{
		CBody_Character* pPartBody = static_cast<CBody_Character*>(Find_PartObject(TEXT("Part_Body")));

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

HRESULT CCharacter::Render()
{



	return S_OK;
}



void CCharacter::Set_Animation(const _char* szAnimationTag)
{
	static_cast<CModel*>(m_pPart_Body->Find_Component(TEXT("Com_Model")))->Set_Animation(szAnimationTag);
}

void CCharacter::Set_Animation(_uint iAnimationIndex)
{
	static_cast<CModel*>(m_pPart_Body->Find_Component(TEXT("Com_Model")))->Set_AnimationIndex(iAnimationIndex);
}

HRESULT CCharacter::Ready_Components()
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

HRESULT CCharacter::Ready_PartObjects()
{
	CBody_Character::BODY_CHARACTER_DESC BodyDesc{};
	BodyDesc.pParentState = &m_iState;
	BodyDesc.pParentTransform = m_pTransformCom;

	/* Part_Body */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Body_Character"),
		TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	m_pPart_Body = dynamic_cast<CBody_Character*>(Find_PartObject(TEXT("Part_Body")));
	
	CFace_Character::FACE_CHARACTER_DESC FaceDesc{};
	FaceDesc.pParentState = &m_iState;
	FaceDesc.pParentTransform = m_pTransformCom;
	FaceDesc.pBodyPtr = m_pPart_Body;
	
	/* Part_Face */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Face_Character"),
		TEXT("Part_Face"), &FaceDesc)))
		return E_FAIL;
	
	CHair_Character::HAIR_CHARACTER_DESC HairDesc{};
	HairDesc.pParentState = &m_iState;
	HairDesc.pParentTransform = m_pTransformCom;
	HairDesc.pBodyPtr = m_pPart_Body;
	
	/* Part_Hair */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Hair_Character"),
		TEXT("Part_Hair"), &HairDesc)))
		return E_FAIL;
	
	CPonyTail_Character::PONYTAIL_CHARACTER_DESC PonyTailDesc{};
	PonyTailDesc.pParentState = &m_iState;
	PonyTailDesc.pParentTransform = m_pTransformCom;
	PonyTailDesc.pBodyPtr = m_pPart_Body;
	
	/* Part_PonyTail */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_PonyTail_Character"),
		TEXT("Part_PonyTail"), &PonyTailDesc)))
		return E_FAIL;

	//CBody_Character* pPart_Body = dynamic_cast<CBody_Character*>(Find_PartObject(TEXT("Part_Body")));

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

CCharacter* CCharacter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCharacter* pInstance = new CCharacter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCharacter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCharacter::Clone(void* pArg)
{
	CCharacter* pInstance = new CCharacter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCharacter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCharacter::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);

}
