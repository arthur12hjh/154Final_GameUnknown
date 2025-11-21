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

	return S_OK;
}

void CSpriteEffect::Priority_Update(_float fTimeDelta)
{
}

void CSpriteEffect::Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta;
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentMat));
	if (m_fTime <= m_tData.fFPS * m_tData.iUV.x * m_tData.iUV.y)
		return;
}

void CSpriteEffect::Late_Update(_float fTimeDelta)
{
	Compute_Depth();
	m_pGameInstance->Add_RenderGroup(RENDER(m_tData.iSelectRender), this);
}

HRESULT CSpriteEffect::Render()
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

void CSpriteEffect::Update(SPRITE_DATA tData)
{
	m_tData = tData;
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_tData.fPosition));
}

void CSpriteEffect::Set_Components(SPRITE_DATA tData)
{
	Safe_Release(m_pShaderCom);
	m_tData = tData;
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_tData.fPosition));
	_tchar sztPrototype[256] = { 0, };
	Set_Texture(0, m_tData.szMaskTexture.c_str());
	Set_Texture(1, m_tData.szDiffuseTexture.c_str());
	Set_Texture(2, m_tData.szNormalTexture.c_str());

	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxSpriteEffect.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
}

HRESULT CSpriteEffect::Set_Texture(_int iIndex, const char* szPrototype)
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

HRESULT CSpriteEffect::Ready_Components()
{

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpriteEffect::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float3))))
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
	Safe_Release(m_pTexture[0]);
	Safe_Release(m_pTexture[1]);
	Safe_Release(m_pTexture[2]);
	Safe_Release(m_pShaderCom);
}
