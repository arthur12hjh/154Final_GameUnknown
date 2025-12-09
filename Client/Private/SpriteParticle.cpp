#include "pch.h"
#include "SpriteParticle.h"

#include "GameInstance.h"

CSpriteParticle::CSpriteParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CSpriteParticle::CSpriteParticle(const CSpriteParticle& Prototype)
	: CGameObject{ Prototype },
	m_tData{ Prototype.m_tData },
	m_eRender{ Prototype.m_eRender }
{
	m_eTeam = Prototype.m_eTeam;
	m_pVIBufferCom = dynamic_cast<CVIBuffer_Point_Instance*>(Prototype.m_pVIBufferCom->Clone(nullptr));
	m_pComputeShader = dynamic_cast<CComputeShader*>(Prototype.m_pComputeShader->Clone(nullptr));
}

HRESULT CSpriteParticle::Initialize_Prototype(const SPRITE_PARTICLE_DATA* pPointParticleData)
{
	m_tData = *pPointParticleData;
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
	case 8:
		m_eRender = RENDER::BLUR;
		m_eTeam = OBJECT_TEAM::ENEMY;
		break;
	}
	CVIBuffer_Point_Instance::POINT_INSTANCE_DESC		Desc{};
	Desc.iNumInstance = pPointParticleData->iNumInstance;
	Desc.vCenter = pPointParticleData->fCenter;
	Desc.vPivot = pPointParticleData->fPivot;
	Desc.vRange = pPointParticleData->fRange;
	Desc.vSize = pPointParticleData->fSize;
	Desc.vLifeTime = pPointParticleData->fLifeTime;
	Desc.vSpeed = pPointParticleData->fSpeed;
	Desc.isLoop = pPointParticleData->bisLoop;
	m_pVIBufferCom = CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &Desc);
	m_pComputeShader = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_Spread.hlsl"), pPointParticleData->szCS.c_str(), Desc.iNumInstance);
	return S_OK;
}

HRESULT CSpriteParticle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	m_fTime = -m_tData.fDelayTime;
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_tData.fPosition));
	m_pTransformCom->Rotation(XMConvertToRadians(m_tData.fRotation.x), XMConvertToRadians(m_tData.fRotation.y), XMConvertToRadians(m_tData.fRotation.z));

	if (FAILED(Ready_Components()))
		return E_FAIL;


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
		return E_FAIL;



	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.Buffer.FirstElement = 0;
	SRVDesc.Buffer.NumElements = m_tData.fSizeDiagrams.size();
	if (FAILED(m_pDevice->CreateShaderResourceView(pBuffer, &SRVDesc, &m_pSizeDiagramSRV)))
		return E_FAIL;

	Safe_Release(pBuffer);

	if (FAILED(Ready_ComputeShader()))
		return E_FAIL;
	return S_OK;
}

void CSpriteParticle::Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta;
	if (m_tData.fDelayTime > m_fTime)
	{
		return;
	}
	else if (0 < m_tData.fEndTime && m_tData.fEndTime <= m_fTime) {
		m_tData.bisLoop = false;
	}
	if (!m_tData.bisLoop && m_tData.fEndTime + m_tData.fLifeTime.y + 1.f <= m_fTime) {
		m_isDead = true;
		return;
	}
	_float4x4 CombinedWorldMatrix;
	XMStoreFloat4x4(&CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr())* XMLoadFloat4x4(m_pParentMat));

	if (m_tData.bisSpectrum) {
		if (0 < XMVectorGetX(XMVector4Length(XMLoadFloat4(reinterpret_cast<_float4*>(&CombinedWorldMatrix.m[0]))))) {
			m_CBData.fTimeDelta.z = m_CBData.fTimeDelta.w;
			if (2 <= m_CBData.iLoopAndCount.x && 4 > m_CBData.iLoopAndCount.x) {
				m_CBData.fTimeDelta.w = fmodf(m_CBData.fTimeDelta.w + 1, m_tData.iNumInstance);
			}
			else {
				m_fLength += XMVectorGetX(XMVector4Length(XMLoadFloat4(reinterpret_cast<_float4*>(&CombinedWorldMatrix.m[3])) - XMLoadFloat4(reinterpret_cast<_float4*>(&m_CombinedWorldMatrix.m[3])))) / XMVectorGetX(XMVector4Length(XMLoadFloat4(reinterpret_cast<_float4*>(&CombinedWorldMatrix.m[0])))) / m_tData.fSphereSize;
				if (1 < m_fLength) {
					_int iLength = (_int)m_fLength;
					m_fLength -= iLength;
					m_CBData.fTimeDelta.w = fmodf(m_CBData.fTimeDelta.w, m_tData.iNumInstance) + iLength;
				}
			}
		}
	}
	m_CombinedWorldMatrix = CombinedWorldMatrix;
	Spread(fTimeDelta);
}

