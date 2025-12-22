#include "Blur.h"

#include "GameInstance.h"
#include "GameObject.h"

CBlur::CBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeferred { pDevice, pContext }
{
}

HRESULT CBlur::Initialize()
{
	/* 셰이더 파일 로딩 */
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_Blur.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	/* 스크린 사이즈는 미리 바인딩 한다. */
	_uint2 vScreenSize = m_pGameInstance->GetScreenSize();
	if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vScreenSize.x, sizeof(_int))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vScreenSize.y, sizeof(_int))))
		return E_FAIL;

	/* Target_Blur. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur"), vScreenSize.x * 0.5f, vScreenSize.y * 0.5f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_Blur_Texture. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Texture"), vScreenSize.x * 0.5f, vScreenSize.y * 0.5f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_Blur_X. X에 대해서 우선 블러처리. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_X"), vScreenSize.x * 0.5f, vScreenSize.y * 0.5f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;
	/* Target_Blur_Final. Y에 대해서도 블러처리 수행. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Final"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* MRT_Blur */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur"), TEXT("Target_Blur"))))
		return E_FAIL;
	/* MRT_Blur_Weight */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Weight"), TEXT("Target_Blur_Texture"))))
		return E_FAIL;
	/* MRT_Blur_X */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X"))))
		return E_FAIL;
	/* MRT_Blur_Final */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Final"), TEXT("Target_Blur_Final"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlur::Add_RenderObject(CGameObject* pRenderObject)
{
	if (nullptr == pRenderObject)
		return E_FAIL;

	m_BlurObjects.push_back(pRenderObject);
	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CBlur::Render(CVIBuffer_Rect* pVIBuffer)
{
	/* 블러 기록할 물체들만 뺴서 기록 */
	if (0 < m_BlurObjects.size()) {
		_uint2 vScreenSize = m_pGameInstance->GetScreenSize();
		m_pGameInstance->Set_ScreenSize(vScreenSize.x * 0.5f, vScreenSize.y * 0.5f);

		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur"))))
			return E_FAIL;

		for (auto& pRenderObject : m_BlurObjects)
		{
			if (nullptr != pRenderObject)
				pRenderObject->Render();

			Safe_Release(pRenderObject);
		}

		m_BlurObjects.clear();

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;

		/* 웨이트 처리 */
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Weight"))))
			return E_FAIL;

		m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
		m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
		m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur"), m_pShader, "g_BlurTexture")))
			return E_FAIL;

		m_pShader->Begin(0);
		pVIBuffer->Bind_Resources();
		pVIBuffer->Render();
		
		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;

		/* 블러 X 처리 */
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_X"))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur_Texture"), m_pShader, "g_BlurTexture")))
			return E_FAIL;

		m_pShader->Begin(1);
		pVIBuffer->Bind_Resources();
		pVIBuffer->Render();

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;

		m_pGameInstance->Set_ScreenSize(vScreenSize.x, vScreenSize.y);

		/* 블러 Y 처리 */
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Final"))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur_X"), m_pShader, "g_BlurTexture")))
			return E_FAIL;
		
		m_pShader->Begin(2);

		pVIBuffer->Bind_Resources();

		pVIBuffer->Render();

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;
	}
	else {
		m_pGameInstance->Clear_MRT(TEXT("MRT_Blur_Final"));
	}
	return S_OK;
}

HRESULT CBlur::Bind_RenderTarget(CShader* pShader, const _char* pConstantName)
{
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur_Final"), pShader, pConstantName)))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG

HRESULT CBlur::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Blur_Final"), fX, fY, fSizeX, fSizeY)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlur::Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader)
{
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Blur_Final"), pShader, pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

#endif

CBlur* CBlur::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBlur* pInstance = new CBlur(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Create Failed : Blur");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlur::Free()
{
	__super::Free();

	for (auto& BlurObject : m_BlurObjects)
	{
		Safe_Release(BlurObject);
	}

	m_BlurObjects.clear();
}
