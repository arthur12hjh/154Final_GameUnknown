#include "VIBuffer_Point_Instance.h"
#include "GameInstance.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype)
	: CVIBuffer_Instance{ Prototype }
	, m_pInstanceVertices{ Prototype.m_pInstanceVertices }
	, m_CBData{ Prototype.m_CBData }
	, m_pDropShaderCom{ Prototype.m_pDropShaderCom }
	, m_pSpreadShaderCom{ Prototype.m_pSpreadShaderCom}
{

}

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc)
{
	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOS);

	m_iNumIndices = 0;
	m_iIndexStride = 0;


	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER
	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.StructureByteStride = m_iVertexStride;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	m_pVertexPositions[0] = pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);

	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INSTANCE_BUFFER
	const POINT_INSTANCE_DESC* pDesc = static_cast<const POINT_INSTANCE_DESC*>(pInstanceDesc);
	m_vPivot = pDesc->vPivot;
	m_isLoop = pDesc->isLoop;
	m_iNumInstance = pDesc->iNumInstance;
	m_iInstanceStride = sizeof(VTX_INSTANCE_POINT_PARTICLE);
	m_iNumIndexPerInstance = 6;

	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	//m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_pInstanceVertices = new VTX_INSTANCE_POINT_PARTICLE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTX_INSTANCE_POINT_PARTICLE) * m_iNumInstance);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float			fScale = m_pGameInstance->Random(pDesc->vSize.x, pDesc->vSize.y);

		m_pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		m_pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
		m_pInstanceVertices[i].vTranslation = _float4(
			m_pGameInstance->Random(pDesc->vCenter.x - pDesc->vRange.x * 0.5f, pDesc->vCenter.x + pDesc->vRange.x * 0.5f),
			m_pGameInstance->Random(pDesc->vCenter.y - pDesc->vRange.y * 0.5f, pDesc->vCenter.y + pDesc->vRange.y * 0.5f),
			m_pGameInstance->Random(pDesc->vCenter.z - pDesc->vRange.z * 0.5f, pDesc->vCenter.z + pDesc->vRange.z * 0.5f),
			1.f);

		m_pInstanceVertices[i].vLifeTime = _float2(0.0f, m_pGameInstance->Random(pDesc->vLifeTime.x, pDesc->vLifeTime.y));
		m_pInstanceVertices[i].vSpeeds.x = m_pGameInstance->Random(pDesc->vSpeed.x, pDesc->vSpeed.y);
	}

	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;

	m_CBData.fPivot.x = pDesc->vPivot.x;
	m_CBData.fPivot.y = pDesc->vPivot.y;
	m_CBData.fPivot.z = pDesc->vPivot.z;
	m_CBData.iLoopAndCount.x = pDesc->isLoop ? 1 : 0;
	m_CBData.iLoopAndCount.y = m_iNumInstance;

#pragma endregion


	_uint HLSLFlags = {};
	ID3DBlob* pBlobVS = nullptr;
	ID3DBlob* pBlobCode = nullptr;
#ifdef _DEBUG
	HLSLFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	HLSLFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	if (FAILED(D3DCompileFromFile(
		TEXT("../Bin/ShaderFiles/Shader_Compute_Point_Drop.hlsl"),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"CS",
		"cs_5_0",
		HLSLFlags,
		0,
		&pBlobCode,
		&pBlobVS)))
	{
		if (pBlobVS) {
			OutputDebugStringA((char*)pBlobVS->GetBufferPointer());
			pBlobVS->Release();
		}
		return E_FAIL;
	}
	if (FAILED(m_pDevice->CreateComputeShader(
		pBlobCode->GetBufferPointer(),
		pBlobCode->GetBufferSize(),
		nullptr,
		&m_pDropShaderCom)))
		return E_FAIL;
	Safe_Release(pBlobVS);
	Safe_Release(pBlobCode);
	if (FAILED(D3DCompileFromFile(
		TEXT("../Bin/ShaderFiles/Shader_Compute_Spread.hlsl"),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"CS",
		"cs_5_0",
		HLSLFlags,
		0,
		&pBlobCode,
		&pBlobVS)))
	{
		if (pBlobVS) {
			OutputDebugStringA((char*)pBlobVS->GetBufferPointer());
			pBlobVS->Release();
		}
		return E_FAIL;
	}
	if (FAILED(m_pDevice->CreateComputeShader(
		pBlobCode->GetBufferPointer(),
		pBlobCode->GetBufferSize(),
		nullptr,
		&m_pSpreadShaderCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Initialize(void* pArg)
{
	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialDesc, &m_pVBInstance)))
		return E_FAIL;

	D3D11_BUFFER_DESC BufferDesc = {};
	BufferDesc.ByteWidth = (sizeof(PointConstBufferData) + 15) / 16 * 16;
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA ConstBufferSubResource = {};
	ConstBufferSubResource.pSysMem = &m_CBData;

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &ConstBufferSubResource, &m_pConstPointBuffer)))
		return E_FAIL;

	D3D11_BUFFER_DESC TrialInitBufferDesc = {};
	TrialInitBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	TrialInitBufferDesc.ByteWidth = sizeof(VTX_INSTANCE_POINT_PARTICLE) * m_iNumInstance;
	TrialInitBufferDesc.StructureByteStride = sizeof(VTX_INSTANCE_POINT_PARTICLE);
	TrialInitBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	TrialInitBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA SubResource = {};
	SubResource.pSysMem = m_pInstanceVertices;
	if (FAILED(m_pDevice->CreateBuffer(&TrialInitBufferDesc, &SubResource, &m_pResourceBuffer[0])))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&TrialInitBufferDesc, nullptr, &m_pResourceBuffer[1])))
		return E_FAIL;

