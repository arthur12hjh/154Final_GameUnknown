#include "pch.h"
#include "TrailEffect.h"
#include "Trail.h"

#include "GameInstance.h"

CTrailEffect::CTrailEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTrailEffect::CTrailEffect(const CTrailEffect& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTrailEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrailEffect::Initialize(void* pArg)
{
	GAMEOBJECT_DESC desc = {};
	desc.fRotationPerSec = 1.f;
	desc.fSpeedPerSec = 15.f;
	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;

	CTrail::TRAILHIGHLOW TrailDesc{};
	TrailDesc.vHigh = { 0.f, 1.f, 0.f,0.f };
	TrailDesc.vLow = { 0.f, -1.f, 0.f,0.f };

	m_pTrail = CTrail::Create(m_pDevice, m_pContext)->Clone(&TrailDesc);
	_float4x4 a;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	m_eTeam = OBJECT_TEAM::FRIENDLY;
	return S_OK;
}

void CTrailEffect::Priority_Update(_float fTimeDelta)
{

}

void CTrailEffect::Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta;
	m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed);
	m_pTransformCom->Turn(XMVectorSet(0, 1, 0, 0), fTimeDelta * m_fSpeed);

	m_pTrail->Update_Trail(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()), fTimeDelta, true);
}

void CTrailEffect::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(m_eRender, this);
}

HRESULT CTrailEffect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(m_tData.iBegin)))
		return E_FAIL;

	m_pTrail->Render();
	return S_OK;
}

void CTrailEffect::Set_Components(TRAIL_DATA tData)
{
	Safe_Release(m_pShaderCom);
	m_tData = tData;
	m_fTime = 0;
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0, 0, 0, 1));
	Set_Texture(0, m_tData.szMaskTexture.c_str());
	Set_Texture(1, m_tData.szDiffuseTexture.c_str());
	Set_Texture(2, m_tData.szDissolveTexture.c_str());

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

	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTrailEffect.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
}

void CTrailEffect::Update(TRAIL_DATA tData)
{
	m_tData = tData;
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
}

HRESULT CTrailEffect::Set_Texture(_int iIndex, const char* szPrototype)
{

	Safe_Release(m_pTexture[iIndex]);
	_tchar sztPrototype[256] = { 0, };
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szPrototype, strlen(szPrototype), sztPrototype, 256);

	char pattern[MAX_PATH] = {};
	strcpy_s(pattern, MAX_PATH, "Com_Texture");

	snprintf(pattern, sizeof(pattern), "Com_Texture_%d", m_iCount++);


	_tchar sztPrototype2[256] = { 0, };
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pattern, strlen(pattern), sztPrototype2, 256);

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::TOOL), sztPrototype,
		sztPrototype2, reinterpret_cast<CComponent**>(&m_pTexture[iIndex]))))
		return E_FAIL;
	return S_OK;
}

HRESULT CTrailEffect::Ready_Components()
{
	return S_OK;
}

HRESULT CTrailEffect::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bisEnd", &m_bisPause, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_tData.fColor, sizeof(_float4))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskUV", &m_tData.fMaskUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskUVSpeed", &m_tData.fMaskUVSpeed, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskUVSize", &m_tData.fMaskUVSize, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseUV", &m_tData.fDiffuseUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseUVSpeed", &m_tData.fDiffuseUVSpeed, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseUVSize", &m_tData.fDiffuseUVSize, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveUV", &m_tData.fDissolveUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveUVSpeed", &m_tData.fDissolveUVSpeed, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveUVSize", &m_tData.fDissolveUVSize, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pTexture[0]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTexture[1]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTexture[2]->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0)))
		return E_FAIL;
	return S_OK;
}

CTrailEffect* CTrailEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrailEffect* pInstance = new CTrailEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrailEffect::Clone(void* pArg)
{
	CTrailEffect* pInstance = new CTrailEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrailEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrailEffect::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	for (_uint i = 0; i < 3; ++i)
		Safe_Release(m_pTexture[i]);
}
