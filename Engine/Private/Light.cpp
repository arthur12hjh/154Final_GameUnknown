#include "Light.h"
#include "Shader.h"

#include "VIBuffer.h"
#include "SphereCollider.h"
#include "OBBCollider.h"

#ifdef _DEBUG
#include "GameInstance.h"

CLight::CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}
#elif
CLight::CLight()
{

}
#endif // _DEBUG


HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
    m_LightDesc = LightDesc;

#ifdef _DEBUG
	_matrix WorldMat = XMMatrixIdentity();
	if (LIGHT_TYPE::DIRECTIONAL == m_LightDesc.eType)
	{
		WorldMat.r[0] = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat4(&m_LightDesc.vDirection)));
		WorldMat.r[1] = XMVector3Normalize(XMVector3Cross(XMLoadFloat4(&m_LightDesc.vDirection), WorldMat.r[0]));
		WorldMat.r[2] = XMLoadFloat4(&m_LightDesc.vDirection);
	}
	else if (LIGHT_TYPE::SPOT == m_LightDesc.eType)
	{

	}

	m_LightDesc.vPosition.w = 1.f;
	WorldMat.r[3] = XMLoadFloat4(&m_LightDesc.vPosition);
	XMStoreFloat4x4(&m_WorldMat, WorldMat);

	if(FAILED(CreateDebugCollider()))
		return E_FAIL;
	
	

#endif // _DEBUG

    return S_OK;
}

void CLight::SetDead(_bool bIsDead)
{
	m_bIsDead = bIsDead;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer* pVIBuffer)
{
	_uint			iPassIndex = { 0 };

	if (VISIBILITY::HIDDEN == m_eVisible)
		return S_OK;

	if (LIGHT_TYPE::DIRECTIONAL == m_LightDesc.eType)
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;
		iPassIndex = 1;
	}
	else
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;
		iPassIndex = 2;
	}

	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(pShader->Begin(iPassIndex)))
		return E_FAIL;

	return pVIBuffer->Render();
}

void CLight::SetVisibility(VISIBILITY eVisibility)
{
	m_eVisible = eVisibility;
}

#ifdef _DEBUG
void CLight::Debug_Render()
{
	m_pCollider->UpdateColiision(XMLoadFloat4x4(&m_WorldMat));
	m_pGameInstance->Add_DebugComponent(m_pCollider);
}
#endif // _DEBUG

#ifdef _DEBUG
HRESULT CLight::CreateDebugCollider()
{
	_float3 vPosition = {};
	memcpy(&vPosition, &m_LightDesc.vPosition, sizeof(_float3));

	if (LIGHT_TYPE::DIRECTIONAL == m_LightDesc.eType)
	{
		
	}
	else if (LIGHT_TYPE::SPOT == m_LightDesc.eType)
	{

	}
	else
	{
		m_pCollider = CSphereCollider::Create(m_pDevice, m_pContext);
		if (nullptr == m_pCollider)
			return E_FAIL;

		CSphereCollider::SPHERE_COLLIDER_DESC SphereDesc = {};
		SphereDesc.fRadius = m_LightDesc.fRange;
		static_cast<CSphereCollider*>(m_pCollider)->Initialize(&SphereDesc);
		
	}
	
	if(m_pCollider)
		m_pCollider->UpdateColiision(XMLoadFloat4x4(&m_WorldMat));
	return S_OK;
}
CLight* CLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc)
{
	CLight* pInstance = new CLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Created : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}
#elif
CLight* CLight::Create(const LIGHT_DESC& LightDesc)
{
	CLight* pInstance = new CLight();

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Created : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}
#endif // _DEBUG

void CLight::Free()
{
    __super::Free();

#ifdef _DEBUG
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pCollider);
	Safe_Release(m_pGameInstance);
#endif

}
