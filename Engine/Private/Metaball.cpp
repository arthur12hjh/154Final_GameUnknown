#include "Metaball.h"

#include "GameInstance.h"
#include "GameObject.h"

CMetaball::CMetaball(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeferred{ pDevice, pContext }
{
}

HRESULT CMetaball::Initialize()
{
	/* 셰이더 파일 로딩 */
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_Metaball.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	/* 스크린 사이즈는 미리 바인딩 한다. */
	_uint2 vScreenSize = m_pGameInstance->GetScreenSize();
	if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vScreenSize.x, sizeof(_int))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vScreenSize.y, sizeof(_int))))
		return E_FAIL;

	/* Target_Metaball. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Metaball"), vScreenSize.x * 0.5f, vScreenSize.y * 0.5f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_Metaball_Texture. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Metaball_Texture"), vScreenSize.x * 0.5f, vScreenSize.y * 0.5f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_Metaball_X. X에 대해서 우선 블러처리. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Metaball_X"), vScreenSize.x * 0.5f, vScreenSize.y * 0.5f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_Metaball_Final. Y에 대해서도 블러처리 수행. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Metaball_Final"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* MRT_Metaball */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Metaball"), TEXT("Target_Metaball"))))
		return E_FAIL;
	/* MRT_Metaball_Weight */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Metaball_Weight"), TEXT("Target_Metaball_Texture"))))
		return E_FAIL;
	/* MRT_Metaball_X */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Metaball_X"), TEXT("Target_Metaball_X"))))
		return E_FAIL;
	/* MRT_Metaball_Final */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Metaball_Final"), TEXT("Target_Metaball_Final"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMetaball::Add_RenderObject(CGameObject* pRenderObject)
{
	if (nullptr == pRenderObject)
		return E_FAIL;

	m_MetaballObjects.push_back(pRenderObject);
	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CMetaball::Render(CVIBuffer_Rect* pVIBuffer)
{
	/* 블러 기록할 물체들만 뺴서 기록 */
	if (0 < m_MetaballObjects.size()) {
		_uint2 vScreenSize = m_pGameInstance->GetScreenSize();
		m_pGameInstance->Set_ScreenSize(vScreenSize.x * 0.5f, vScreenSize.y * 0.5f);

		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Metaball"))))
			return E_FAIL;

		for (auto& pRenderObject : m_MetaballObjects)
		{
			if (nullptr != pRenderObject)
				pRenderObject->Render();

			Safe_Release(pRenderObject);
		}

		m_MetaballObjects.clear();

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;

		/* 웨이트 처리 */
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Metaball_Weight"))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Metaball"), m_pShader, "g_MetaballTexture")))
			return E_FAIL;

		m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
		m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
		m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

		m_pShader->Begin(0);
		pVIBuffer->Bind_Resources();
		pVIBuffer->Render();

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;

		/* 블러 X 처리 */
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Metaball_X"))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Metaball_Texture"), m_pShader, "g_MetaballTexture")))
			return E_FAIL;

		m_pShader->Begin(1);
		pVIBuffer->Bind_Resources();
		pVIBuffer->Render();

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;

		m_pGameInstance->Set_ScreenSize(vScreenSize.x, vScreenSize.y);

		/* 블러 Y 처리 */
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Metaball_Final"))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Metaball_X"), m_pShader, "g_MetaballTexture")))
			return E_FAIL;

		m_pShader->Begin(2);

		pVIBuffer->Bind_Resources();

		pVIBuffer->Render();

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;
	}
	else {
		m_pGameInstance->Clear_MRT(TEXT("MRT_Metaball_Final"));
	}
	return S_OK;
}

HRESULT CMetaball::Bind_RenderTarget(CShader* pShader, const _char* pConstantName)
{
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Metaball_Final"), pShader, pConstantName)))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG

HRESULT CMetaball::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Metaball_Final"), fX, fY, fSizeX, fSizeY)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMetaball::Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader)
{
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Metaball_Final"), pShader, pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

#endif

CMetaball* CMetaball::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMetaball* pInstance = new CMetaball(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Create Failed : Metaball");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMetaball::Free()
{
	__super::Free();

	for (auto& MetaballObject : m_MetaballObjects)
	{
		Safe_Release(MetaballObject);
	}
	m_MetaballObjects.clear();
}
