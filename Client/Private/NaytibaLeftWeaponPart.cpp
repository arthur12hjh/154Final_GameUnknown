#include "pch.h"
#include "NaytibaLeftWeaponPart.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Effect.h"
#include "Trail.h"
#include "TrailEffect.h"
#include "StringHelper.h"

CNaytibaLeftWeaponPart::CNaytibaLeftWeaponPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CPartObject(pDevice, pContext)
{
}

CNaytibaLeftWeaponPart::CNaytibaLeftWeaponPart(const CNaytibaLeftWeaponPart& Prototype) :
    CPartObject(Prototype)
{
}

HRESULT CNaytibaLeftWeaponPart::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNaytibaLeftWeaponPart::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;

    SetVisibility(VISIBILITY::VISIBLE);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(*pDesc)))
		return E_FAIL;

	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::NORMAL, "g_NormalTexture");
	//m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::EMISSIVE, "g_EmissiveTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORM, "g_ORMTexture");

	m_pTransformCom->Rotation(XMConvertToRadians(90.f), XMConvertToRadians(-90.f), XMConvertToRadians(0.f));

	return S_OK;
}

void CNaytibaLeftWeaponPart::Priority_Update(_float fTimeDelta)
{
}

void CNaytibaLeftWeaponPart::Update(_float fTimeDelta)
{
}

void CNaytibaLeftWeaponPart::Late_Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	_matrix		ParentMatrix = XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());
	_matrix		MyMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());

	for (size_t i = 0; i < 3; i++)
		MyMatrix.r[i] = XMVector3Normalize(MyMatrix.r[i]);
	for (size_t i = 0; i < 3; i++)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	for (size_t i = 0; i < 3; i++)
		ParentMatrix.r[i] = XMVector3Normalize(ParentMatrix.r[i]);
	//m_pTransformCom->Get_WorldMatrixPtr())

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		MyMatrix * SocketMatrix * ParentMatrix);
	//XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    if (m_eVisibility == VISIBILITY::VISIBLE)
    {
        m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
        m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
    }

	m_fLineTime += fTimeDelta;
	for (auto TrailEffect : m_pTrailEffects)
	{
		if (nullptr == TrailEffect.first->pRootMatrix) {
			TrailEffect.first->pTrailEffect->Update_Trail(XMLoadFloat4x4(&m_CombinedWorldMatrix), fTimeDelta, TrailEffect.first->bisPlay);
		}
		else {
			TrailEffect.first->pTrailEffect->Update_Trail(XMLoadFloat4x4(TrailEffect.first->pRootMatrix) * XMLoadFloat4x4(&m_CombinedWorldMatrix), fTimeDelta, TrailEffect.first->bisPlay);
		}
	}
	for (auto LineTrailEffect : m_pLineTrailEffects)
	{
		if (nullptr == LineTrailEffect.first->pRootMatrix) {

			_matrix mat = XMLoadFloat4x4(&m_CombinedWorldMatrix);
			LineTrailEffect.first->pTrailEffect->Update_Trail(mat, LineTrailEffect.first->bisPlay ? fTimeDelta : fTimeDelta * 1.5f, LineTrailEffect.first->bisPlay);
		}
		else {

			_matrix mat = XMLoadFloat4x4(LineTrailEffect.first->pRootMatrix) * XMLoadFloat4x4(&m_CombinedWorldMatrix);
			LineTrailEffect.first->pTrailEffect->Update_Trail(mat, LineTrailEffect.first->bisPlay ? fTimeDelta : fTimeDelta * 1.5f, LineTrailEffect.first->bisPlay);
		}
	}
}

HRESULT CNaytibaLeftWeaponPart::Render()
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

	return S_OK;
}

HRESULT CNaytibaLeftWeaponPart::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
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

