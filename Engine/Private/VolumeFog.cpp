#include "VolumeFog.h"

#include "GameInstance.h"

CVolumeFog::CVolumeFog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeferred{pDevice, pContext}
{
}

void* CVolumeFog::Get_Desc()
{
	m_tVolumeFogDesc.pInscatterDesc = &m_tInscatterDesc;
	m_tVolumeFogDesc.pVolumeFogDesc = &m_tVolumeFogCommonDesc;
	m_tVolumeFogDesc.pActive = &m_isActive;

	return &m_tVolumeFogDesc;
}

void CVolumeFog::Set_Desc(void* pArg)
{
}

HRESULT CVolumeFog::Initialize()
{
	/* 셰이더 파일 로딩 */
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_VolumeFog.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	/* 픽셀 당 스캐터링 시작 함수 */
	m_pInScatterShader = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_InScattering.hlsl"), 
		"main", 0);

	/* 픽셀 당 스캐터링 누적 함수 */
	m_pAccumulateScatteringShader = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_AccumulateScattering.hlsl"),
		"main", 0);

	m_pNoiseTextureCom = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/3DPerlinNoise.dds"), 1);

	_uint2 vScreenSize = m_pGameInstance->GetScreenSize();
	/* Target_Fog. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_VolumeFog"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 1.0f))))
		return E_FAIL;
	/* MRT_Fog */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_VolumeFog"), TEXT("Target_VolumeFog"))))
		return E_FAIL;

	if (FAILED(Ready_Texture()))
		return E_FAIL;

	if (FAILED(Ready_ConstantBuffer()))
		return E_FAIL;

	return S_OK;
}

void CVolumeFog::Update(_float fTimeDelta)
{
	/* 
	다른 제어 필요없음? 
	-> ㅇㅇ. 같은 Context라면 입력이 들어온대로만 실행해줌.

	그래서 현재 Bind, Unbind, UpdateShader 다 순서대로 잘 돌아감.
	*/
	m_fNoiseTimeAcc += fTimeDelta;
	m_fNoiseTimeAcc = fmod(m_fNoiseTimeAcc, 16384.0f);
	m_tInscatterDesc.NoiseTime = m_fNoiseTimeAcc;

	CAMERA_INFO 	CamInfo = m_pGameInstance->Get_CurrentCamInfo();
	m_pShader->Bind_RawValue("g_fFar", &CamInfo.fFar, sizeof(_float));

	memcpy(&m_tInscatterDesc.ViewMatrix, m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW), sizeof(_float4x4));
	memcpy(&m_tInscatterDesc.ProjMatrix, m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ), sizeof(_float4x4));
	memcpy(&m_tInscatterDesc.PreViewMatrix, m_pGameInstance->Get_PreTransform_Float4x4(D3DTS::VIEW), sizeof(_float4x4));
	memcpy(&m_tInscatterDesc.PreProjMatrix, m_pGameInstance->Get_PreTransform_Float4x4(D3DTS::PROJ), sizeof(_float4x4));
	memcpy(&m_tInscatterDesc.InvViewMatrix, m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW), sizeof(_float4x4));
	memcpy(&m_tInscatterDesc.InvProjMatrix, m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ), sizeof(_float4x4));
	memcpy(&m_tInscatterDesc.CameraPos, m_pGameInstance->Get_CamPosition(), sizeof(_float4));

	memcpy(&m_tVolumeFogCommonDesc.NearPlaneDist, &m_tInscatterDesc.StartDistance, sizeof(_float));
	memcpy(&m_tVolumeFogCommonDesc.FarPlaneDist, &CamInfo.fFar, sizeof(_float));

	m_pContext->UpdateSubresource(m_pInscatterBuffer, 0, nullptr, &m_tInscatterDesc, 0, 0);
	m_pContext->UpdateSubresource(m_pFogDescBuffer, 0, nullptr, &m_tVolumeFogCommonDesc, 0, 0);

	m_pInScatterShader->Bind_ConstBuffer_Slot(0, m_pInscatterBuffer); // b0
	m_pInScatterShader->Bind_ConstBuffer_Slot(1, m_pFogDescBuffer);   // b1

	ID3D11ShaderResourceView* pSRV = m_pNoiseTextureCom->Get_SRV(0);
	m_pInScatterShader->Bind_SRV(&pSRV);
	
	m_pContext->CSSetSamplers(0, 1, &m_pNoiseSampler);

	m_pInScatterShader->Bind_UAV(&m_pVolumeUAV);
	//m_vVolumeTextureSize
	m_pInScatterShader->Update_Shader(
		_float3(ceil(m_vVolumeTextureSize.x/ 8.f), ceil(m_vVolumeTextureSize.y /8.f), ceil(m_vVolumeTextureSize.z / 8.f)));
	m_pInScatterShader->Unbind_UAV();

	m_pAccumulateScatteringShader->Bind_ConstBuffer_Slot(1, m_pFogDescBuffer);   // b1

	m_pAccumulateScatteringShader->Bind_UAV(&m_pVolumeUAV);
	m_pAccumulateScatteringShader->Update_Shader(
		_float3(ceil(m_vVolumeTextureSize.x / 8.f), ceil(m_vVolumeTextureSize.y / 8.f), 1));
	m_pAccumulateScatteringShader->Unbind_UAV();
}

