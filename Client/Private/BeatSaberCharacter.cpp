#include "pch.h"
#include "BeatSaberCharacter.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "BeatSaberCharacterBody.h"

#include "Note.h"

#pragma region State
#include "BeatSaberFsm.h"
#include "BeatSaber_IdleState.h"
#include "BeatSaber_MoveState.h"
#pragma endregion

CBeatSaberCharacter::CBeatSaberCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CCharacter(pDevice, pContext)
{
}

CBeatSaberCharacter::CBeatSaberCharacter(const CBeatSaberCharacter& Prototype) :
	CCharacter(Prototype)
{
}

HRESULT CBeatSaberCharacter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBeatSaberCharacter::Initialize(void* pArg)
{
	m_pGameManager->Bind_BeatSaberCharacter(this);
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_CharacterData()))
		return E_FAIL;

	if (FAILED(ADD_PartObjects()))
		return E_FAIL;

	if (FAILED(ADD_Components()))
		return E_FAIL;

	auto pOBBCollider = static_cast<COBBCollider*>(m_pColliderCom);
	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	m_BoxSizeZ = pOBBCollider->GetBounding().Extents.z;
	return S_OK;
}

void CBeatSaberCharacter::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CBeatSaberCharacter::Update(_float fTimeDelta)
{
	Key_Input(fTimeDelta);

	m_pFsm->Update(fTimeDelta);


	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	__super::Update(fTimeDelta);

	// 현재 상태와 전 프레임 상태 동기화
	m_CharacterDesc.ePreDirection = m_CharacterDesc.eDirection;
}

void CBeatSaberCharacter::Late_Update(_float fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;
	if (m_bIsActive == TRUE)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	}
	m_pGameInstance->ADD_Collider(m_pColliderCom);
	__super::Late_Update(fTimeDelta);
}

HRESULT CBeatSaberCharacter::Render()
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Render();

	return S_OK;
}

HRESULT CBeatSaberCharacter::Render_Shadow()
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Render_Shadow();

	return S_OK;
}

HRESULT CBeatSaberCharacter::Render_MotionBlur()
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Render_MotionBlur();

	return S_OK;
}

void CBeatSaberCharacter::ResetPoints()
{
	m_CharacterDesc.iScore = 0;
	m_CharacterDesc.iComboCnt = 0;
	m_CharacterDesc.isFullCombo = false;
}

HRESULT CBeatSaberCharacter::Ready_CharacterData()
{
	m_CharacterDesc.iMaxGameLife = 3;
	m_CharacterDesc.iGameLife = m_CharacterDesc.iMaxGameLife;
	m_CharacterDesc.fBoundCharacterSpeed = {3.f, 5.f, 7.f};
	//m_CharacterDesc.fCharacterSpeed = m_CharacterDesc.fBoundCharacterSpeed.y;
	m_CharacterDesc.fCharacterSpeed = 50.f;
	m_CharacterDesc.eDirection = DIRECTION::END;
	m_CharacterDesc.iScore = 0;

	return S_OK;
}

HRESULT CBeatSaberCharacter::ADD_PartObjects()
{
	CBeatSaberCharacterBody::BEATSABER_CHARACTER_DESC BodyDesc = { };
	BodyDesc.pParent = this;
	BodyDesc.pParentTransform = m_pTransformCom;
	BodyDesc.vScale = { 1.f, 1.f, 1.f };
	BodyDesc.fSpeedPerSec = 5.f;
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_GameObject_BeatSaberCharacterBody"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	auto pPartBody = Find_PartObject(TEXT("Part_Body"));
	if (nullptr == pPartBody)
		return E_FAIL;

	m_pPartBody = static_cast<CBeatSaberCharacterBody*>(pPartBody);

	Import_ModelPtr();
	return S_OK;
}

HRESULT CBeatSaberCharacter::ADD_Components()
{
	/* Com_Collider_AABB */
	COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};
	OBBDesc.vSize = { 1.f, 1.f, 1.f };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
		return E_FAIL;

	m_pColliderCom->SetColliderHitType(HIT_TYPE::PLAYER);
	m_pColliderCom->BindOverlappingEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { OverlappingEvent(vHitPoint, vHitDir, pHitActor); });
	m_pColliderCom->BindEndOverlapEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { OverlapEnd(vHitPoint, vHitDir, pHitActor); });

	CBeatSaberFsm::STATEMACHINE_DESC FsmDesc = {};
	FsmDesc.pOwner = this;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_BeatSaberCharacterFsm"),
		TEXT("BeatSaberFsm"), reinterpret_cast<CComponent**>(&m_pFsm), &FsmDesc)))
		return E_FAIL;

#pragma region ADD_State
	CState::STATE_DESC StateDesc = {};
	StateDesc.pOwner = this;

	if(FAILED(m_pFsm->Add_State(TEXT("Idle"), CBeatSaber_IdleState::Create(&StateDesc))))
		return E_FAIL;

	if (FAILED(m_pFsm->Add_State(TEXT("Move"), CBeatSaber_MoveState::Create(&StateDesc))))
		return E_FAIL;

	m_pFsm->Change_State(TEXT("Idle"));
