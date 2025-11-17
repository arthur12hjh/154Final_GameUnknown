#include "pch.h"
#include "Loader.h"

#include "Terrain.h"
#include "Sky.h"
#include "BackGround.h"
#include "Player.h"

#include "GameInstance.h"
#include "Vil_Bui03_04.h"
#include "CinemaData.h"
#include "Terrain_Sand.h"

// Buildings
#include "StoneWall1.h"
#include "StoneWall2.h"
#include "Inscription_L.h"
#include "Inscription_R.h"
#include "TombStone.h"
#include "TombStoneBase1.h"
#include "TombStoneBase2.h"
#include "Stair.h"
#include "Stone1.h"
#include "Stone2.h"
#include "Stone3.h"
#include "Stone4.h"

// Environment
#include "Reed.h"
#include "Rock1.h"
#include "Rock2.h"
#include "Rock3.h"
#include "Rock4.h"
#include "Rock5.h"
#include "Rock6.h"
#include "Rock7.h"
#include "Rock8.h"
#include "Bamboo.h"

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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Scarlet/Terrain/005_A_TD.png"), 1))))
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
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Scarlet/Terrain/Height.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Terrain */
	/*if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Tutorial/Textures/Height2.bmp")))))
		return E_FAIL;*/

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

	/* For.Prototype_Component_Model_Vil_Bui03_04 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Vil_Bui03_04"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Village/Obj/Vil_Bui03_04.bin", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Stone1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Stone1"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/Stone1.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Stone2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Stone2"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/Stone2.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Stone3 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Stone3"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/Stone3.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Stone4 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Stone4"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/Stone4.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_StoneWall1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_StoneWall1"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/StoneWall1.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_StoneWall2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_StoneWall2"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/StoneWall2.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bamboo */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Bamboo"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Tree/Tree1.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Rock1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Rock1"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Rock/Rock1.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Rock2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Rock2"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Rock/Rock2.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Rock3 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Rock3"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Rock/Rock3.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Rock4 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Rock4"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Rock/Rock4.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Rock5 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Rock5"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Rock/Rock5.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Rock6 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Rock6"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Rock/Rock6.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Rock7 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Rock7"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Rock/Rock7.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Rock8 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Rock8"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Rock/Rock8.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TombStone */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_TombStone"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/TombStone/tombstone.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Inscription_L */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Inscription_L"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/TombStone/Inscription_L.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Inscription_R */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Inscription_R"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/TombStone/Inscription_R.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TombStoneBase1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_TombStoneBase1"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/TombStone/TombStoneBase1.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TombStoneBase2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_TombStoneBase2"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/TombStone/TombStoneBase2.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Stair */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Stair"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stair/Stair.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Tile */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Tile"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Tile/StoneTile.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CherryBlossom1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CherryBlossom1"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Tree/CherryBlossom1.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CherryBlossom2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CherryBlossom2"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Tree/CherryBlossom2.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CherryBlossom3 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CherryBlossom3"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Tree/CherryBlossom3.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CherryBlossom4 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CherryBlossom4"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Tree/CherryBlossom4.binx", PreTransformMatrix))))
		return E_FAIL;
	///* For.Prototype_Component_Model_CherryBlossom5 */
	//PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CherryBlossom5"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Tree/CherryBlossom5.fbx", PreTransformMatrix))))
	//	return E_FAIL;

	/* For.Prototype_Component_Model_Giwajip */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Giwajip"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Giwajip/Giwajip.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_StoneLantern1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_StoneLantern1"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/StoneLantern/StoneLantern1.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_StoneLantern2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_StoneLantern2"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/StoneLantern/StoneLantern2.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Reed */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	CVIBuffer_Instance_Model::MODEL_INSTANCE_DESC ModelDesc{};
	ModelDesc.iNumInstance = 1000;
	ModelDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	ModelDesc.vRange = _float3(30.f, 0.f, 30.f);
	ModelDesc.pModelFilePath = "../Bin/Resources/Maps/Scarlet/Reed/Reed1.fbx";
	ModelDesc.PreModelMatrix = PreTransformMatrix;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Reed"),
		CVIBuffer_Instance_Model::Create(m_pDevice, m_pContext, &ModelDesc))))
		return E_FAIL;

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_Shader_Instance_Model */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_Instance_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/VTX_InstnaceMesh.hlsl"), VTX_NONEANIM_INSTANCE_DESC::Elements, VTX_NONEANIM_INSTANCE_DESC::iNumElements))))
		return E_FAIL;

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

	/* For.Prototype_Component_CinemaComponent */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_CinemaComponent"),
		CCinemaData::Create(m_pDevice, m_pContext))))
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

	/* For.Prototype_GameObject_Rock1 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Rock1"),
		CRock1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Rock2 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Rock2"),
		CRock2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Rock3 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Rock3"),
		CRock3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Rock4 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Rock4"),
		CRock4::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Rock5 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Rock5"),
		CRock5::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Rock6 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Rock6"),
		CRock6::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Rock7 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Rock7"),
		CRock7::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Rock8 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Rock8"),
		CRock8::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Reed */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Reed"),
		CReed::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Bamboo */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Bamboo"),
		CBamboo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_StoneWall1 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_StoneWall1"),
		CStoneWall1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_StoneWall2 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_StoneWall2"),
		CStoneWall2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Stone1 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Stone1"),
		CStone1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Stone2 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Stone2"),
		CStone2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Stone3 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Stone3"),
		CStone3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Stone4 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Stone4"),
		CStone4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Inscription_L */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Inscription_L"),
		CInscription_L::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Inscription_R */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Inscription_R"),
		CInscription_R::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_TombStone */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_TombStone"),
		CTombStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_TombStoneBase1 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_TombStoneBase1"),
		CTombStoneBase1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_TombStoneBase2 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_TombStoneBase2"),
		CTombStoneBase2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Stair */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Stair"),
		CStair::Create(m_pDevice, m_pContext))))
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
