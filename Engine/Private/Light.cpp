#include "Light.h"
#include "Shader.h"
#include "VIBuffer.h"
#include "SphereCollider.h"

CLight::CLight()
{
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
    m_LightDesc = LightDesc;

#ifdef _DEBUG
	_matrix WorldMat = XMMatrixIdentity();
	if (LIGHT_TYPE::DIRECTIONAL == m_LightDesc.eType)
	{
		WorldMat.r[0] = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMLoadFloat4(&m_LightDesc.vDirection)));
		WorldMat.r[1] = XMVector3Normalize(XMVector3Cross(XMLoadFloat4(&m_LightDesc.vDirection), WorldMat.r[0]));
		WorldMat.r[2] = XMLoadFloat4(&m_LightDesc.vDirection);
	}
	else if (LIGHT_TYPE::SPOT == m_LightDesc.eType)
	{

	}

	WorldMat.r[3] = XMLoadFloat4(&m_LightDesc.vPosition);
	XMStoreFloat4x4(&m_WorldMat, WorldMat);
#endif // _DEBUG

    return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer* pVIBuffer)
{
	_uint			iPassIndex = { 0 };

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

#ifdef _DEBUG
void CLight::Debug_Render()
{
	m_pCollier->UpdateColiision(XMLoadFloat4x4(&m_WorldMat));
	m_pCollier->Render();
}
#endif // _DEBUG

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

void CLight::Free()
{
    __super::Free();


}
