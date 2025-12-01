#include "Material.h"
#include "Shader.h"
#include "GameInstance.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMaterial::Initialize(const _char* pModelFilePath, const binMaterial* pBinMaterial)
{	
	for (size_t i = 0; i < BINMATERIAL::TEXTURETYPE::END; i++)
	{
		m_iNumSRVs = pBinMaterial->vNumSRVs[i];

		m_SRVs[i].reserve(m_iNumSRVs);

		for (size_t j = 0; j < m_iNumSRVs; j++)
		{
			/* pModelFilePath : 모델파일이 저장되어있는 경로 + 모델파일이름 + 모델파일확장자. */
			/* 추출한파일의경로 + 파일이름 + 확장자 */
			char	strTexturePath[MAX_PATH];

			char		szDrive[MAX_PATH] = {};
			char		szDir[MAX_PATH] = {};
			char		szFileName[MAX_PATH] = {};
			char		szEXT[MAX_PATH] = {};

			strcpy_s(strTexturePath, pBinMaterial->strTexturePaths[i][j].c_str());

			_char		szTextureFilePath[MAX_PATH] = {};
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strTexturePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			strcpy_s(szTextureFilePath, szDrive);
			strcat_s(szTextureFilePath, szDir);
			strcat_s(szTextureFilePath, szFileName);
			strcat_s(szTextureFilePath, szEXT);

			_tchar		szAbsolutePath[MAX_PATH] = {};
			MultiByteToWideChar(CP_ACP, 0, szTextureFilePath, strlen(szTextureFilePath),
				szAbsolutePath, MAX_PATH);

			ID3D11ShaderResourceView* pSRV = { nullptr };

			HRESULT			hr = {};

			if (false == strcmp(".dds", szEXT))
				hr = CreateDDSTextureFromFile(m_pDevice, szAbsolutePath, nullptr, &pSRV);
			else if (false == strcmp(".tga", szEXT))
				hr = S_OK;
			else
				hr = CreateWICTextureFromFile(m_pDevice, szAbsolutePath, nullptr, &pSRV);

			if (FAILED(hr))
				return E_FAIL;

			m_SRVs[i].push_back(pSRV);

		}

	}

	if (pBinMaterial->szName[0] != '\0')
		strcpy_s(m_szName, pBinMaterial->szName);

	return S_OK;
}

HRESULT CMaterial::Bind_SRV(CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex)
{
	if (m_SRVs[eType].size() > iTextureIndex)
		return pShader->Bind_SRV(pConstantName, m_SRVs[eType][iTextureIndex]);
	else
	{
		return BindDefaultTexture(pShader, eType);
	}

	return E_FAIL;
}

HRESULT CMaterial::Import_Texture(aiTextureType eType, ID3D11ShaderResourceView* pSRV)
{
	if(pSRV == nullptr)
		return E_FAIL;

	m_SRVs[eType].push_back(pSRV);

	return S_OK;
}

HRESULT CMaterial::BindDefaultTexture(CShader* pShader, _uint eType)
{
	switch (static_cast<binMaterial::TEXTURETYPE>(eType))
	{
	case binMaterial::NONE:
		return S_OK;
		break;
	case binMaterial::DIFFUSE:
		m_pGameInstance->Get_ResourceManagerTextureResource(TEXT("Default_Opacity.png"))->Bind_ShaderResource(pShader, "g_DiffuseTexture", 0);
		return S_OK;
		break;
	case binMaterial::SPECULAR:
		return S_OK;
		break;
	case binMaterial::AMBIENT:
		return S_OK;
		break;
	case binMaterial::EMISSIVE:
		m_pGameInstance->Get_ResourceManagerTextureResource(TEXT("Default_Emissive.png"))->Bind_ShaderResource(pShader, "g_EmissiveTexture", 0);
		break;
	case binMaterial::HEIGHT:
		return S_OK;
		break;
	case binMaterial::NORMALS:
		m_pGameInstance->Get_ResourceManagerTextureResource(TEXT("Default_Normal.png"))->Bind_ShaderResource(pShader, "g_NormalTexture", 0);
		break;
	case binMaterial::SHININESS:
		return S_OK;
		break;
	case binMaterial::OPACITY:
		m_pGameInstance->Get_ResourceManagerTextureResource(TEXT("Default_Opacity.png"))->Bind_ShaderResource(pShader, "g_OpacityTexture", 0);
		return S_OK;
		break;
	case binMaterial::DISPLACEMENT:
		return S_OK;
		break;
	case binMaterial::LIGHTMAP:
		return S_OK;
		break;
	case binMaterial::REFLECTION:
		return S_OK;
		break;
	case binMaterial::BASE_COLOR:
		return S_OK;
		break;
	case binMaterial::NORMAL_CAMERA:
		return S_OK;
		break;
	case binMaterial::EMISSIVE_COLOR:
		return S_OK;
		break;
	case binMaterial::METALNESS:
		m_pGameInstance->Get_ResourceManagerTextureResource(TEXT("Default_ORM.png"))->Bind_ShaderResource(pShader, "g_ORMTexture", 0);
		return S_OK;
		break;
	case binMaterial::DIFFUSE_ROUGHNESS:
		return S_OK;
		break;
	case binMaterial::AMBIENT_OCCLUSION:
		return S_OK;
		break;
	case binMaterial::UNKNOWN:
		return S_OK;
		break;
	case binMaterial::SHEEN:
		return S_OK;
		break;
	case binMaterial::CLEARCOAT:
		m_pGameInstance->Get_ResourceManagerTextureResource(TEXT("Default_ORM.png"))->Bind_ShaderResource(pShader, "g_ORSSTexture", 0);
		return S_OK;
		break;
	case binMaterial::TRANSMISSION:
		return S_OK;
		break;
	case binMaterial::MAYA_BASE:
		return S_OK;
		break;
	case binMaterial::MAYA_SPECULAR:
		return S_OK;
		break;
	case binMaterial::MAYA_SPECULAR_COLOR:
		return S_OK;
		break;
	case binMaterial::MAYA_SPECULAR_ROUGHNESS:
		return S_OK;
		break;
	case binMaterial::ANISOTROPY:
		return S_OK;
		break;
	case binMaterial::GLTF_METALLIC_ROUGHNESS:
		return S_OK;
		break;
	case binMaterial::END:
		return S_OK;
		break;
	default:
		break;
	}

	return S_OK;
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const binMaterial* pBinMaterial)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pModelFilePath, pBinMaterial)))
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
	Safe_Release(m_pGameInstance);

	for (auto& SRVs : m_SRVs)
	{
		for (auto& pSRV : SRVs)
			Safe_Release(pSRV);
		SRVs.clear();
	}
}
