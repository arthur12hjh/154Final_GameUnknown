#include "pch.h"
#include "Particle.h"

#include "GameInstance.h"

CParticle::CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CParticle::CParticle(const CParticle& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CParticle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	return S_OK;
}

void CParticle::Priority_Update(_float fTimeDelta)
{

}

void CParticle::Update(_float fTimeDelta)
{
	Spread(fTimeDelta);
}

void CParticle::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER(m_tData.m_iSelectRender), this);
}

HRESULT CParticle::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_tData.iBegin);

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}

void CParticle::Set_Components(PARTICLE_DATA tData)
{
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pComputeShader);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pReadSource);
	CVIBuffer_Point_Instance::POINT_INSTANCE_DESC		Desc{};
	Desc.iNumInstance = tData.iNumInstance;
	Desc.vCenter = tData.fCenter;
	Desc.vPivot = tData.fPivot;
	Desc.vRange = tData.fRange;
	Desc.vSize = tData.fSize;
	Desc.vLifeTime = tData.fLifeTime;
	Desc.vSpeed = tData.fSpeed;
	Desc.isLoop = tData.bisLoop;

	m_pVIBufferCom = CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &Desc);
	m_pVIBufferCom->Initialize(nullptr);
	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointParticle.hlsl"), VTX_POS_INSTANCE_PARTICLE::Elements, VTX_POS_INSTANCE_PARTICLE::iNumElements);
	m_pComputeShader = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_Spread.hlsl"), tData.szCS.c_str(), tData.iNumInstance);
	//m_pComputeShader = pComputeShader;
	//m_pShaderCom = pShaderCom;
	m_tData = tData;
	Ready_ComputeShader();
}

HRESULT CParticle::Ready_Components()
{
	return S_OK;
}

HRESULT CParticle::Bind_ShaderResources()
{

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float3))))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vSize", &m_tData.fSizeDiagram, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_tData.fColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTexture[0]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTexture[1]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTexture[2]->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0)))
		return E_FAIL;
	return S_OK;
}

HRESULT CParticle::Ready_ComputeShader()
{
	D3D11_MAPPED_SUBRESOURCE pIntanceData = {};
	m_pVIBufferCom->Lock(D3D11_MAP_WRITE_NO_OVERWRITE, &pIntanceData);

#pragma region Bind Compute Shader
	// 이건 컴퓨트 셰이더를 바인딩한다.
	/* ComputeShader_Snow */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::TOOL), TEXT("Prototype_Component_ComputeShader_Expolosion"),
	//	TEXT("Com_ComputeShader"), reinterpret_cast<CComponent**>(&m_pComputeShader))))
	//	return E_FAIL;
#pragma endregion

	ID3D11Buffer* pBuffer = nullptr;

#pragma region Const Buffer Setting
	_uint iNumData = m_pComputeShader->GetNumData();
	m_CBData.vGravity = m_tData.fGravityDiagram;
	m_CBData.vPivot = { m_tData.fPivot.x,  m_tData.fPivot.y,  m_tData.fPivot.z, 1.f};
	m_CBData.iLoopAndCount.x = m_pVIBufferCom->IsLoop() ? 1 : 0;
	m_CBData.iLoopAndCount.y = iNumData;

	D3D11_BUFFER_DESC BufferDesc = {};
	BufferDesc.ByteWidth = (sizeof(PointConstBufferData) + 15) / 16 * 16;
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA ConstBufferSubResource = {};
	ConstBufferSubResource.pSysMem = &m_CBData;

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &ConstBufferSubResource, &pBuffer)))
		return E_FAIL;
	m_pComputeShader->ADD_Buffer(CComputeShader::BUFFER_TYPE::CONSTATNT, pBuffer);
#pragma endregion

#pragma region Input & Output Base Buffer
	D3D11_BUFFER_DESC TrialInitBufferDesc = {};
	TrialInitBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	TrialInitBufferDesc.ByteWidth = sizeof(CVIBuffer_Point_Instance::VTX_INSTANCE_VERTEX_PARTICLE) * iNumData;
	TrialInitBufferDesc.StructureByteStride = sizeof(CVIBuffer_Point_Instance::VTX_INSTANCE_VERTEX_PARTICLE);
	TrialInitBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	TrialInitBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA SubResource = {};
	SubResource.pSysMem = pIntanceData.pData;
