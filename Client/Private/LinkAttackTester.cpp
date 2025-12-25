#include "pch.h"
#include "LinkAttackTester.h"

#include "Body_LinkAttackTester.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"

CLinkAttackTester::CLinkAttackTester(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEntity{ pDevice, pContext }
{
}

CLinkAttackTester::CLinkAttackTester(const CLinkAttackTester& Prototype)
	: CEntity{ Prototype }
	, m_pGameManager{ CGameManager::GetInstance() }
{
}

HRESULT CLinkAttackTester::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLinkAttackTester::Initialize(void* pArg)
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

	m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(760.197f, 46.912f, 700.319f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(741.9f, 46.912f, 637.93f, 1.f));
	m_pBodyModelCom = static_cast<CModel*>(m_pPart_Body->Find_Component(TEXT("Com_Model")));

	m_pBodyModelCom->Set_Animation("M_Gorilla_S12_Crush", FALSE, 1.f, 0.12f, TRUE, 37.f, 0.f, TRUE, TRUE);

	m_pPlayer = m_pGameManager->GetGameCharacter();

	return S_OK;
}

void CLinkAttackTester::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CLinkAttackTester::Update(_float fTimeDelta)
{
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_Z))
	{
		m_bIsActive = TRUE;
		m_iAnimationSequence = 0;
		m_fMoveTime = 0.f;
		m_pBodyModelCom->Set_Animation("M_Gorilla_S12_Crush", FALSE, 1.f, 0.12f, TRUE, 37.f, 0.f, TRUE, TRUE);
		m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(760.197f, 46.912f, 700.319f, 1.f));      
	}

	if (m_bIsActive)
	{
		m_fMoveTime += fTimeDelta;
		_bool isFinished = Play_Animation(fTimeDelta, m_pTransformCom, 1.f);

		if (m_fMoveTime > 1.2f && m_fMoveTime < 2.8f)
		{
			_float fRatio = (m_fMoveTime - 1.2f) / 1.6f;
			_vector vPosition = XMVectorLerp(XMVectorSet(760.197f, 0.f, 700.319f, 1.f), XMVectorSet(741.9f, 0.f, 637.93f, 1.f), fRatio);
			_float fHighestPoint = 60.f;
			_float fTime = m_fMoveTime - 1.2f;

			_float fPositionY = Lerp(46.912f, 0.5f, fRatio) + 4.f * fHighestPoint * fRatio * (1.f - fRatio);

			vPosition = XMVectorSetY(vPosition, fPositionY);

			m_pTransformCom->Set_State(STATE::POSITION, vPosition);
		}

		if (isFinished)
		{
			++m_iAnimationSequence;
			if (m_iAnimationSequence == 3)
			{
				m_bIsActive = FALSE;
			}
			else if (m_iAnimationSequence == 1)
			{
				m_pBodyModelCom->Set_Animation("MV_Quest_Sub_033_Gorilla_NA05_01", FALSE, 1.f, 0.12f, FALSE);
			}
			else if (m_iAnimationSequence == 2)
			{
				m_pBodyModelCom->Set_Animation("M_Gorilla_S20_ParryMode", FALSE, 1.f, 0.12f, FALSE, -1.f, 34.f, TRUE, TRUE);
				//m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(741.f, 2.94f, 637.f, 1.f));
			}
		}
	}


	__super::Update(fTimeDelta);
}

void CLinkAttackTester::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	if (m_bIsActive)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, m_pPart_Body);
	}


#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->ADD_Collider(m_pColliderCom);
#endif
}

HRESULT CLinkAttackTester::Render()
{



	return S_OK;
}

void CLinkAttackTester::Set_Animation(const _char* szAnimationTag)
{
	static_cast<CModel*>(m_pPart_Body->Find_Component(TEXT("Com_Model")))->Set_Animation(szAnimationTag);
}

void CLinkAttackTester::Set_Animation(_uint iAnimationIndex)
{
	static_cast<CModel*>(m_pPart_Body->Find_Component(TEXT("Com_Model")))->Set_AnimationIndex(iAnimationIndex);
}

HRESULT CLinkAttackTester::Ready_Components()
{
	/* Com_Collider_AABB */
	CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};

	AABBDesc.vSize = _float3(0.8f, 1.3f, 0.8f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLinkAttackTester::Ready_PartObjects()
{
	CBody_LinkAttackTester::BODY_LINKATTACKTESTER_DESC BodyDesc{};
	BodyDesc.pParentTransform = m_pTransformCom;

	/* Part_Body */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_LinkAttackTester"),
		TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	m_pPart_Body = dynamic_cast<CBody_LinkAttackTester*>(Find_PartObject(TEXT("Part_Body")));

	return S_OK;
}

CLinkAttackTester* CLinkAttackTester::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLinkAttackTester* pInstance = new CLinkAttackTester(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLinkAttackTester");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLinkAttackTester::Clone(void* pArg)
{
	CLinkAttackTester* pInstance = new CLinkAttackTester(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLinkAttackTester");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLinkAttackTester::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pPlayer);

}
