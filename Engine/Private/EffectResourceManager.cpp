#include "EffectResourceManager.h"

#include "GameInstance.h"
#include "StringHelper.h"

CEffectResourceManager::CEffectResourceManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEffectResourceManager::Initalize()
{
	m_pGameInstance->Add_ThreadjobList([&](void* pArg) { this->LoadVIBuffer(pArg); });
	if (FAILED(LoadTexture()))
		return E_FAIL;
	while (m_pGameInstance->IsWorkThread());
	return S_OK;
}

void CEffectResourceManager::ADD_TextureResource(const WCHAR* TextureTag, CTexture* pTexture)
{
	auto iter = m_pTextures.find(TextureTag);
	if (iter != m_pTextures.end())
		return;

	m_pTextures.emplace(TextureTag, pTexture);
}

void CEffectResourceManager::ADD_ShaderResource(const WCHAR* ShaderTag, CShader* pShader)
{
	auto iter = m_pShaders.find(ShaderTag);
	if (iter != m_pShaders.end())
		return;

	m_pShaders.emplace(ShaderTag, pShader);
}

void CEffectResourceManager::ADD_VIBufferResource(const WCHAR* VIBufferTag, CComponent* pVIBuffer)
{
	auto iter = m_pVIBuffers.find(VIBufferTag);
	if (iter != m_pVIBuffers.end())
		return;

	m_pVIBuffers.emplace(VIBufferTag, pVIBuffer);
}

CTexture* CEffectResourceManager::GetTextureResource(const WCHAR* TextureTag)
{
	auto iter = m_pTextures.find(TextureTag);
	if (iter == m_pTextures.end())
		return nullptr;

	return iter->second;
}

CShader* CEffectResourceManager::GetShaderResource(const WCHAR* ShaderTag)
{
	auto iter = m_pShaders.find(ShaderTag);
	if (iter == m_pShaders.end())
		return nullptr;

	return iter->second;
}

CComponent* CEffectResourceManager::GetVIBufferResource(const WCHAR* VIBufferTag)
{
	auto iter = m_pVIBuffers.find(VIBufferTag);
	if (iter == m_pVIBuffers.end())
		return nullptr;

	return iter->second;
}

HRESULT CEffectResourceManager::LoadTexture()
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddatai64_t  fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst64("../Bin/Resources/Static/Textures/*.png*", &fd);

	if (handle == -1)
		return S_OK;

	int iResult = 0;
	_wstring FrontPath = TEXT("../Bin/Resources/Static/Textures/");
	while (iResult != -1)
	{
		int iLength = strlen(fd.name) + 1;
		WCHAR* pFileName = new WCHAR[iLength];
		ZeroMemory(pFileName, sizeof(WCHAR) * iLength);

		// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
		MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pFileName, iLength);

		_wstring FullPath = FrontPath + pFileName;
		_wstring szTextureTag = pFileName;

		auto iter = m_pTextures.find(szTextureTag);
		if (iter == m_pTextures.end())
		{
			auto pTexture = CTexture::Create(m_pDevice, m_pContext, FullPath.c_str(), 1);
			if (nullptr == pTexture)
				MSG_BOX("Create Fail : Static Resource");
			m_pTextures.emplace(szTextureTag, pTexture);
		}
		
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext64(handle, &fd);
		Safe_Delete_Array(pFileName);
	}

	_findclose(handle);
	return S_OK;
}

HRESULT CEffectResourceManager::LoadVIBuffer(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddatai64_t  fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst64("../Bin/Resources/Static/Models/*.fbx*", &fd);

	if (handle == -1)
		return S_OK;

	int iResult = 0;

	string FrontPath = "../Bin/Resources/Static/Models/";
	_matrix PreMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	while (iResult != -1)
	{
		int iLength = strlen(fd.name) + 1;
		WCHAR* pFileName = new WCHAR[iLength];
		ZeroMemory(pFileName, sizeof(WCHAR) * iLength);
		// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
		MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pFileName, iLength);

		string FullPath = FrontPath + fd.name;
		_wstring szTextureTag = pFileName;
		auto iter = m_pVIBuffers.find(szTextureTag);
		if (iter == m_pVIBuffers.end())
		{
			auto pModel = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, FullPath.c_str(), PreMatrix);
			if (nullptr == pModel)
				MSG_BOX("Create Fail : Static Resource");

			m_pVIBuffers.emplace(szTextureTag, pModel);
		}
		
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext64(handle, &fd);
		Safe_Delete_Array(pFileName);
	}

	_findclose(handle);
	Desc->OnCompleted(this_thread::get_id());
	//m_pVIBuffers.emplace(TEXT("VI_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pContext));
	return S_OK;
}

CEffectResourceManager* CEffectResourceManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffectResourceManager* pEffectManager = new CEffectResourceManager(pDevice, pContext);
	if (FAILED(pEffectManager->Initalize()))
	{
		Safe_Release(pEffectManager);
		MSG_BOX("Create Fail : Effect Manager");
	}
	return pEffectManager;
}

void CEffectResourceManager::Free()
{
	__super::Free();

	for (auto& iter : m_pTextures)
		Safe_Release(iter.second);
	m_pTextures.clear();

	for (auto& iter : m_pVIBuffers)
		Safe_Release(iter.second);
	m_pVIBuffers.clear();

	for (auto& iter : m_pShaders)
		Safe_Release(iter.second);
	m_pShaders.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
