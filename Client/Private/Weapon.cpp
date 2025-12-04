#include "pch.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"
#include "Weapon.h"
#include "Nayitba.h"

#include "Trail.h"
#include "TrailEffect.h"
#include "Effect.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Parts{ pDevice, pContext }
{
}

CWeapon::CWeapon(const CWeapon& Prototype)
	: CPlayer_Parts{ Prototype }
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_bIsTrail = FALSE;
	m_vRotationQuaternion = { -90.f, 0.f, 0.f };

	//m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotationQuaternion.x), XMConvertToRadians(m_vRotationQuaternion.y), XMConvertToRadians(m_vRotationQuaternion.z));
	//m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.2f, 0.1f, -0.1f, 1.f));


	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::NORMAL, "g_NormalTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::EMISSIVE, "g_EmissiveTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORM, "g_ORMTexture");

	m_pPlayerDesc = m_pGameManager->Get_PlayerDesc();

	return S_OK;
}

void CWeapon::Priority_Update(_float fTimeDelta)
{

}

void CWeapon::Update(_float fTimeDelta)
{
	m_pSpark->Update(fTimeDelta);
	m_pCharge->Update(fTimeDelta);


	//ANIM_NOTIFY GaraNotify;
	//GaraNotify.fNumData01 = 1.f;
	//
	//if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, 0))
	//	Active_SFX(TEXT("Trail"), GaraNotify);
	//if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, 0))
	//	Active_SFX(TEXT("Spark"), GaraNotify);
	//if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2))
	//	Active_SFX(TEXT("Charge"), GaraNotify);

	//if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_P))
	//	m_pSpark->Stop();
	//if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_O))
	//	m_pSpark->Play();
	//if (nullptr != m_pCharge) {
	//	if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::RBUTTON)))
	//		m_pCharge->Play();
	//	if (m_pGameInstance->KeyUp(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::RBUTTON)))
	//		m_pCharge->End();
	//}
	//else if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::RBUTTON))) {
	//	CEffect::EFFECT_TRANSFORM_DESC desc;
	//	desc.fRotationPerSec = 1.f;
	//	desc.fSpeedPerSec = 1.f;
	//	desc.pRootMatrix = &m_CombinedWorldMatrix;
	//	desc.vPos = XMVectorSet(-0.06f, -0.2f, 0, 1);
	//	desc.fRot = _float3(0, 0, 0);
	//	desc.fSize = 0.3f;
	//
	//	m_pCharge = static_cast<CEffect*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_SwordCharge"), &desc));
	//	m_pCharge->Play(4.f);
	//}
	//if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_P))
	//	m_vRotationQuaternion.x += fTimeDelta * 30.f;
	//if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_O))
	//	m_vRotationQuaternion.x -= fTimeDelta * 30.f;
	//
	//if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_L))
	//	m_vRotationQuaternion.y += fTimeDelta * 30.f;
	//if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_K))
	//	m_vRotationQuaternion.y -= fTimeDelta * 30.f;
	//
	//if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_M))
	//	m_vRotationQuaternion.z += fTimeDelta * 30.f;
	//if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_N))
	//	m_vRotationQuaternion.z -= fTimeDelta * 30.f;
	//
	//
	//m_pTransformCom->Rotation(XMConvertToRadians(m_vRotationQuaternion.x), XMConvertToRadians(m_vRotationQuaternion.y), XMConvertToRadians(m_vRotationQuaternion.z));
	//
	//swprintf_s(m_szRotationAngle, L"%.1f, %.1f, %.1f", m_vRotationQuaternion.x, m_vRotationQuaternion.y, m_vRotationQuaternion.z);
}

void CWeapon::Late_Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * SocketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
	//XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	if (m_bIsEnableCollider)
	{
		m_pColliderCom->UpdateColiision(XMLoadFloat4x4(&m_CombinedWorldMatrix));
		m_pGameInstance->ADD_Collider(m_pColliderCom);

#ifdef _DEBUG
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	}

	//=============== RENDERING LOGIC =================//
	if (false == isVisible())
		return;

	if (m_fTrailTime > 0.f)
	{
		m_fTrailTime -= fTimeDelta;
		if (m_fTrailTime <= 0.f)
		{
			m_bIsTrail = FALSE;
			m_pSpark->Stop();
		}
	}


	if (m_fChargeTime > 0.f)
	{
		m_fChargeTime -= fTimeDelta;
		if (m_fChargeTime <= 0.f)
		{
			m_pCharge->Stop();
		}
	}

	m_pTrail->Update_Trail(XMLoadFloat4x4(&m_CombinedWorldMatrix), fTimeDelta, m_bIsTrail);
	m_pSpark->Late_Update(fTimeDelta);
	m_pCharge->Late_Update(fTimeDelta);



	m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);


}

