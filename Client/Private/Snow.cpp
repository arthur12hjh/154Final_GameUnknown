#include "pch.h"
#include "Snow.h"

#include "GameInstance.h"

CSnow::CSnow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CSnow::CSnow(const CSnow& Prototype) 
	: CGameObject { Prototype }
{
}

HRESULT CSnow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSnow::Initialize(void* pArg)
{		
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_ComputeShader()))
		return E_FAIL;

	return S_OK;
}

void CSnow::Priority_Update(_float fTimeDelta)
{

}

void CSnow::Update(_float fTimeDelta)
{
	Drop(fTimeDelta);
}

void CSnow::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);
}

HRESULT CSnow::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;	

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CSnow::Ready_Components()
{
	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Snow"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Particle_Snow"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxRectParticle"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSnow::Ready_ComputeShader()
{
	D3D11_MAPPED_SUBRESOURCE pIntanceData = {};
	m_pVIBufferCom->Lock(D3D11_MAP_WRITE_NO_OVERWRITE, &pIntanceData);

	/* ComputeShader_Snow */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_ComputeShader_Snow"),
		TEXT("Com_ComputeShader"), reinterpret_cast<CComponent**>(&m_pComputeShader))))
		return E_FAIL;

	_uint iNumData = m_pComputeShader->GetNumData();

	//m_CBData.fPivot = {};
	m_CBData.iLoopAndCount.x = m_pVIBufferCom->IsLoop() ? 1 : 0;
	m_CBData.iLoopAndCount.y = iNumData;

	ID3D11Buffer* pBuffer = nullptr;

#pragma region Constant Buffer
	D3D11_BUFFER_DESC BufferDesc = {};
	BufferDesc.ByteWidth = (sizeof(RectConstBufferData) + 15) / 16 * 16;
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
	TrialInitBufferDesc.ByteWidth = sizeof(CVIBuffer_Point_Instance::VTX_INSTANCE_POINT_PARTICLE) * iNumData;
	TrialInitBufferDesc.StructureByteStride = sizeof(CVIBuffer_Point_Instance::VTX_INSTANCE_POINT_PARTICLE);
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
	ReadBufferDesc.ByteWidth = sizeof(CVIBuffer_Point_Instance::VTX_INSTANCE_POINT_PARTICLE) * iNumData;
	ReadBufferDesc.StructureByteStride = sizeof(CVIBuffer_Point_Instance::VTX_INSTANCE_POINT_PARTICLE);
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

HRESULT CSnow::Bind_ShaderResources()
{
	
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;


	return S_OK;
}

void CSnow::Drop(_float fTimeDelta)
{
	m_CBData.iLoopAndCount.x = m_pVIBufferCom->IsLoop() ? 1 : 0;
	m_CBData.fTimeDelta.x = fTimeDelta;

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

CSnow* CSnow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSnow* pInstance = new CSnow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSnow::Clone(void* pArg)
{
	CSnow* pInstance = new CSnow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSnow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSnow::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pComputeShader);
	Safe_Release(m_pReadSource);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
