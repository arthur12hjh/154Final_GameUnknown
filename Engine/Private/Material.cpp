#include "Material.h"

#include "Shader.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CMaterial::Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{	
	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		m_iNumSRVs = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i));

		m_SRVs[i].reserve(m_iNumSRVs);

		for (size_t j = 0; j < m_iNumSRVs; j++)
		{
			/*pModelFilePath : 모델파일이 저장되어있느 ㄴㄱ경로 + 모델파일이름 + 모델파일확장자. */
			/* 추출한파일의경로 + 파일이름 + 확장자 */
			aiString	strTexturePath;

		
			char		szDrive[MAX_PATH] = {};
			char		szDir[MAX_PATH] = {};
			char		szFileName[MAX_PATH] = {};
			char		szEXT[MAX_PATH] = {};			

			if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(i), j, &strTexturePath)))
				continue;

			_char		szTextureFilePath[MAX_PATH] = {};
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			strcpy_s(szTextureFilePath, szDrive);
			strcat_s(szTextureFilePath, szDir);
			strcat_s(szTextureFilePath, szFileName);
			strcat_s(szTextureFilePath, szEXT);

			_tchar		szPerfectPath[MAX_PATH] = {};
			MultiByteToWideChar(CP_ACP, 0, szTextureFilePath, strlen(szTextureFilePath),
				szPerfectPath, MAX_PATH);

			ID3D11ShaderResourceView* pSRV = { nullptr };

			HRESULT			hr = {};

			if (false == strcmp(".dds", szEXT))
				hr = CreateDDSTextureFromFile(m_pDevice, szPerfectPath, nullptr, &pSRV);
			else if (false == strcmp(".tga", szEXT))
				hr = S_OK;
			else
				hr = CreateWICTextureFromFile(m_pDevice, szPerfectPath, nullptr, &pSRV);

			if(FAILED(hr))
				return E_FAIL;

			m_SRVs[i].push_back(pSRV);
		}
	}

	return S_OK;
}

HRESULT CMaterial::Bind_SRV(CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex)
{
	return pShader->Bind_SRV(pConstantName, m_SRVs[eType][iTextureIndex]);
	
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pModelFilePath, pAIMaterial)))
	{
		MSG_BOX("Failed to Created : CMaterial");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CMaterial::Free()
{
    __super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& SRVs : m_SRVs)
	{
		for (auto& pSRV : SRVs)
			Safe_Release(pSRV);
		SRVs.clear();
	}
}