void CNaytibaLeftWeaponPart::Active_SFX(const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)
{
    if (strObjectTag == TEXT("Play_Effect"))
    {
        CEffect* pEffect = nullptr;
        for (auto Effect : m_pEffects)
        {
            if (Effect.second == NotifyReference.iNumData01) {
                pEffect = Effect.first;
                break;
            }
        }

        if (nullptr == pEffect) {
            CEffect::EFFECT_TRANSFORM_DESC EffectDesc;
            EffectDesc.fRotationPerSec = 1.f;
            EffectDesc.fSpeedPerSec = 1.f;
            EffectDesc.fSpeed = NotifyReference.fNumData01;

            if (NotifyReference.szSocketTag.compare("None") == 0)
            {
                EffectDesc.pRootMatrix = nullptr;
                EffectDesc.pWorldMatrix = nullptr;
            }
            else if (NotifyReference.szSocketTag.compare("Transform") == 0)
            {
                EffectDesc.pRootMatrix = &m_CombinedWorldMatrix;
                EffectDesc.pWorldMatrix = nullptr;
            }
            else
            {
                EffectDesc.pRootMatrix = m_pModelCom->Get_BoneMatrixPtr(NotifyReference.szSocketTag.c_str());
                EffectDesc.pWorldMatrix = &m_CombinedWorldMatrix;
            }

            EffectDesc.vPos = XMVectorSet(NotifyReference.vNotifyPosition.x, NotifyReference.vNotifyPosition.y, NotifyReference.vNotifyPosition.z, 1);
            EffectDesc.fRot = _float3(XMConvertToRadians(NotifyReference.vNotifyRotation.x), XMConvertToRadians(NotifyReference.vNotifyRotation.y), XMConvertToRadians(NotifyReference.vNotifyRotation.z));
            EffectDesc.fSize = NotifyReference.vNotifyScale.x;
            EffectDesc.iFloor = NotifyReference.iNumData02;
            _TCHAR szEffectTag[MAX_PATH];
            CStringHelper::ConvertUTFToWide(NotifyReference.szNotifyArg02.c_str(), szEffectTag);

            pEffect = static_cast<CEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), szEffectTag,
                ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc));

            Safe_AddRef(pEffect);
            m_pEffects.push_back({ pEffect, NotifyReference.iNumData01 });
        }
        pEffect->Play();
    }
    else if (strObjectTag == TEXT("Stop_Effect"))
    {
        CEffect* pEffect = nullptr;
        for (auto Effect : m_pEffects)
        {
            if (Effect.second == NotifyReference.iNumData01) {
                pEffect = Effect.first;
                break;
            }
        }
        if (nullptr != pEffect)
            pEffect->Stop();
    }
    else if (strObjectTag == TEXT("Stop_All_Effect"))
    {
        for (auto Effect : m_pEffects)
        {
            Effect.first->Stop();
        }
        for (auto TrailEffect : m_pTrailEffects)
        {
            TrailEffect.first->bisPlay = false;
        }
    }
    else if (strObjectTag == TEXT("Play_Trail"))
    {
        CTrailEffect* pTrailEffect = nullptr;
        for (auto TrailEffect : m_pTrailEffects)
        {
            if (TrailEffect.second == NotifyReference.iNumData01) {
                TrailEffect.first->bisPlay = true;
                return;
            }
        }

        if (nullptr == pTrailEffect) {

            NAYITBA_TRAIL_DESC* ptrailDesc = new NAYITBA_TRAIL_DESC;

            if (NotifyReference.szSocketTag.compare("Transform") != 0)
            {
                ptrailDesc->pRootMatrix = m_pModelCom->Get_BoneMatrixPtr(NotifyReference.szSocketTag.c_str());
            }
            ptrailDesc->bisPlay = true;

            CTrailEffect::TRAIL_DATA Traildesc{};
            Traildesc.vHigh = _float4(NotifyReference.vNotifyScale.x, NotifyReference.vNotifyScale.y, NotifyReference.vNotifyScale.z, 0.f);
            Traildesc.vLow = _float4(NotifyReference.vNotifyPosition.x, NotifyReference.vNotifyPosition.y, NotifyReference.vNotifyPosition.z, 0.f);
            Traildesc.bisLine = false;
            Traildesc.bisLong = NotifyReference.iNumData02 == 1;
            _TCHAR szEffectTag[MAX_PATH];
            CStringHelper::ConvertUTFToWide(NotifyReference.szNotifyArg02.c_str(), szEffectTag);

            pTrailEffect = static_cast<CTrailEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), szEffectTag,
                ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &Traildesc));
            ptrailDesc->pTrailEffect = pTrailEffect;
            Safe_AddRef(pTrailEffect);
            m_pTrailEffects.push_back({ ptrailDesc, NotifyReference.iNumData01 });
        }
    }
    else if (strObjectTag == TEXT("Stop_Trail"))
    {
        for (auto TrailEffect : m_pTrailEffects)
        {
            if (TrailEffect.second == NotifyReference.iNumData01) {
                TrailEffect.first->bisPlay = false;
                return;
            }
        }
    }

    else if (strObjectTag == TEXT("Play_LineTrail"))
    {
        CTrailEffect* pTrailEffect = nullptr;
        for (auto LineTrailEffect : m_pLineTrailEffects)
        {
            if (LineTrailEffect.second == NotifyReference.iNumData01) {
                LineTrailEffect.first->bisPlay = true;
                return;
            }
        }

        NAYITBA_LINE_TRAIL_DESC* pLinetrailDesc = new NAYITBA_LINE_TRAIL_DESC;

        if (NotifyReference.szSocketTag.compare("Transform") != 0)
        {
            pLinetrailDesc->pRootMatrix = m_pModelCom->Get_BoneMatrixPtr(NotifyReference.szSocketTag.c_str());
        }
        pLinetrailDesc->bisPlay = true;

        CTrailEffect::TRAIL_DATA Traildesc{};
        Traildesc.vHigh = _float4(NotifyReference.vNotifyScale.x, NotifyReference.vNotifyScale.y, NotifyReference.vNotifyScale.z, 0.f);
        Traildesc.vLow = _float4(NotifyReference.vNotifyPosition.x, NotifyReference.vNotifyPosition.y, NotifyReference.vNotifyPosition.z, 0.f);
        Traildesc.fSpeed = NotifyReference.fNumData01;
        Traildesc.fPow = NotifyReference.fNumData02;
        Traildesc.bisLine = true;

        _TCHAR szEffectTag[MAX_PATH];
        CStringHelper::ConvertUTFToWide(NotifyReference.szNotifyArg02.c_str(), szEffectTag);

        pTrailEffect = static_cast<CTrailEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), szEffectTag,
            ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &Traildesc));
        pLinetrailDesc->pTrailEffect = pTrailEffect;
        Safe_AddRef(pTrailEffect);
        m_pLineTrailEffects.push_back({ pLinetrailDesc, NotifyReference.iNumData01 });
    }
    else if (strObjectTag == TEXT("Stop_LineTrail"))
    {
        for (auto LineTrailEffect : m_pLineTrailEffects)
        {
            if (LineTrailEffect.second == NotifyReference.iNumData01) {
                LineTrailEffect.first->bisPlay = false;
                return;
            }
        }
    }
}

