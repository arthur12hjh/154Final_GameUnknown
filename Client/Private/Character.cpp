#include "pch.h"
#include "Character.h"
#include "GameManager.h"

#include "Notify.h"

#include "GameInstance.h"

CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CEntity(pDevice, pContext),
	m_pGameManager(CGameManager::GetInstance())
{
	Safe_AddRef(m_pGameManager);
}

CCharacter::CCharacter(const CCharacter& Prototype) :
	CEntity(Prototype),
	m_pGameManager(CGameManager::GetInstance())
{
	Safe_AddRef(m_pGameManager);
}

HRESULT CCharacter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCharacter::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/* Com_Notify */
	CNotify::NOTIFY_DESC NotifyDesc{};
	NotifyDesc.pCharacter = this;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Notify"),
		TEXT("Com_Notify"), reinterpret_cast<CComponent**>(&m_pNotifyCom), &NotifyDesc)))
		return E_FAIL;

	return S_OK;
}

void CCharacter::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CCharacter::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_pNotifyCom->Update(fTimeDelta);
}

void CCharacter::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CCharacter::Render()
{
	return S_OK;
}

void CCharacter::SocketAttachment(const _float4x4* pAttachedMatrix)
{
	m_pSocketMatrix = pAttachedMatrix;
}

const _bool CCharacter::GetActionEnable()
{
	return m_bIsActionEnabled;
}

void CCharacter::DetachedMatrix()
{
	m_pSocketMatrix = nullptr;
}

void CCharacter::Set_Position(_vector vPosition)
{
	vPosition.m128_f32[3] = 1.f;

	m_pTransformCom->Set_State(STATE::POSITION, vPosition);
}

_vector CCharacter::Get_Position()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

const _float4x4* CCharacter::Get_WorldMatrixPtr()
{
	return m_pTransformCom->Get_WorldMatrixPtr();
}

void CCharacter::SetActionEnable(_bool bFlag)
{
	m_bIsActionEnabled = bFlag;
}

CGameObject* CCharacter::Clone(void* pArg)
{
	return nullptr;
}

void CCharacter::Free()
{
	__super::Free();

	Safe_Release(m_pCCT);
	Safe_Release(m_pGameManager);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNotifyCom);
}