void CSpriteParticle::Late_Update(_float fTimeDelta)
{
	if (!m_tData.bisLoop && m_tData.fEndTime + m_tData.fLifeTime.y + 1.f <= m_fTime) {
		return;
	}
	m_pGameInstance->Add_RenderGroup(m_eRender, this);
}

HRESULT CSpriteParticle::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_tData.iBegin);

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}

void CSpriteParticle::Stop() {
	if (!m_bisStop && !m_tData.bisSpectrum) {
		m_CBData.fTimeDelta.w = m_CBData.fTimeDelta.y;
	}
	m_bisStop = true;
}

void CSpriteParticle::Play()
{
	if (m_bisStop && m_tData.bisSpectrum) {
		m_CBData.iLoopAndCount.x = 2;
	}
	m_bisStop = false;
}

void CSpriteParticle::End()
{
	m_tData.fEndTime = m_fTime;
}


HRESULT CSpriteParticle::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxSpriteParticle"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	return S_OK;
}

HRESULT CSpriteParticle::Bind_ShaderResources()
{
	if (m_tData.bisSpectrum) {
		_float4x4 world = m_CombinedWorldMatrix;
		world._41 = 0;
		world._42 = 0;
		world._43 = 0;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &world)))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
			return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_CamMatrix", m_pGameInstance->GetMainCameraWorldMatrixPtr())))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_tData.fColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSize", &m_tData.fParticleSize, sizeof(_float2))))
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

	if (FAILED(m_pTexture[2]->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iUV", &m_tData.iUV, sizeof(_int2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAngle", &m_tData.fAngle, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bisBillboard", &m_tData.bisBillboard, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bisSpectrum", &m_tData.bisSpectrum, sizeof(_bool))))
		return E_FAIL;
	int iSizeCount = m_tData.fSizeDiagrams.size();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iSizeCount", &iSizeCount, sizeof(_int))))
		return E_FAIL;

	m_pShaderCom->Bind_SRV("g_fSizeDiagram", m_pSizeDiagramSRV);
	return S_OK;
}

HRESULT CSpriteParticle::Ready_ComputeShader()
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
	m_CBData.vPivot = { m_tData.fPivot.x,  m_tData.fPivot.y, m_tData.fPivot.z, m_tData.bisSpectrum ? 0.f : 1.f };
	m_CBData.fTurnPower = m_tData.fTurnPower;
	m_CBData.fisSphere.x = m_tData.bisSphere ? 1 : m_tData.bisCircle ? 2 : 0;
	m_CBData.fisSphere.y = m_tData.fSphereSize;
	m_CBData.fCircle = m_tData.fCircle;
	m_CBData.iLoopAndCount.x = m_bisStop ? 4 : m_tData.bisLoop ? m_tData.bisSpectrum ? (2 == m_CBData.iLoopAndCount.x || 3 == m_CBData.iLoopAndCount.x) ? 3 : 2 : 1 : 0;
	m_CBData.iLoopAndCount.y = iNumData;
	m_CBData.fTimeDelta.z = 0;
	m_CBData.fTimeDelta.w = 0;

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

void CSpriteParticle::Spread(_float fTimeDelta)
{
	m_CBData.iLoopAndCount.x = m_bisStop ? 5 : m_tData.bisLoop ? m_tData.bisSpectrum ? (4 > m_CBData.iLoopAndCount.x) ? m_CBData.iLoopAndCount.x + 1 : 4 : 1 : 0;
	m_CBData.fTimeDelta.x = fTimeDelta;
	m_CBData.fTimeDelta.y += fTimeDelta * m_tData.fCircleSpeed;
	m_CBData.matWorld = m_CombinedWorldMatrix;
	m_CBData.fTurnPower = m_tData.fTurnPower;
	m_CBData.fisSphere.x = m_tData.bisSphere ? 1 : m_tData.bisCircle ? 2 : 0;
	m_CBData.fisSphere.y = m_tData.fSphereSize;
	m_CBData.fCircle = m_tData.fCircle;
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

CSpriteParticle* CSpriteParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const SPRITE_PARTICLE_DATA* pPointParticleData)
{
	CSpriteParticle* pInstance = new CSpriteParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pPointParticleData)))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpriteParticle::Clone(void* pArg)
{
	CSpriteParticle* pInstance = new CSpriteParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpriteParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpriteParticle::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pComputeShader);
	Safe_Release(m_pReadSource);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pSizeDiagramSRV);

	for (_uint i = 0; i < 3; ++i)
		Safe_Release(m_pTexture[i]);
}
