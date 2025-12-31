#include "pch.h"
#include "RimLight.h"

#include "GameInstance.h"

CRimLight::CRimLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{pDevice, pContext}
{
}

HRESULT CRimLight::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRimLight::Initialize(void* pArg)
{
	RIMLIGHT_DESC* pDesc = static_cast<RIMLIGHT_DESC*>(pArg);

	memcpy(&m_tRimLightDesc, pDesc, sizeof(RIMLIGHT_DESC));

	return S_OK;
}

HRESULT CRimLight::Bind_RimLightShaderResources(CShader* pShader, const _char* pRimColorTag, const _char* pRimPowerTag, const _char* pRimLightIntensity, const _char* pCamPositionTag)
{
	if (nullptr == pShader)
		return E_FAIL;

	if(nullptr != pRimColorTag)
		pShader->Bind_RawValue(pRimColorTag, &m_tRimLightDesc.vRimLightColor, sizeof(_float4));

	if (nullptr != pRimPowerTag)
		pShader->Bind_RawValue(pRimPowerTag, &m_tRimLightDesc.fRimLightPower, sizeof(_float));

	if (nullptr != pRimLightIntensity)
		pShader->Bind_RawValue(pRimLightIntensity, &m_tRimLightDesc.fRimLightIntensity, sizeof(_float));

	if (nullptr != pCamPositionTag)
		pShader->Bind_RawValue(pCamPositionTag, m_pGameInstance->Get_CamPosition(), sizeof(_float4));

	return S_OK;
}

CRimLight* CRimLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRimLight* pInstance = new CRimLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to create : CRimLight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CRimLight::Clone(void* pArg)
{
	CRimLight* pInstance = new CRimLight(*this);

	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRimLight");
		Safe_Release(pInstance);
	}	
	return pInstance;
}

void CRimLight::Free()
{
	__super::Free();
}