#pragma region SRV Buffer
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.Buffer.NumElements = m_iNumInstance;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pResourceBuffer[0], &SRVDesc, &m_pPointInputSRV[0])))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pResourceBuffer[1], &SRVDesc, &m_pPointInputSRV[1])))
		return E_FAIL;
#pragma endregion

#pragma region UAV Buffer
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.Buffer.NumElements = m_iNumInstance;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pResourceBuffer[0], &UAVDesc, &m_pPointOutUAV[0])))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pResourceBuffer[1], &UAVDesc, &m_pPointOutUAV[1])))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Bind_Resources()
{
	m_pContext->CopyResource(m_pVBInstance, m_pResourceBuffer[m_bFlag ? 0 : 1]);
	ID3D11Buffer* VertexBuffers[] = {
		   m_pVB,
		   m_pVBInstance
	};

	_uint		VertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint		Offsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, VertexBuffers, VertexStrides, Offsets);

	m_pContext->IASetPrimitiveTopology(m_ePrimitive);

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Render()
{
	m_pContext->DrawInstanced(m_iNumIndexPerInstance, m_iNumInstance, 0, 0);

	return S_OK;
}

void CVIBuffer_Point_Instance::Drop(_float fTimeDelta)
{
	m_CBData.iLoopAndCount.x = m_isLoop ? 1 : 0;
	m_CBData.fTimeDelta.x = fTimeDelta;
	m_pContext->UpdateSubresource(m_pConstPointBuffer, 0, nullptr, &m_CBData, 0, 0);
	ID3D11ShaderResourceView* pViewNULL = NULL;

	ID3D11ShaderResourceView* inputSRV = m_pPointInputSRV[m_bFlag ? 0 : 1];
	ID3D11UnorderedAccessView* outputUAV = m_pPointOutUAV[m_bFlag ? 1 : 0];
	m_bFlag = !m_bFlag;

	m_pContext->CSSetShaderResources(0, 1, &inputSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 1, &outputUAV, nullptr);
	m_pContext->CSSetConstantBuffers(0, 1, &m_pConstPointBuffer);

	unsigned int groupCount = (m_iNumInstance + 255) / 256;
	m_pContext->CSSetShader(m_pDropShaderCom, nullptr, 0);
	m_pContext->Dispatch(groupCount, 1, 1);

	ID3D11ShaderResourceView* nullSRV = { nullptr };
	ID3D11UnorderedAccessView* nullUAV = { nullptr };
	m_pContext->CSSetShader(nullptr, nullptr, 0);
	m_pContext->CSSetShaderResources(0, 1, &nullSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
}

void CVIBuffer_Point_Instance::Spread(_float fTimeDelta)
{
	m_CBData.iLoopAndCount.x = m_isLoop ? 1 : 0;
	m_CBData.fTimeDelta.x = fTimeDelta;
	m_pContext->UpdateSubresource(m_pConstPointBuffer, 0, nullptr, &m_CBData, 0, 0);
	ID3D11ShaderResourceView* pViewNULL = NULL;

	ID3D11ShaderResourceView* inputSRV = m_pPointInputSRV[m_bFlag ? 0 : 1];
	ID3D11UnorderedAccessView* outputUAV = m_pPointOutUAV[m_bFlag ? 1 : 0];
	m_bFlag = !m_bFlag;

	m_pContext->CSSetShaderResources(0, 1, &inputSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 1, &outputUAV, nullptr);
	m_pContext->CSSetConstantBuffers(0, 1, &m_pConstPointBuffer);

	_uint groupCount = (m_iNumInstance + 255) / 256;
	m_pContext->CSSetShader(m_pSpreadShaderCom, nullptr, 0);
	m_pContext->Dispatch(groupCount, 1, 1);

	ID3D11ShaderResourceView* nullSRV = { nullptr };
	ID3D11UnorderedAccessView* nullUAV = { nullptr };
	m_pContext->CSSetShader(nullptr, nullptr, 0);
	m_pContext->CSSetShaderResources(0, 1, &nullSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
}

CVIBuffer_Point_Instance* CVIBuffer_Point_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pInstanceDesc)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Point_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Point_Instance::Clone(void* pArg)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Point_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CVIBuffer_Point_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pInstanceVertices);
		Safe_Release(m_pSpreadShaderCom);
		Safe_Release(m_pDropShaderCom);
	}
	Safe_Release(m_pResourceBuffer[0]);
	Safe_Release(m_pResourceBuffer[1]);
	Safe_Release(m_pPointInputSRV[0]);
	Safe_Release(m_pPointInputSRV[1]);
	Safe_Release(m_pPointOutUAV[0]);
	Safe_Release(m_pPointOutUAV[1]);
	Safe_Release(m_pConstPointBuffer);
}