void CNaytibaLeftWeaponPart::Activate_PartObject_Collider(const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef)
{
}

void CNaytibaLeftWeaponPart::EnableCollider(_bool bIsEnable)
{
}

HRESULT CNaytibaLeftWeaponPart::Ready_Components(const WEAPON_DESC& Desc)
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), Desc.szWeaponModelPrototype,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CNaytibaLeftWeaponPart::Bind_ShaderResources()
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

CNaytibaLeftWeaponPart* CNaytibaLeftWeaponPart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNaytibaLeftWeaponPart* pInstance = new CNaytibaLeftWeaponPart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Naytiba Left Weapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNaytibaLeftWeaponPart::Clone(void* pArg)
{
	CNaytibaLeftWeaponPart* pInstance = new CNaytibaLeftWeaponPart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Naytiba Left Weapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNaytibaLeftWeaponPart::Free()
{
	__super::Free();
    for (auto pEffect : m_pEffects)
        Safe_Release(pEffect.first);
    m_pEffects.clear();

    for (auto& pTrailEffect : m_pTrailEffects)
    {
        Safe_Release(pTrailEffect.first->pTrailEffect);
        Safe_Delete(pTrailEffect.first);
    }

    m_pTrailEffects.clear();


    for (auto& pLineTrailEffect : m_pLineTrailEffects)
    {
        Safe_Release(pLineTrailEffect.first->pTrailEffect);
        Safe_Delete(pLineTrailEffect.first);
    }

    m_pLineTrailEffects.clear();
}