HRESULT CWeapon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_AllMaterials(i, m_pShaderCom, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	//m_pGameInstance->Render_Text(TEXT("KoPub"), m_szRotationAngle, _float2(g_iWinSizeX / 2 - 180, 0), XMVectorSet(1.f, 1.f, 1.f, 0.1f));

	return S_OK;
}

HRESULT CWeapon::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

void CWeapon::Active_SFX(const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)
{
	// 어떤 방식으로 짜야하지?
	// 1) strPartTag가 empty일 경우, Player에서 탐색 (V)
	//    strPartTag가 있을 경우, strPartTag로 모델을 탐색 (V)
	// 2) strObjectTag라는 매핑된 값을 문자열 탐색해서 찾고, Play() 함수를 실행함
	// 3) 그 모델들은 시간이 지난 후 알아서 Stop()

	if (strObjectTag == TEXT("Trail"))
	{
		m_fTrailTime = NotifyReference.fNumData01;
		m_bIsTrail = TRUE;
	}
	else if (strObjectTag == TEXT("Spark"))
	{
		m_pSpark->Play();
	}
	else if (strObjectTag == TEXT("Charge"))
	{
		m_pCharge->Play();
		m_fChargeTime = NotifyReference.fNumData01;
	}

}

void CWeapon::Activate_PartObject_Collider(const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef)
{
	auto pComponents = Find_Component(strColliderTag);
	if (nullptr == pComponents)
		return;

	m_bIsEnableCollider = NotifyRef.iNumData01;
	if (false == m_bIsEnableCollider)
		static_cast<CCollider*>(pComponents)->ResetCollision();
}

void CWeapon::EnableCollider(_bool bIsEnable)
{
	m_bIsEnableCollider = bIsEnable;
}

HRESULT CWeapon::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Weapon"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Collider_OBB */
	COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};

	OBBDesc.vSize = _float3(0.5f, 2.f, 0.5f);
	OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y, 0.f);
	OBBDesc.vAngles = _float3(0.f, 0.f/*XMConvertToRadians(45.0f)*/, 0.f);
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
		return E_FAIL;

	m_pColliderCom->BindBeginOverlapEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { Begin_OverlapEvent(vHitPoint, vHitDir, pHitActor); });
	m_pColliderCom->SetColliderHitType(HIT_TYPE::PLAYER);

	m_pColliderCom->ADD_IgnoreObject(HIT_TYPE::SENCE);
	m_pColliderCom->ADD_IgnoreObject(HIT_TYPE::PLAYER);
	m_pColliderCom->ADD_IgnoreObject(HIT_TYPE::INTERACTION);

	CTrail::TRAILHIGHLOW Traildesc{};
	Traildesc.vHigh = _float4(0.f, 5.f, 0.f, 0.f);
	Traildesc.vLow = _float4(0.f, 1.f, 0.f, 0.f);
	m_pTrail = static_cast<CTrailEffect*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_TrailEffect_Default_Slash"), &Traildesc));

	CEffect::EFFECT_TRANSFORM_DESC desc;
	desc.fRotationPerSec = 1.f;
	desc.fSpeedPerSec = 1.f;
	desc.pRootMatrix = &m_CombinedWorldMatrix;
	desc.vPos = XMVectorSet(0, 5, 0, 1);
	desc.fRot = _float3(0, 0, 0);
	desc.fSize = 1.5f;

	m_pSpark = static_cast<CEffect*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Slash_Spark"), &desc));
	m_pSpark->Play();
	m_pSpark->Stop();

	//m_pCharge->Stop();

	return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

void CWeapon::Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	CNayitba* pNaytiba = dynamic_cast<CNayitba*>(pHitActor);
	if (pNaytiba)
	{
		DEFAULT_DAMAGE_DESC pDamageDesc = {};
		pDamageDesc.pAttacker = m_pParent;
		pDamageDesc.vHitPoint = vHitPoint;
		pDamageDesc.vHitDir = vHitDir;

		pDamageDesc.pSkillData = m_pGameManager->Find_SkillData(static_cast<CPlayer*>(m_pParent)->GetSillDataID());
		pNaytiba->Damaged(&pDamageDesc);
	}
}

_bool CWeapon::isVisible()
{
	return m_pPlayerDesc->isWeaponVisible;
}

CWeapon* CWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon* pInstance = new CWeapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon::Clone(void* pArg)
{
	CWeapon* pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTrail);
	Safe_Release(m_pSpark);
	Safe_Release(m_pCharge);
}