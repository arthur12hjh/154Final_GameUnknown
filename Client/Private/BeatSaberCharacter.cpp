#include "pch.h"
#include "BeatSaberCharacter.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "BeatSaberCharacterBody.h"

#include "Note.h"

#include "Effect.h"

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

	m_pEffect[0]->Stop();
	m_pEffect[1]->Stop();
	m_pEffect[2]->Stop();
	m_pEffect[3]->Stop();
	m_pEffect[4]->Stop();
	m_pEffect[5]->Stop();
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

	CEffect::EFFECT_TRANSFORM_DESC EffectDesc;
	EffectDesc.fRotationPerSec = 1.f;
	EffectDesc.fSpeedPerSec = 1.f;
	EffectDesc.pRootMatrix = nullptr;
	EffectDesc.pWorldMatrix = nullptr;

	EffectDesc.fRot = _float3(0, 0, 0);
	EffectDesc.fSize = 0.09f;

	EffectDesc.vPos = XMVectorSet(5.6f, 3.5f, -3.2f, 1.f);
	m_pEffect[0] = static_cast<CEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Dororong_Fire"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc));

	m_pEffect[2] = static_cast<CEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Dororong_Fire_Good"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc));

	m_pEffect[4] = static_cast<CEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Dororong_Fire_Perfect"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc));

	EffectDesc.vPos = XMVectorSet(-2.65f, 3.5f, 5.15f, 1.f);
	m_pEffect[1] = static_cast<CEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Dororong_Fire"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc));

	m_pEffect[3] = static_cast<CEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Dororong_Fire_Good"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc));

	m_pEffect[5] = static_cast<CEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Dororong_Fire_Perfect"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc));

	m_pEffect[0]->Stop();
	m_pEffect[1]->Stop();
	m_pEffect[2]->Stop();
	m_pEffect[3]->Stop();
	m_pEffect[4]->Stop();
	m_pEffect[5]->Stop();
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


				m_pGameInstance->Manager_PlaySound(TEXT("tamp.wav"), CHANNELID::EFFECT, 1.f, 1.f);
				CEffect::EFFECT_TRANSFORM_DESC EffectDesc;
				EffectDesc.fRotationPerSec = 1.f;
				EffectDesc.fSpeedPerSec = 1.f;
				EffectDesc.pRootMatrix = nullptr;
				EffectDesc.pWorldMatrix = nullptr;

				EffectDesc.vPos = -m_pTransformCom->Get_State(STATE::LOOK) * m_pGameInstance->Random(7.f, 10.f) + m_pTransformCom->Get_State(STATE::RIGHT) * m_pGameInstance->Random(-7.f, 7.f) + m_pTransformCom->Get_State(STATE::UP) * m_pGameInstance->Random(9.f, 13.f);
				EffectDesc.fRot = _float3(0, 0, 0);
				EffectDesc.fSize = m_pGameInstance->Random(0.3f, 0.5f);
				EffectDesc.fSpeed = 1.5f;
				m_pGameInstance->Active_RadialBlur(0.1f, 5, 0.1f);
				_float fRand = m_pGameInstance->Random(0.f, 4.f);
				if (3.f < fRand) {
					m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Dororong_Firecracker"),
						ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc);
				}
				else if (2.f < fRand) {
					m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Dororong_Bikini"),
						ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc);
				}
				else if (1.f < fRand) {
					m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Dororong_Pain"),
						ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc);
				}
				else {
					m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Dororong_Gosu"),
						ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc);
				}
				if (0 == m_CharacterDesc.iComboCnt % 5) {
					EffectDesc.fSpeed = 1.f;
					EffectDesc.fSize = 0.04f;
					_float3 fDir;
					XMStoreFloat3(&fDir, -m_pTransformCom->Get_State(STATE::LOOK));
					EffectDesc.pDir = &fDir;
					if (5 == m_CharacterDesc.iComboCnt) {
						EffectDesc.vPos = m_pTransformCom->Get_State(STATE::UP) * 9 + m_pTransformCom->Get_State(STATE::LOOK) * 2;
						m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Dororong_Good"),
							ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc);
					}
					else {
						EffectDesc.vPos = m_pTransformCom->Get_State(STATE::UP) * 8 + m_pTransformCom->Get_State(STATE::LOOK) * 2;
						m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Dororong_Perfect"),
							ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc);
					}
				}
				if (15 < m_CharacterDesc.iComboCnt) {
					m_pEffect[0]->Stop();
					m_pEffect[1]->Stop();
					m_pEffect[2]->Stop();
					m_pEffect[3]->Stop();
					m_pEffect[4]->Play();
					m_pEffect[5]->Play();
				}
				else if (5 < m_CharacterDesc.iComboCnt) {
					m_pEffect[0]->Stop();
					m_pEffect[1]->Stop();
					m_pEffect[2]->Play();
					m_pEffect[3]->Play();
					m_pEffect[4]->Stop();
					m_pEffect[5]->Stop();
				}
				else if (0 < m_CharacterDesc.iComboCnt) {
					m_pEffect[0]->Play();
					m_pEffect[1]->Play();
					m_pEffect[2]->Stop();
					m_pEffect[3]->Stop();
					m_pEffect[4]->Stop();
					m_pEffect[5]->Stop();
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
		m_pEffect[0]->Play();
		m_pEffect[1]->Play();
		m_pEffect[2]->Stop();
		m_pEffect[3]->Stop();
		m_pEffect[4]->Stop();
		m_pEffect[5]->Stop();
		
		
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
