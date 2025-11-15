#include "pch.h"
#include "Loader.h"

#include "Terrain.h"
#include "Sky.h"
#include "BackGround.h"
#include "Player.h"

#include "GameInstance.h"
#include "Vil_Bui03_04.h"
#include "Terrain_Sand.h"

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
	case LEVEL::LOGO:
		hr = Loading_For_Logo();
		break;
	case LEVEL::VILLAGE:
		hr = Loading_For_Village();
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

HRESULT CLoader::Loading_For_Logo()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Texture/Logo/T_Abyss_BGMain.png"), 1))))
		return E_FAIL;

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Village()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1))))
		return E_FAIL;

	///* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Texture_Sky1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Sky/T_skybox_06.png"), 4))))
		return E_FAIL;

	m_strMessage = TEXT("네비게이션를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation2.bin")))))
		return E_FAIL;

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix			PreTransformMatrix = XMMatrixIdentity();

	///* For.Prototype_Component_Model_Terrain_Sand */
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Terrain_Sand"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Terrain/Earth/Sand/Terrain_Sand.fbx", PreTransformMatrix))))
	//	return E_FAIL;

	/* For.Prototype_Component_Model_Terrain_006_A_SM */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Terrain_Sand"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Terrain/006_A_SM//006_A_SM.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Sky"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Sky/Sky1.bin", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Fiona */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Fiona/Fiona.bin", PreTransformMatrix))))
		return E_FAIL;

	///* For.Prototype_Component_Model_Vil_Bui03_04 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Vil_Bui03_04"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Village/Obj/Vil_Bui03_04.bin", PreTransformMatrix))))
		return E_FAIL;



	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_VtxRectParticle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectParticle.hlsl"), VTX_POSTEX_INSTANCE_PARTICLE::Elements, VTX_POSTEX_INSTANCE_PARTICLE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_VtxPointParticle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointParticle.hlsl"), VTX_POS_INSTANCE_PARTICLE::Elements, VTX_POS_INSTANCE_PARTICLE::iNumElements))))
		return E_FAIL;

	m_strMessage = TEXT("콜라이더를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Collider_AABB"),
		CBoxCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Collider_OBB"),
		COBBCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Collider_Sphere"),
		CSphereCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain_Sand */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Terrain_Sand"),
		CTerrain_Sand::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Vil_Bui03_04 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Vil_Bui03_04"),
		CVil_Bui03_04::Create(m_pDevice, m_pContext))))
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