#pragma region SRV Buffer
	if (FAILED(m_pDevice->CreateBuffer(&TrialInitBufferDesc, &SubResource, &pBuffer)))
		return E_FAIL;

	m_pComputeShader->ADD_Buffer(CComputeShader::BUFFER_TYPE::INPUT, pBuffer);
#pragma endregion

#pragma region Read Buffer
	D3D11_BUFFER_DESC ReadBufferDesc = {};
	ReadBufferDesc.Usage = D3D11_USAGE_STAGING;
	ReadBufferDesc.ByteWidth = sizeof(CVIBuffer_Point_Instance::VTX_INSTANCE_VERTEX_PARTICLE) * iNumData;
	ReadBufferDesc.StructureByteStride = sizeof(CVIBuffer_Point_Instance::VTX_INSTANCE_VERTEX_PARTICLE);
	ReadBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	if (FAILED(m_pDevice->CreateBuffer(&ReadBufferDesc, nullptr, &m_pReadSource)))
		return E_FAIL;
#pragma endregion

#pragma region UAV Buffer
	if (FAILED(m_pDevice->CreateBuffer(&TrialInitBufferDesc, &SubResource, &pBuffer)))
		return E_FAIL;

	m_pComputeShader->ADD_Buffer(CComputeShader::BUFFER_TYPE::OUTPUT, pBuffer);
#pragma endregion

#pragma endregion
	m_pVIBufferCom->UnLock();

	m_pVIBufferCom->CopyResource(m_pReadSource);

	return S_OK;
}

void CParticle::Spread(_float fTimeDelta)
{
	m_CBData.iLoopAndCount.x = m_pVIBufferCom->IsLoop() ? 1 : 0;
	m_CBData.fTimeDelta.x = fTimeDelta;
	m_CBData.matWorld = *m_pTransformCom->Get_WorldMatrixPtr();
	// 버퍼 세팅
	// Update_BufferResource 
	// 매개변수 1 : 어떤 버퍼 타입에서 데이터를 가져올지
	// 매개변수 2 : void* 타입의 변수를 넘겨주면 복사함
	m_pComputeShader->Update_BufferResource(CComputeShader::BUFFER_TYPE::CONSTATNT, 0, &m_CBData);

	// Update_BufferResource 
	// 매개변수 1 : 어떤 버퍼 타입에서 데이터를 가져올지
	// 매개변수 2 : ID3D11Buffer 타입의 변수를 넘겨주면 그 버퍼를 복사함
	m_pComputeShader->Update_BufferResource(CComputeShader::BUFFER_TYPE::INPUT, 0, m_pReadSource);

	_uint			GropIndex = { 0 };
	// 상수 버퍼 바인딩
	// Bind_ConstBuffer 
	// 매개변수 1 : 컴퓨트 셰이더에 몇개의 버퍼를 바인딩할지
	// 매개변수 2 : 어떤 Buffer들끼리 묶어서 넘길지
	// 의사 코드 ex) Bind_ConstBuffer(2, { 1, 2 })
	m_pComputeShader->Bind_ConstBuffer(1, &GropIndex);
	m_pComputeShader->Bind_InputBuffer(1, &GropIndex);
	m_pComputeShader->Bind_OutputBuffer(1, &GropIndex);

	unsigned int groupCount = (m_pComputeShader->GetNumData() + 255) / 256;
	m_pComputeShader->Update_Shader({ (_float)groupCount, 1, 1 });

	// 데이터 가져오는거
	// GetBufferResource
	// 매개변수 1 : 어떤 버퍼 타입에서 데이터를 가져올지
	// 매개변수 2 : 타입에 맞는 버퍼가 몇번째 버퍼인지
	// 매개변수 3 : 값을 받아올 ID3D11Buffer 타입의 변수
	m_pComputeShader->GetBufferResource(CComputeShader::BUFFER_TYPE::OUTPUT, 0, m_pReadSource);

	// 이건 붙여넣기임
	m_pVIBufferCom->PasteResource(m_pReadSource);
}

CParticle* CParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle* pInstance = new CParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticle::Clone(void* pArg)
{
	CParticle* pInstance = new CParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pComputeShader);
	Safe_Release(m_pReadSource);
	Safe_Release(m_pShaderCom);
	for (_uint i = 0; i < 3; ++i)
		Safe_Release(m_pTexture[i]);
}