HRESULT CVolumeFog::Render(CVIBuffer_Rect* pVIBuffer)
{
	if (false == m_isActive)
	{
		m_pGameInstance->Clear_MRT(TEXT("MRT_VolumeFog"));
		return S_OK;
	}

	/* 블러 X 처리 */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_VolumeFog"))))
		return E_FAIL;

	_float fTimeDelta = m_pGameInstance->Get_TimeDelta(TEXT("GameLoopTime"));

	m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
	m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));
	m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ));

	m_pShader->Bind_RawValue("g_fDepthPackExponent", &m_tVolumeFogCommonDesc.DepthPackExponent, sizeof(_float));
	m_pShader->Bind_RawValue("g_fNearPlaneDist", &m_tVolumeFogCommonDesc.NearPlaneDist, sizeof(_float));
	m_pShader->Bind_RawValue("g_fFarPlaneDist", &m_tVolumeFogCommonDesc.FarPlaneDist, sizeof(_float));

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_SRV("g_VolumetricFogTexture", m_pVolumeSRV)))
		return E_FAIL;

	CAMERA_INFO 	CamInfo = m_pGameInstance->Get_CurrentCamInfo();
	m_pShader->Bind_RawValue("g_fFar", &CamInfo.fFar, sizeof(_float));

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float3))))
		return E_FAIL;

	// 상황따라 갈아끼울 수 있게.
	m_pShader->Begin(0);

	pVIBuffer->Bind_Resources();
	pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVolumeFog::Bind_RenderTarget(CShader* pShader, const _char* pConstantName)
{
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_VolumeFog"), pShader, pConstantName)))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CVolumeFog::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_VolumeFog"), fX, fY, fSizeX, fSizeY)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVolumeFog::Render_Debug(CVIBuffer_Rect* pVIBuffer, CShader* pShader)
{
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_VolumeFog"), pShader, pVIBuffer)))
		return E_FAIL;

	return S_OK;
}
#endif
HRESULT CVolumeFog::Ready_Texture()
{
	D3D11_TEXTURE3D_DESC Desc = {};
	Desc.Width = m_vVolumeTextureSize.x;
	Desc.Height = m_vVolumeTextureSize.y;
	Desc.Depth = m_vVolumeTextureSize.z; 
	Desc.MipLevels = 1;  
	Desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture3D(&Desc, nullptr, &m_pVolumeTexture)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = Desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	SRVDesc.Texture3D.MostDetailedMip = 0;
	SRVDesc.Texture3D.MipLevels = 1;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pVolumeTexture, &SRVDesc, &m_pVolumeSRV)))
		return E_FAIL;

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
	UAVDesc.Format = Desc.Format;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
	UAVDesc.Texture3D.MipSlice = 0;
	UAVDesc.Texture3D.FirstWSlice = 0;
	UAVDesc.Texture3D.WSize = Desc.Depth;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pVolumeTexture, &UAVDesc, &m_pVolumeUAV)))
		return E_FAIL;

	D3D11_SAMPLER_DESC SamplerDesc{};
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if(FAILED(m_pDevice->CreateSamplerState(&SamplerDesc, &m_pNoiseSampler)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVolumeFog::Ready_ConstantBuffer()
{

	D3D11_BUFFER_DESC InScatterDesc = {};
	InScatterDesc.ByteWidth = sizeof(INSCATTER_DESC);
	InScatterDesc.Usage = D3D11_USAGE_DEFAULT;
	InScatterDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	if (FAILED(m_pDevice->CreateBuffer(&InScatterDesc, nullptr, &m_pInscatterBuffer)))
		return E_FAIL;


	D3D11_BUFFER_DESC FogDesc = {};
	FogDesc.ByteWidth = sizeof(VOLUMEFOG_COMMON_DESC);
	FogDesc.Usage = D3D11_USAGE_DEFAULT;
	FogDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	if (FAILED(m_pDevice->CreateBuffer(&FogDesc, nullptr, &m_pFogDescBuffer)))
		return E_FAIL;

	m_tVolumeFogCommonDesc.Exposure = 1.0f;
	m_tVolumeFogCommonDesc.DepthPackExponent = 2.0f;  

	m_tInscatterDesc.AsymmetryParameterG = 0.2f;
	m_tInscatterDesc.Density = 0.03f;
	m_tInscatterDesc.Intensity = 0.8f;
	m_tInscatterDesc.LightDirection = _float4(1.f, -1.f, 0.f, 0.f);
	m_tInscatterDesc.LightColor = _float4(1.f, 1.f, 1.f, 1.f);

	return S_OK;
}

CVolumeFog* CVolumeFog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVolumeFog* pInstance = new CVolumeFog(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Create Failed : CVolumeFog");
	}

	return pInstance;
}

void CVolumeFog::Free()
{
	__super::Free();


	Safe_Release(m_pInscatterBuffer);
	Safe_Release(m_pFogDescBuffer);

	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pInScatterShader);
	Safe_Release(m_pAccumulateScatteringShader);

	Safe_Release(m_pVolumeUAV);
	Safe_Release(m_pVolumeSRV);
	Safe_Release(m_pVolumeTexture);
	Safe_Release(m_pNoiseSampler);
}
