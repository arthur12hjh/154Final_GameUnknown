#include "pch.h"
#include "Loader.h"
#include "Sky.h"


#include "GameInstance.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY LoadingMain(void* pArg)
{
	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 1;

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	/* 세마포어, 뮤텍스, 크리티컬섹션 */

	/* 임계영역(힙, 데이터, 코드)에 접근하기위한 키를 생성한다. */
	InitializeCriticalSection(&m_CriticalSection);

	/* 실제 로딩을 수행하기위한 스레드를 생성한다. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	CoInitializeEx(nullptr, 0);

	EnterCriticalSection(&m_CriticalSection);

	HRESULT		hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL::TOOL:
		hr = Loading_For_Tool();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

void CLoader::Output()
{
	SetWindowText(g_hWnd, m_strMessage.c_str());
}

HRESULT CLoader::Loading_For_Tool()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOOL), TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");




	char pattern[MAX_PATH] = {};
	strcpy_s(pattern, MAX_PATH, "../Bin/Resources/Models/EffectMesh/*.binx");

	WIN32_FIND_DATAA fd{};
	HANDLE h = FindFirstFileA(pattern, &fd);
	if (h != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				char szFilePath[MAX_PATH] = {};
				strcpy_s(szFilePath, MAX_PATH, "../Bin/Resources/Models/EffectMesh/");
				strcat_s(szFilePath, MAX_PATH, fd.cFileName);
				_tchar fileName[256] = { 0, };
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, fd.cFileName, strlen(fd.cFileName), fileName, 256);
				_matrix PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
				if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOOL), fileName,
					CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, szFilePath, PreTransformMatrix))))
					return E_FAIL;

			}
		} while (FindNextFileA(h, &fd));
		FindClose(h);
	}

	memset(pattern, 0, sizeof(pattern));
	strcpy_s(pattern, MAX_PATH, "../Bin/Resources/Textures/MaskTexture/*.dds");

	h = FindFirstFileA(pattern, &fd);
	if (h != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				char szFilePath[MAX_PATH] = {};
				char szProtoName[MAX_PATH] = {};
				strcpy_s(szFilePath, MAX_PATH, "../Bin/Resources/Textures/MaskTexture/");
				strcat_s(szFilePath, MAX_PATH, fd.cFileName);
				strcat_s(szProtoName, MAX_PATH, "Prototype_Component_Texture_Mask_");
				strcat_s(szProtoName, MAX_PATH, fd.cFileName);

				_tchar sztProtoName[256] = { 0, };
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szProtoName, strlen(szProtoName), sztProtoName, 256);
				_tchar szPath[256] = { 0, };
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szFilePath, strlen(szFilePath), szPath, 256);

				if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOOL), sztProtoName,
					CTexture::Create(m_pDevice, m_pContext, szPath, 1))))
					return E_FAIL;

			}
		} while (FindNextFileA(h, &fd));
		FindClose(h);
	}

	memset(pattern, 0, sizeof(pattern));
	strcpy_s(pattern, MAX_PATH, "../Bin/Resources/Textures/DiffuseTexture/*.dds");

	h = FindFirstFileA(pattern, &fd);
	if (h != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				char szFilePath[MAX_PATH] = {};
				char szProtoName[MAX_PATH] = {};
				strcpy_s(szFilePath, MAX_PATH, "../Bin/Resources/Textures/DiffuseTexture/");
				strcat_s(szFilePath, MAX_PATH, fd.cFileName);
				strcat_s(szProtoName, MAX_PATH, "Prototype_Component_Texture_Diffuse_");
				strcat_s(szProtoName, MAX_PATH, fd.cFileName);

				_tchar sztProtoName[256] = { 0, };
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szProtoName, strlen(szProtoName), sztProtoName, 256);
				_tchar szPath[256] = { 0, };
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szFilePath, strlen(szFilePath), szPath, 256);

				if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOOL), sztProtoName,
					CTexture::Create(m_pDevice, m_pContext, szPath, 1))))
					return E_FAIL;

			}
		} while (FindNextFileA(h, &fd));
		FindClose(h);
	}

	memset(pattern, 0, sizeof(pattern));
	strcpy_s(pattern, MAX_PATH, "../Bin/Resources/Textures/DissolveTexture/*.dds");

	h = FindFirstFileA(pattern, &fd);
	if (h != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				char szFilePath[MAX_PATH] = {};
				char szProtoName[MAX_PATH] = {};
				strcpy_s(szFilePath, MAX_PATH, "../Bin/Resources/Textures/DissolveTexture/");
				strcat_s(szFilePath, MAX_PATH, fd.cFileName);
				strcat_s(szProtoName, MAX_PATH, "Prototype_Component_Texture_Dissolve_");
				strcat_s(szProtoName, MAX_PATH, fd.cFileName);

				_tchar sztProtoName[256] = { 0, };
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szProtoName, strlen(szProtoName), sztProtoName, 256);
				_tchar szPath[256] = { 0, };
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szFilePath, strlen(szFilePath), szPath, 256);

				if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOOL), sztProtoName,
					CTexture::Create(m_pDevice, m_pContext, szPath, 1))))
					return E_FAIL;
			}
		} while (FindNextFileA(h, &fd));
		FindClose(h);
	}

	memset(pattern, 0, sizeof(pattern));
	strcpy_s(pattern, MAX_PATH, "../Bin/Resources/Textures/NormalTexture/*.dds");

	h = FindFirstFileA(pattern, &fd);
	if (h != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				char szFilePath[MAX_PATH] = {};
				char szProtoName[MAX_PATH] = {};
				strcpy_s(szFilePath, MAX_PATH, "../Bin/Resources/Textures/NormalTexture/");
				strcat_s(szFilePath, MAX_PATH, fd.cFileName);
				strcat_s(szProtoName, MAX_PATH, "Prototype_Component_Texture_Normal_");
				strcat_s(szProtoName, MAX_PATH, fd.cFileName);

				_tchar sztProtoName[256] = { 0, };
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szProtoName, strlen(szProtoName), sztProtoName, 256);
				_tchar szPath[256] = { 0, };
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szFilePath, strlen(szFilePath), szPath, 256);

				if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOOL), sztProtoName,
					CTexture::Create(m_pDevice, m_pContext, szPath, 1))))
					return E_FAIL;
			}
		} while (FindNextFileA(h, &fd));
		FindClose(h);
	}













	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOOL), TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;


	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOOL), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOOL), TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