#pragma endregion

	return S_OK;
}

void CBeatSaberCharacter::Key_Input(_float fTimeDelta)
{
	_bool bIsMove = { false };
	if (DIRECTION::END != m_CharacterDesc.eDirection)
		m_CharacterDesc.eDirection = DIRECTION::END;

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A)) // 3
	{
		m_CharacterDesc.eDirection = DIRECTION::RIGHT;
		bIsMove = true;
	}

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S)) // 1
	{
		m_CharacterDesc.eDirection = DIRECTION::LEFT_FRONT;
		bIsMove = true;
	}

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)) // 0
	{
		m_CharacterDesc.eDirection = DIRECTION::LEFT;
		bIsMove = true;
	}

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_J)) // 5
	{
		m_CharacterDesc.eDirection = DIRECTION::RIGHT_BACK;
		bIsMove = true;
	}

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_K)) // 4
	{
		m_CharacterDesc.eDirection = DIRECTION::RIGHT_FRONT;
		bIsMove = true;
	}

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_L)) // 2
	{
		m_CharacterDesc.eDirection = DIRECTION::LEFT_BACK;
		bIsMove = true;
	}

	if(bIsMove)
		m_pFsm->Change_State(TEXT("Move"));
	else
		m_pFsm->Change_State(TEXT("Idle"));
}

void CBeatSaberCharacter::OverlappingEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	auto pNote = dynamic_cast<CNote*>(pHitActor);
	if (pNote)
	{
		pNote->OverlapTime(m_fTimeDelta);
		auto pNoteData = pNote->GetNoteData();

		_bool bIsSuccess = { false };
		if (pNoteData.eDirection == m_CharacterDesc.eDirection)
		{
			_float fAnimFrame = m_pBodyModelCom->Get_AnimationKeyFrameIndex();
			if (pNoteData.vBoundAnimFrame.x <= fAnimFrame && fAnimFrame <= pNoteData.vBoundAnimFrame.y)
			{
				_float fOverlapTime = pNote->GetOverlapTime();
				_vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
				_vector vObjectPosition = pHitActor->GetTransform()->Get_State(STATE::POSITION);

				_float fLength = XMVectorGetX(XMVector3Length(vObjectPosition - vPosition));
				_float fLengthRatio = fLength / m_BoxSizeZ;
				// 여기서 타임 판별해서 체크
				if (1.f > fLength)
				{
					m_CharacterDesc.iScore += 100;
					m_CharacterDesc.iComboCnt++;
					m_CharacterDesc.iAccuracy = 3;
					bIsSuccess = true;
				}
				else if (1.5f > fLength)
				{
					m_CharacterDesc.iScore += 60;
					m_CharacterDesc.iComboCnt++;
					m_CharacterDesc.iAccuracy = 2;
					bIsSuccess = true;
				}
				else if (2.f > fLength)
				{
					m_CharacterDesc.iScore += 30;
					m_CharacterDesc.iComboCnt++;
					m_CharacterDesc.iAccuracy = 1;
					bIsSuccess = true;
				}
				else
				{
					m_CharacterDesc.iComboCnt = 0;
					m_CharacterDesc.iAccuracy = 0;
					bIsSuccess = true;
				}
			}
		}

		if (bIsSuccess)
			pHitActor->Set_Dead(true);
	}

}

void CBeatSaberCharacter::OverlapEnd(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	if (false == pHitActor->isDead())
	{
		m_CharacterDesc.iGameLife--;
		m_CharacterDesc.iComboCnt = 0;
		m_CharacterDesc.iAccuracy = 0;

		if (0 >= m_CharacterDesc.iGameLife)
		{
			// 게임 실패
			// 여기서 UI처리 후 UI 단에서 원래 레벨로 전환
		}
		pHitActor->Set_Dead(true);
	}
}

CBeatSaberCharacter* CBeatSaberCharacter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBeatSaberCharacter* pBeatSaberCharacter = new CBeatSaberCharacter(pDevice, pContext);
	if (FAILED(pBeatSaberCharacter->Initialize_Prototype()))
	{
		Safe_Release(pBeatSaberCharacter);
		MSG_BOX("Create Fail : Beat Saber Character");
	}
	return pBeatSaberCharacter;
}

CGameObject* CBeatSaberCharacter::Clone(void* pArg)
{
	CBeatSaberCharacter* pBeatSaberCharacter = new CBeatSaberCharacter(*this);
	if (FAILED(pBeatSaberCharacter->Initialize(pArg)))
	{
		Safe_Release(pBeatSaberCharacter);
		MSG_BOX("Clone Fail : Beat Saber Character");
	}
	return pBeatSaberCharacter;
}

void CBeatSaberCharacter::Free()
{
	__super::Free();

	m_pGameManager->Bind_BeatSaberCharacter(nullptr);
	Safe_Release(m_pFsm);
}
