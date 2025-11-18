#include "pch.h"
#include "MeshEffect.h"

#include "GameInstance.h"

CMeshEffect::CMeshEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMeshEffect::CMeshEffect(const CMeshEffect& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CMeshEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMeshEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	return S_OK;
}

void CMeshEffect::Priority_Update(_float fTimeDelta)
{

}

void CMeshEffect::Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta;
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentMat));
}

void CMeshEffect::Late_Update(_float fTimeDelta)
{
	if ((0 < m_tData.fEndTime && m_tData.fEndTime <= m_fTime))
		return;
	if(0 <= m_fTime)
		m_pGameInstance->Add_RenderGroup(RENDER(m_tData.iSelectRender), this);
}

HRESULT CMeshEffect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(m_tData.iBegin)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

void CMeshEffect::Set_Components(MESH_DATA tData)
{
	Safe_Release(m_pShaderCom);
	m_tData = tData;
	m_fTime = -m_tData.fDelayTime;
	m_pTransformCom->Set_Scale(m_tData.fScale.x, m_tData.fScale.y, m_tData.fScale.z);
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_tData.fPosition));
	m_pTransformCom->Rotation(XMConvertToRadians(m_tData.fRotation.x), XMConvertToRadians(m_tData.fRotation.y), XMConvertToRadians(m_tData.fRotation.z));

	_tchar sztPrototype[256] = { 0, };
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_tData.szModel.c_str(), strlen(m_tData.szModel.c_str()), sztPrototype, 256);
	Set_Model(sztPrototype);
	Set_Texture(0, m_tData.szMaskTexture.c_str());
	Set_Texture(1, m_tData.szDiffuseTexture.c_str());
	Set_Texture(2, m_tData.szDissolveTexture.c_str());

	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshEffect.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements);
}

void CMeshEffect::Update(MESH_DATA tData)
{
	m_tData = tData;
	m_pTransformCom->Set_Scale(m_tData.fScale.x, m_tData.fScale.y, m_tData.fScale.z);
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_tData.fPosition));
	m_pTransformCom->Rotation(XMConvertToRadians(m_tData.fRotation.x), XMConvertToRadians(m_tData.fRotation.y), XMConvertToRadians(m_tData.fRotation.z));
}

HRESULT CMeshEffect::Set_Texture(_int iIndex, const char* szPrototype)
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

void CMeshEffect::Set_Model(_wstring szMode)
{
	Safe_Release(m_pModelCom);


	char pattern[MAX_PATH] = {};
	strcpy_s(pattern, MAX_PATH, "Com_Model");

	snprintf(pattern, sizeof(pattern), "Com_Model_%d", m_iCount++);


	_tchar sztPrototype[256] = { 0, };
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pattern, strlen(pattern), sztPrototype, 256);


	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::TOOL), szMode,
		sztPrototype, reinterpret_cast<CComponent**>(&m_pModelCom))))
		return;
}

HRESULT CMeshEffect::Ready_Components()
{
	return S_OK;
}

HRESULT CMeshEffect::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
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

CMeshEffect* CMeshEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMeshEffect* pInstance = new CMeshEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMeshEffect::Clone(void* pArg)
{
	CMeshEffect* pInstance = new CMeshEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMeshEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshEffect::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	for (_uint i = 0; i < 3; ++i)
		Safe_Release(m_pTexture[i]);
}
