#include "pch.h"
#include "SpriteEffect.h"

#include "GameInstance.h"

CSpriteEffect::CSpriteEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlendObject{ pDevice, pContext }
{
}

CSpriteEffect::CSpriteEffect(const CSpriteEffect& Prototype)
	: CBlendObject{ Prototype }
{
}

HRESULT CSpriteEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpriteEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
		m_pGameInstance->Random(0.f, 10.f),
		3.f,
		m_pGameInstance->Random(0.f, 10.f),
		1.f
	));

	return S_OK;
}

void CSpriteEffect::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CSpriteEffect::Update(_float fTimeDelta)
{
	m_fFrame += 90.f * fTimeDelta;
	if (m_fFrame >= 90.f)
		m_fFrame = 0.f;
}

void CSpriteEffect::Late_Update(_float fTimeDelta)
{
	Compute_Depth();

	// m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this);

	m_pGameInstance->Add_RenderGroup(RENDER::DISTORTION, this);
}

HRESULT CSpriteEffect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//현재 인덱스 3이 Distortion, 인덱스 2가 Blur야.
	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpriteEffect::Ready_Components()
{

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Explosion_Test"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpriteEffect::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	_float fDistortionIntensity = { 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDistortionIntensity", &fDistortionIntensity, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", static_cast<_uint>(0))))
		return E_FAIL;

	return S_OK;
}

CSpriteEffect* CSpriteEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpriteEffect* pInstance = new CSpriteEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpriteEffect::Clone(void* pArg)
{
	CSpriteEffect* pInstance = new CSpriteEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpriteEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpriteEffect::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
