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
		m_pGameInstance->Add_RenderGroup(m_eRender, this);
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
	Safe_Release(m_pSizeDiagramSRV);
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

	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshEffect.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements);

	ID3D11Buffer* pBuffer = nullptr;
	D3D11_BUFFER_DESC BufferDesc = {};
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.ByteWidth = sizeof(_float3) * m_tData.fSizeDiagrams.size();
	BufferDesc.StructureByteStride = sizeof(_float3);
	BufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA ConstBufferSubResource = {};
	ConstBufferSubResource.pSysMem = m_tData.fSizeDiagrams.data();

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &ConstBufferSubResource, &pBuffer)))
		return;



	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.Buffer.FirstElement = 0;
	SRVDesc.Buffer.NumElements = m_tData.fSizeDiagrams.size();
	if (FAILED(m_pDevice->CreateShaderResourceView(pBuffer, &SRVDesc, &m_pSizeDiagramSRV)))
		return;

	Safe_Release(pBuffer);
}

void CMeshEffect::Update(MESH_DATA tData)
{
	m_tData = tData;
	m_pTransformCom->Set_Scale(m_tData.fScale.x, m_tData.fScale.y, m_tData.fScale.z);
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_tData.fPosition));
	m_pTransformCom->Rotation(XMConvertToRadians(m_tData.fRotation.x), XMConvertToRadians(m_tData.fRotation.y), XMConvertToRadians(m_tData.fRotation.z));

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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fEndTime", &m_tData.fEndTime, sizeof(_float))))
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

	int iSizeCount = m_tData.fSizeDiagrams.size();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iSizeCount", &iSizeCount, sizeof(_int))))
		return E_FAIL;

	m_pShaderCom->Bind_SRV("g_fSizeDiagram", m_pSizeDiagramSRV);
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
	Safe_Release(m_pSizeDiagramSRV);
	for (_uint i = 0; i < 3; ++i)
		Safe_Release(m_pTexture[i]);
}
