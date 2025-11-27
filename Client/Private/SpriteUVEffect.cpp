#include "pch.h"
#include "SpriteUVEffect.h"

#include "GameInstance.h"

CSpriteUVEffect::CSpriteUVEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlendObject{ pDevice, pContext }
{
}

CSpriteUVEffect::CSpriteUVEffect(const CSpriteUVEffect& Prototype)
	: CBlendObject{ Prototype },
	m_tData{ Prototype.m_tData },
	m_eRender{ Prototype.m_eRender }
{
	m_eTeam = Prototype.m_eTeam;
}

HRESULT CSpriteUVEffect::Initialize_Prototype(const SPRITE_DATA* pSpriteData)
{
	m_tData = *pSpriteData;
	switch (m_tData.iSelectRender)
	{
	case 0:
		m_eRender = RENDER::NONBLEND;
		m_eTeam = OBJECT_TEAM::FRIENDLY;
		break;
	case 1:
		m_eRender = RENDER::NONLIGHT;
		m_eTeam = OBJECT_TEAM::FRIENDLY;
		break;
	case 2:
		m_eRender = RENDER::BLUR;
		m_eTeam = OBJECT_TEAM::FRIENDLY;
		break;
	case 3:
		m_eRender = RENDER::GLOW;
		m_eTeam = OBJECT_TEAM::NEUTRAL;
		break;
	case 4:
		m_eRender = RENDER::GLOW;
		m_eTeam = OBJECT_TEAM::ENEMY;
		break;
	case 5:
		m_eRender = RENDER::GLOW;
		m_eTeam = OBJECT_TEAM::FRIENDLY;
		break;
	case 6:
		m_eRender = RENDER::DISTORTION;
		m_eTeam = OBJECT_TEAM::FRIENDLY;
		break;
	case 7:
		m_eRender = RENDER::BLEND;
		m_eTeam = OBJECT_TEAM::FRIENDLY;
		break;
	}
	return S_OK;
}

HRESULT CSpriteUVEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_tData.fPosition));

	if (FAILED(Ready_Components()))
		return E_FAIL;
	return S_OK;
}

void CSpriteUVEffect::Priority_Update(_float fTimeDelta)
{
}

void CSpriteUVEffect::Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta;
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentMat));
	if (m_fTime > m_tData.fFPS * m_tData.iUV.x * m_tData.iUV.y) {
		m_isDead = true;
		return;
	}
}

void CSpriteUVEffect::Late_Update(_float fTimeDelta)
{
	if (m_fTime <= m_tData.fFPS * m_tData.iUV.x * m_tData.iUV.y) {
		Compute_Depth();
		m_pGameInstance->Add_RenderGroup(m_eRender, this);
	}
}

HRESULT CSpriteUVEffect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_tData.iBegin)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpriteUVEffect::Ready_Components()
{

	_tchar sztPrototype[256] = { 0, };
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_tData.szMaskTexture.c_str(), strlen(m_tData.szMaskTexture.c_str()), sztPrototype, 256);
	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), sztPrototype,
		TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pTexture[0]))))
		return E_FAIL;
	memset(sztPrototype, 0, sizeof(sztPrototype));

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_tData.szDiffuseTexture.c_str(), strlen(m_tData.szDiffuseTexture.c_str()), sztPrototype, 256);
	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), sztPrototype,
		TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pTexture[1]))))
		return E_FAIL;
	memset(sztPrototype, 0, sizeof(sztPrototype));

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_tData.szNormalTexture.c_str(), strlen(m_tData.szNormalTexture.c_str()), sztPrototype, 256);
	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), sztPrototype,
		TEXT("Com_NormalTexture"), reinterpret_cast<CComponent**>(&m_pTexture[2]))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxSpriteUVEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpriteUVEffect::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_CamMatrix", m_pGameInstance->GetMainCameraWorldMatrixPtr())))
		return E_FAIL;


	if (FAILED(m_pTexture[0]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTexture[1]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTexture[2]->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_tData.fColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSize", &m_tData.fSize, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iUV", &m_tData.iUV, sizeof(_int2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFPS", &m_tData.fFPS, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAngle", &m_tData.fAngle, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

CSpriteUVEffect* CSpriteUVEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const SPRITE_DATA* pSpriteData)
{
	CSpriteUVEffect* pInstance = new CSpriteUVEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pSpriteData)))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpriteUVEffect::Clone(void* pArg)
{
	CSpriteUVEffect* pInstance = new CSpriteUVEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpriteUVEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpriteUVEffect::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTexture[0]);
	Safe_Release(m_pTexture[1]);
	Safe_Release(m_pTexture[2]);
	Safe_Release(m_pShaderCom);
}
