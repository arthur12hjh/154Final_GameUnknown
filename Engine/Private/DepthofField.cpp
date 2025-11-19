#include "DepthofField.h"

#include "GameInstance.h"

CDepthofField::CDepthofField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeferred { pDevice, pContext }
{
}

void CDepthofField::Set_Active()
{
	m_isActive = !m_isActive;
}

HRESULT CDepthofField::Initialize()
{
	/* 셰이더 파일 로딩 */
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_DoF.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	/* 스크린 사이즈는 미리 바인딩 한다. */
	_uint2 vScreenSize = m_pGameInstance->GetScreenSize();
	if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vScreenSize.x, sizeof(_int))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vScreenSize.y, sizeof(_int))))
		return E_FAIL;

	/* 블러 X 처리 */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ScreenBlurX"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* MRT_ScreenBlurX */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SreenBlurX"), TEXT("Target_ScreenBlurX"))))
		return E_FAIL;

	/* DoF. 기존 화면과 블러 처리된 화면에 대해서 럴프 처리해준다. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DoF"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* MRT_DoF */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DoF"), TEXT("Target_DoF"))))
		return E_FAIL;

	// 위 과정들이 다 끝났으면 Combine 해주는 과정이 있어야한다.

	return S_OK;
}

HRESULT CDepthofField::Render(CVIBuffer_Rect* pVIBuffer, const _wstring& strRTTag, const _wstring& strDepthRTTag, const _wstring& strReturnRTTag)
{
	//blur X 처리
	if (false == m_isActive)
		return S_OK;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SreenBlurX"))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Bind_RenderTarget(strRTTag, m_pShader, "g_SceneTexture")))
		return E_FAIL;

	m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
	m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

	m_pShader->Begin(0);
	pVIBuffer->Bind_Resources();
	pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DoF"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_ScreenBlurX"), m_pShader, "g_SceneBlurXTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(strDepthRTTag, m_pShader, "g_DepthTexture")))
		return E_FAIL;

	m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
	m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

	m_pShader->Begin(1);
	pVIBuffer->Bind_Resources();
	pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	// DOF 처리 반환
	if (FAILED(m_pGameInstance->Begin_MRT(strReturnRTTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_DoF"), m_pShader, "g_SceneTexture")))
		return E_FAIL;

	m_pShader->Begin(2);
	pVIBuffer->Bind_Resources();
	pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

CDepthofField* CDepthofField::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDepthofField* pInstance = new CDepthofField(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Create failed : CDepthofField");
	}

	return pInstance;
}

void CDepthofField::Free()
{
	__super::Free();
}
