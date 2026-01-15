#include "pch.h"
#include "MusicTriggerBox.h"

#include "GameInstance.h"
#include "Character.h"
#include "Camera_Player.h"

CMusicTriggerBox::CMusicTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CGameObject(pDevice, pContext)
{
}

CMusicTriggerBox::CMusicTriggerBox(const CMusicTriggerBox& Prototype) :
	CGameObject(Prototype)
{
}

HRESULT CMusicTriggerBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMusicTriggerBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MUSIC_TRIGGER_BOX_DESC* pDesc = static_cast<MUSIC_TRIGGER_BOX_DESC*>(pArg);
	if (FAILED(Ready_Components(*pDesc)))
		return E_FAIL;

	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	return S_OK;
}

void CMusicTriggerBox::Priority_Update(_float fTimeDelta)
{
}

void CMusicTriggerBox::Update(_float fTimeDelta)
{
	/*if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_B))
	{
		m_bIsRender = !m_bIsRender;
	}*/

	if (m_bIsFadeOut)
	{
		_float fCurrentVol = 0.f;
		m_fFadeOutTimer += fTimeDelta;
		
		_float fFadeVolume = 3.f;
		fFadeVolume = Lerp<_float>(fFadeVolume, 0.f, fTimeDelta * 0.5f);

		m_pGameInstance->Manager_SetChannelVolume(CHANNELID::BGM, 0.f, true);
		if (m_fFadeOutTimer >= 2.5f || fFadeVolume <= 0.05f)
		{
			m_pGameInstance->Manager_StopSound(CHANNELID::BGM);

			m_pGameInstance->Manager_PlayBGM(m_szNextBGMKey, 0.01f);
			m_pGameInstance->Manager_SetChannelVolume(CHANNELID::BGM, m_fNextBGMVolume, true);

			m_bIsFadeOut = false;
			m_fFadeOutTimer = 0.f;
			fFadeVolume = 3.f;
		}
	}
}

void CMusicTriggerBox::Late_Update(_float fTimeDelta)
{
	//if (m_pGameInstance->isIn_WorldFrustum(m_pColliderCom))
	m_pGameInstance->ADD_Collider(m_pColliderCom);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CMusicTriggerBox::Render()
{
	/*if (m_bIsRender == true)
	{
		COBBCollider* pObbCollider = static_cast<COBBCollider*>(m_pColliderCom);
		pObbCollider->Render_Face(_float4(1.f, 1.f, 0.f, 0.f));
	}*/

	return S_OK;
}

HRESULT CMusicTriggerBox::Ready_Components(const MUSIC_TRIGGER_BOX_DESC& pDesc)
{
	/* Com_Collider_OBB */
	COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};
	OBBDesc.vSize = _float3(1.f, 1.f, 1.f);
	OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
		return E_FAIL;

	// 충돌 끝낫을때 이벤트는 사용해야하는 때가 오면 그때 만들게요
	m_pColliderCom->BindOverlappingEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { OverlappingEvent(vHitPoint, vHitDir, pHitActor); });
	m_pColliderCom->BindEndOverlapEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { End_OverlapEvent(vHitPoint, vHitDir, pHitActor); });

	m_pColliderCom->SetColliderHitType(HIT_TYPE::STATIC);

	// 일단은 플레이어만 충돌처리 피직스 오브젝트 들어가면 그녀석들 충돌처리
	// 공통으로 할수있는 Tag 만들어서 하기
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::STATIC);
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::INTERACTION);
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::SENCE);
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::OBJECT);
	static_cast<COBBCollider*>(m_pCullingCollider)->SetCollision(_float3(0.f, pDesc.vScale.y, 0.f), {}, pDesc.vScale);
	return S_OK;
}

void CMusicTriggerBox::Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	
}

void CMusicTriggerBox::OverlappingEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	if (m_bIsSound)
		return;
	else
	{
		auto pCharacter = dynamic_cast<CCharacter*>(pHitActor);
		if (pCharacter)
		{
			m_bIsFadeOut = true;
			lstrcpy(m_szNextBGMKey, TEXT("Can_Can.mp3"));
			m_fNextBGMVolume = 3.f;

			m_bIsSound = true;
		}
	}

}

void CMusicTriggerBox::End_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	auto pCharacter = dynamic_cast<CCharacter*>(pHitActor);
	if (pCharacter)
	{
		m_bIsFadeOut = true;
		lstrcpy(m_szNextBGMKey, TEXT("BGM_WASTELAND_UNDISCOVER_LOOP_100_C.wav"));
		m_fNextBGMVolume = 3.f;

		m_bIsSound = false;
	}
}

CMusicTriggerBox* CMusicTriggerBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMusicTriggerBox* MusicTriggerBox = new CMusicTriggerBox(pDevice, pContext);
	if (FAILED(MusicTriggerBox->Initialize_Prototype()))
	{
		Safe_Release(MusicTriggerBox);
		MSG_BOX("Create Fail : Trigger Box");
	}
	return MusicTriggerBox;
}

CGameObject* CMusicTriggerBox::Clone(void* pArg)
{
	CMusicTriggerBox* MusicTriggerBox = new CMusicTriggerBox(*this);
	if (FAILED(MusicTriggerBox->Initialize(pArg)))
	{
		Safe_Release(MusicTriggerBox);
		MSG_BOX("Clone Fail : Trigger Box");
	}
	return MusicTriggerBox;
}

void CMusicTriggerBox::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
