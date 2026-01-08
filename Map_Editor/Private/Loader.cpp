#include "pch.h"
#include "Loader.h"

#include "Brush.h"
#include "Terrain.h"
#include "Sky.h"
#include "BackGround.h"
#include "Player.h"
#include "Camera_Free.h"
#include "GameInstance.h"
#include "CinemaTrack.h"
#include "Monster.h"
//#include "Body_Monster.h"

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
#include "Giwajip.h"
#include "Giwajip2.h"
#include "StoneLantern1.h"
#include "StoneLantern2.h"
#include "StoneTile.h"
#include "Virtual_Wall.h"

// Environment
#include "Rock.h"
#include "Reed.h"
#include "Rock1.h"
#include "Rock2.h"
#include "Rock3.h"
#include "Rock4.h"
#include "Rock5.h"
#include "Rock6.h"
#include "Rock7.h"
#include "Rock8.h"
#include "CM_Rock1.h"
#include "CM_Rock2.h"
#include "CM_Rock3.h"
#include "CM_Rock4.h"
#include "CM_Rock5.h"
#include "CM_Rock6.h"
#include "CM_Rock7.h"
#include "CM_Rock8.h"
#include "CM_Rock9.h"
#include "CM_Rock10.h"
#include "CM_Rock11.h"
#include "CM_Rock12.h"
#include "CM_Rock13.h"
#include "CM_Rock14.h"
#include "Bamboo.h"
#include "CherryBlossom1.h"
#include "CherryBlossom2.h"
#include "CherryBlossom3.h"
#include "CherryBlossom4.h"
#include "Grass.h"
#include "DryGrass1.h"
#include "DryGrass2.h"
#include "DryGrass3.h"
#include "Moon.h"
#include "InstanceModel.h"

#pragma region Map_Desert
#include "Sky_Desert.h"
#include "Terrain_Desert.h"
#include "Building_Ruin.h"
#include "Player_Test.h"
#include "Canyon.h"
#include "Instance_Desert.h"
#include "Deco.h"
#include "Lift_Body.h"
#include "Lift_Controller.h"
#include "Lift_Platform.h"
#include "Iron_Floor.h"
#include "SpawnBox.h"
#include "Desert_Tree.h"
#include "RepairConsole.h"
#include "Top_Roof.h"
#include "SciFi_Door.h"
#include "CanBox.h"
#include "Desert_Architecture.h"
#include "Interaction_NonAnim.h"
#include "Desert_Grass.h"
#include "DisplayBox.h"
#include "Shutter.h"
#include "Npc.h"
#include "Pad.h"
#include "Rail.h"
#include "Beat_Indicator.h"
#include "DororongBox.h"
#pragma endregion


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
	case LEVEL::DESERT:
	{
		m_strMessage = TEXT("집 가고싶다.");
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Building_Ruin(pArg); });

		m_strMessage = TEXT("맵 로딩중.");
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Canyon1(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Canyon1_1(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Canyon1_2(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Canyon2(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Canyon2_1(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Canyon2_2(pArg); });
		
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Archi(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Bridge(pArg); });
		
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Camp(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Corpse_And_Container(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Fence_And_Ruin(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Vehicle_And_Wheel(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Building_And_Trash(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Sign_And_Crane(pArg); });

		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Environment_Tree1(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Environment_Tree2(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Environment_Grass1(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Monster(pArg); });

		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Boxes(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Duct(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Garden_And_Poster(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Door_And_Statue(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Furniture(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Stair(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Xion_Wall(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Xion_Building(pArg); });

		//m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Monster(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Sky_Texture(pArg); });

		hr = Loading_For_Desert();
		m_strMessage = TEXT("로딩 완료.");
	}
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


	while (m_pGameInstance->IsWorkThread());
	m_strMessage = TEXT("완료..");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Village()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Scarlet/Terrain/005_A_TD.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Reed_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Texture_Reed_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Scarlet/Terrain/ReedMask.png"), 1))))
		return E_FAIL;

	///* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Texture_Sky1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Sky/T_Sky_RockyHills.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Player/Player.jpg"), 1))))
		return E_FAIL;

	m_strMessage = TEXT("네비게이션를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation2.bin")))))
		return E_FAIL;

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Scarlet/Terrain/Height2.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix			PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Sky"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Sky/Sky1.bin", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Stone1 */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Stone1"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/Stone1.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Stone2 */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Stone2"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/Stone2.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Stone3 */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Stone3"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/Stone3.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Stone4 */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Stone4"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/Stone4.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_StoneWall1 */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_StoneWall1"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/StoneWall1.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_StoneWall2 */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
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
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
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
	PreTransformMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
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
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/TombStone/TombStone_L.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Inscription_R */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Inscription_R"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/TombStone/TombStone_R.binx", PreTransformMatrix))))
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
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_StoneTile"),
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

	/* For.Prototype_Component_Texture_BlossomMask */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Texture_BlossomMask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Scarlet/Tree/009_B_TD_Mask.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BlossomMask2 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Texture_BlossomMask2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Scarlet/Tree/009_Bc_TD_Mask.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Giwajip */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Giwajip"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Giwajip/Giwajip.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Giwajip2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Giwajip2"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Giwajip/Giwajip2.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_StoneLantern1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_StoneLantern1"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/StoneLantern/StoneLantern1.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_StoneLantern2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_StoneLantern2"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/StoneLantern/StoneLantern2.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Grass */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Grass"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Grass/Grass.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DryGrass1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_DryGrass1"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Grass/DryGrass1.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_DryGrass2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_DryGrass2"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Grass/DryGrass2.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_DryGrass3 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_DryGrass3"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Grass/DryGrass3.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CM_Rock1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CM_Rock1"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock1.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CM_Rock2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CM_Rock2"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock2.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CM_Rock3 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CM_Rock3"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock3.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CM_Rock4 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CM_Rock4"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock4.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CM_Rock5 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CM_Rock5"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock5.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CM_Rock6 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CM_Rock6"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock6.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CM_Rock7 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CM_Rock7"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock7.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CM_Rock8 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CM_Rock8"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock8.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CM_Rock9 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CM_Rock9"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock9.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CM_Rock10 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CM_Rock10"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock10.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CM_Rock11 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CM_Rock11"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock11.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CM_Rock12 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CM_Rock12"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock12.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CM_Rock13 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CM_Rock13"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock13.binx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CM_Rock14 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CM_Rock14"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock14.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Moon */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Moon"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Sky/Moon2.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Reed */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	CVIBuffer_Instance_Model::MODEL_INSTANCE_DESC ModelDesc{};
	ModelDesc.iNumInstance = 10000;
	ModelDesc.vCenter = _float3(200.0f, 0.f, 200.0f);
	ModelDesc.vRange = _float3(150.f, 0.f, 150.f);
	ModelDesc.pModelFilePath = "../Bin/Resources/Maps/Scarlet/Reed/Reed5.binx";
	ModelDesc.PreModelMatrix = PreTransformMatrix;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Reed"),
		CVIBuffer_Instance_Model::Create(m_pDevice, m_pContext, &ModelDesc))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bamboo */
	/*PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	CVIBuffer_Instance_Model::MODEL_INSTANCE_DESC BambooDesc{};
	BambooDesc.iNumInstance = 1;
	BambooDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	BambooDesc.vRange = _float3(0.f, 0.f, 0.f);
	BambooDesc.pModelFilePath = "../Bin/Resources/Maps/Scarlet/Tree/Tree1.binx";
	BambooDesc.PreModelMatrix = PreTransformMatrix;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Bamboo"),
		CVIBuffer_Instance_Model::Create(m_pDevice, m_pContext, &BambooDesc))))
		return E_FAIL;*/

	/* For.Prototype_Component_Model_Rock5 */
	/*PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	CVIBuffer_Instance_Model::MODEL_INSTANCE_DESC Rock5Desc{};
	Rock5Desc.iNumInstance = 1;
	Rock5Desc.vCenter = _float3(0.0f, 0.f, 0.0f);
	Rock5Desc.vRange = _float3(0.f, 0.f, 0.f);
	Rock5Desc.pModelFilePath = "../Bin/Resources/Maps/Scarlet/Rock/Rock5.binx";
	Rock5Desc.PreModelMatrix = PreTransformMatrix;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Rock5"),
		CVIBuffer_Instance_Model::Create(m_pDevice, m_pContext, &Rock5Desc))))
		return E_FAIL;*/

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_Shader_Instance_Model */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_Instance_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/VTX_InstnaceMesh.hlsl"), VTX_NONEANIM_INSTANCE_DESC::Elements, VTX_NONEANIM_INSTANCE_DESC::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_VtxRectParticle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxRectParticle.hlsl"), VTX_POSTEX_INSTANCE_PARTICLE::Elements, VTX_POSTEX_INSTANCE_PARTICLE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_VtxPointParticle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxPointParticle.hlsl"), VTX_POS_INSTANCE_PARTICLE::Elements, VTX_POS_INSTANCE_PARTICLE::iNumElements))))
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
		CCinemaTrack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Brush */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Brush"),
		CBrush::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Virtual_Wall */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Virtual_Wall"),
		CVirtual_Wall::Create(m_pDevice, m_pContext))))
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

	/* For.Prototype_GameObject_StoneTile */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_StoneTile"),
		CStoneTile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Giwajip */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Giwajip"),
		CGiwajip::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Giwajip2 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Giwajip2"),
		CGiwajip2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Grass */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Grass"),
		CGrass::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_DryGrass1 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_DryGrass1"),
		CDryGrass1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_DryGrass2 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_DryGrass2"),
		CDryGrass2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_DryGrass3 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_DryGrass3"),
		CDryGrass3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Moon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Moon"),
		CMoon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CherryBlossom1 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CherryBlossom1"),
		CCherryBlossom1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CherryBlossom2 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CherryBlossom2"),
		CCherryBlossom2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CherryBlossom3 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CherryBlossom3"),
		CCherryBlossom3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CherryBlossom4 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CherryBlossom4"),
		CCherryBlossom4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_StoneLantern1 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_StoneLantern1"),
		CStoneLantern1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_StoneLantern2 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_StoneLantern2"),
		CStoneLantern2::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma region Common
	/* For.Prototype_GameObject_CM_Rock1 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CM_Rock1"),
		CCM_Rock1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CM_Rock2 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CM_Rock2"),
		CCM_Rock2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CM_Rock3 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CM_Rock3"),
		CCM_Rock3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CM_Rock4 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CM_Rock4"),
		CCM_Rock4::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CM_Rock5 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CM_Rock5"),
		CCM_Rock5::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CM_Rock6 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CM_Rock6"),
		CCM_Rock6::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CM_Rock7 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CM_Rock7"),
		CCM_Rock7::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CM_Rock8 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CM_Rock8"),
		CCM_Rock8::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CM_Rock9 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CM_Rock9"),
		CCM_Rock9::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CM_Rock10 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CM_Rock10"),
		CCM_Rock10::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CM_Rock11 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CM_Rock11"),
		CCM_Rock11::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CM_Rock12 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CM_Rock12"),
		CCM_Rock12::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CM_Rock13 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CM_Rock13"),
		CCM_Rock13::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CM_Rock14 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CM_Rock14"),
		CCM_Rock14::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Rock */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_CM_Rock"),
		CRock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_InstanceModel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_InstanceModel"),
		CInstanceModel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion 

	while (m_pGameInstance->IsWorkThread());
	m_strMessage = TEXT("완료 되었습니다..");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Monster(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	/* For.Prototype_Component_Model_StatueA */
	_matrix PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StatueA");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Models/Monster/Statue/A/CH_M_NA_40.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_StatueB */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StatueB");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Models/Monster/Statue/B/CH_M_NA_40_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Gorilla */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Gorilla");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Models/Gorilla/SuperGorilla.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	/* For.Prototype_Component_Model_Minion11 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Minion11");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../../Client/Bin/Resources/Models/Monster/Minion/11/Minion11.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	/* For.Prototype_Component_Model_SunFlower */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_SunFlower");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../../Client/Bin/Resources/Models/Monster/SunFlower/SunFlower.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Canyon2(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));


	/* For.Prototype_Component_Model_Canyon_100A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_100A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_100A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_101A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_101A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_101A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_103A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_103A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_103A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_104A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_104A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_104A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_105A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_105A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_105A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_106A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_106A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_106A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_108A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_108A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_108A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_109A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_109A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_109A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_110A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_110A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_110A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_111A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_111A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_111A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_112A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_112A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_112A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_113A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_113A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_113A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_115A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_115A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_115A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_116A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_116A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_116A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_117A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_117A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_117A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_121A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_121A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_121A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Canyon */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Canyon");
	pProtoDesc.pPrototype = CCanyon::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Instance_Desert */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Instance_Desert");
	pProtoDesc.pPrototype = CInstance_Desert::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_SpawnBox */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_SpawnBox");
	pProtoDesc.pPrototype = CSpawnBox::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Canyon2_1(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Canyon_127A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_127A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_127A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_127B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_127B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_127B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_127C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_127C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_127C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_127D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_127D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_127D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_127E */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_127E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_127E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_128A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_128A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_128A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_128B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_128B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_128B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_128C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_128C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_128C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_131A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_131A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_131A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_132A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_132A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_132A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_132B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_132B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_132B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_132C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_132C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_132C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_133B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_133B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_133B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Canyon2_2(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);
	_matrix PreTransformMatrix = {};

	/* For.Prototype_Component_Model_Canyon_80A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_80A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_80A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_81A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_81A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_81A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_93A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_93A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_93A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_95A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_95A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_95A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_96A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_96A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_96A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_97A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_97A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_97A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_98A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_98A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_98A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_122A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_122A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_122A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_123A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_123A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_123A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_125A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_125A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_125A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_126A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_126A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_126A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Lamp_47A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Lamp_47A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Lamp/Lamp_47A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_VendingMachine_6A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_VendingMachine_6A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/VendingMachine/VendingMachine_6A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_VendingMachine_7A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_VendingMachine_7A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/VendingMachine/VendingMachine_7A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Poster_4D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Poster_4D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Poster/Poster_4D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_SciFi_Door */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_SciFi_Door");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Maps/Desert/Door/SciFi_Door.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CanBox */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CanBox");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Maps/Desert/Deco/Box/CanBox.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Deco */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Deco");
	pProtoDesc.pPrototype = CDeco::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CanBox */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CanBox");
	pProtoDesc.pPrototype = CCanBox::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Interaction_NonAnim */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Interaction_NonAnim");
	pProtoDesc.pPrototype = CInteraction_NonAnim::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Desert_Architecture */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Architecture");
	pProtoDesc.pPrototype = CDesert_Architecture::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_GameObject_SciFi_Door */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_SciFi_Door");
	pProtoDesc.pPrototype = CSciFi_Door::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Camp(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Base_1A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Base_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Base_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- CAMP_1B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Camp_1B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1D
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Camp_1D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1E
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Camp_1E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1F
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Camp_1F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1G
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1G");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Camp_1G.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1H
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1H");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Camp_1H.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1I
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1I");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Camp_1I.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1J
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1J");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Camp_1J.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1K
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1K");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Camp_1K.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1L
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1L");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Camp_1L.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1M
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1M");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Camp_1M.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1N
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1N");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Camp_1N.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1R
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1R");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Camp_1R.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1S
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1S");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Camp_1S.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1T
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1T");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Camp_1T.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1W
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1W");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Camp_1W.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- REPAIRCONSOLE
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_RepairConsole");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/RepairConsole.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TOP_ROOF
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Top_Roof");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Maps/Desert/Deco/Camp/Top_Roof.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_GameObject_RepairConsole */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_RepairConsole");
	pProtoDesc.pPrototype = CRepairConsole::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Top_Roof */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Top_Roof");
	pProtoDesc.pPrototype = CTop_Roof::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Corpse_And_Container(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Corpse_1A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Corpse_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Corpse/Corpse_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Corpse_1B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Corpse_1B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Corpse/Corpse_1B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Corpse_2A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Corpse_2A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Corpse/Corpse_2A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Corpse_2B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Corpse_2B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Corpse/Corpse_2B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Corpse_2C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Corpse_2C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Corpse/Corpse_2C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Corpse_3A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Corpse_3A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Corpse/Corpse_3A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_2A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_2A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Container/Container_2A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_2B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_2B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Container/Container_2B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_2C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_2C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Container/Container_2C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_2D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_2D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Container/Container_2D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_3A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_3A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Container/Container_3A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_4B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_4B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Container/Container_4B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_5A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_5A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Container/Container_5A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_5B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_5B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Container/Container_5B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_5C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_5C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Container/Container_5C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_5D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_5D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Container/Container_5D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_5E */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_5E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Container/Container_5E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_7B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_7B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Container/Container_7B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_7F */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_7F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Container/Container_7F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Fence_And_Ruin(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Fence_1A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Fence_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Fence/Fence_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Fence_1B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Fence_1B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Fence/Fence_1B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Fence_1F */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Fence_1F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Fence/Fence_1F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Fence_1H */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Fence_1H");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Fence/Fence_1H.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_7A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_7A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Ruin/Ruin_7A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_7B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_7B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Ruin/Ruin_7B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_7C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_7C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Ruin/Ruin_7C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_7D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_7D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Ruin/Ruin_7D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_7E */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_7E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Ruin/Ruin_7E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Bridge(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Bridge_4A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_4A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Bridge/Bridge_4A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_4B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_4B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Bridge/Bridge_4B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_4D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_4D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Bridge/Bridge_4D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_4H */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_4H");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Bridge/Bridge_4H.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_4L */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_4L");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Bridge/Bridge_4L.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_4M */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_4M");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Bridge/Bridge_4M.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_4N */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_4N");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Bridge/Bridge_4N.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_5 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_5");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Bridge/Bridge_5.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_6 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_6");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Bridge/Bridge_6.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_8 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_8");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Bridge/Bridge_8.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_14A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_14A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Bridge/Bridge_14A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_14B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_14B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Bridge/Bridge_14B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Vehicle_And_Wheel(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Vehicle_2A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Vehicle_2A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Vehicle/Vehicle_2A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Vehicle_2B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Vehicle_2B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Vehicle/Vehicle_2B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Vehicle_3B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Vehicle_3B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Vehicle/Vehicle_3B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Vehicle_4C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Vehicle_4C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Vehicle/Vehicle_4C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Vehicle_6A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Vehicle_6A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Vehicle/Vehicle_6A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Vehicle_8B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Vehicle_8B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Vehicle/Vehicle_8B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Vehicle_14A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Vehicle_14A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Vehicle/Vehicle_14A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Vehicle_14B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Vehicle_14B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Vehicle/Vehicle_14B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wheel_1A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wheel_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Wheel/Wheel_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wheel_1B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wheel_1B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Wheel/Wheel_1B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wheel_1C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wheel_1C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Wheel/Wheel_1C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Building_And_Trash(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Building_4B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Building_4B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Building/Building_4B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Building_4C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Building_4C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Building/Building_4C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Building_4D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Building_4D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Building/Building_4D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Trash_1A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Trash_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Trash/Trash_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Trash_2A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Trash_2A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Trash/Trash_2A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Trash_2B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Trash_2B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Trash/Trash_2B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Trash_4A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Trash_4A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Trash/Trash_4A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Trash_9A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Trash_9A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Trash/Trash_9A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Trash_17A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Trash_17A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Trash/Trash_17A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Sign_And_Crane(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Sign_11A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Sign_11A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Sign/Sign_11A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Sign_11B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Sign_11B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Sign/Sign_11B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Sign_11F */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Sign_11F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Sign/Sign_11F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Sign_12B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Sign_12B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Sign/Sign_12B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Sign_12C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Sign_12C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Sign/Sign_12C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Sign_36B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Sign_36B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Sign/Sign_36B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Crane_1C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Crane_1C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Crane/Crane_1C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Crane_11 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Crane_11");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Crane/Crane_11.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Crane_13 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Crane_13");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Crane/Crane_13.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Archi(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Lift_Controller */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Lift_Controller");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Maps/Desert/Lift/Lift_Controller.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Lift_Body */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Lift_Body");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Lift/Lift_Body.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Lift_Platform */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Lift_Platform");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Lift/Lift_Platform.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Floor/Floor7_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Floor/Floor7_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Floor/Floor7_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Floor/Floor7_D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_E */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Floor/Floor7_E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_F */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Floor/Floor7_F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Iron_Floor */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Iron_Floor");
	pProtoDesc.pPrototype = CIron_Floor::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Lift_Controller */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Lift_Controller");
	pProtoDesc.pPrototype = CLift_Controller::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Lift_Body */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Lift_Body");
	pProtoDesc.pPrototype = CLift_Body::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Lift_Platform */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Lift_Platform");
	pProtoDesc.pPrototype = CLift_Platform::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Environment_Tree1(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Tree_1A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree1_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_2A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_2A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree2_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_3A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_3A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree3_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_4A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_4A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree4_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_5B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_5B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree5_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_5C
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_5C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree5_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_6A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_6A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree6_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_6B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_6B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree6_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_7A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_7A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree7_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_7B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_7B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree7_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_8A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_8A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree8_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_8Aa
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_8Aa");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree8_Aa.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_8B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_8B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree8_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_8Ba
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_8Ba");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree8_Ba.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_10A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_10A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree10_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_11B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_11B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree11_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_15A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_15A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree15_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_15B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_15B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree15_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_16A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_16A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree16_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_17A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_17A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree17_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_18A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_18A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree18_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_19A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_19A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree19_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_20A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_20A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree20_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_21A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_21A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree21_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_23A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_23A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree23_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_25A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_25A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree25_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Desert_Tree */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Desert_Tree");
	pProtoDesc.pPrototype = CDesert_Tree::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Environment_Tree2(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	// --- TREE_26A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_26A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree26_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_27A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_27A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree27_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_29A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_29A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree29_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_30A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_30A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree30_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_31A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_31A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree31_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_32A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_32A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree32_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_33A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_33A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree33_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_34A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_34A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree34_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree40_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40C
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree40_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40D
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree40_D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40E
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree40_E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40F
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree40_F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40G
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40G");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree40_G.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40H
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40H");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree40_H.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40I
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40I");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree40_I.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40J
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40J");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree40_J.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_42A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_42A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree42_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_43A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_43A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree43_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_43B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_43B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree43_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_44A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_44A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree44_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_44B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_44B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree44_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_44D
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_44D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree44_D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_44F
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_44F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree44_F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_45A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_45A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree45_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_45B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_45B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree45_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_45C
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_45C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Tree/Tree45_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Environment_Grass1(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_DeadShrubs_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_DeadShrubs_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/DeadShrubs/DeadShrubs_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_DeadShrubs_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_DeadShrubs_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/DeadShrubs/DeadShrubs_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_GameObject_Desert_Grass */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Desert_Grass");
	pProtoDesc.pPrototype = CDesert_Grass::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Var02_Body */
	PreTransformMatrix = XMMatrixScaling(2.8f, 2.8f, 2.8f) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_D1G-g2r_Body");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Models/Character/NPC/CH_NPC_05/CH_NPC_05_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	
	/* For.Prototype_Component_Model_Scarlet_Body */
	PreTransformMatrix = XMMatrixScaling(3.5f, 3.5f, 3.5f) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Scarlet_Body");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Models/Scarlet/CH_M_Scarlet_Body/CH_M_Scarlet_Body_test05.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Npc */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Npc");
	pProtoDesc.pPrototype = CNpc::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Monster(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(2.8f, 2.8f, 2.8f) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	/* For.Prototype_Component_Model_Beholder */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Beholder");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Models/Monster/Beholder/CH_M_NA_51.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	PreTransformMatrix = XMMatrixScaling(2.8f, 2.8f, 2.8f) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	/* For.Prototype_Component_Model_SunFlower */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_SunFlower");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Models/Monster/SunFlower/SunFlower.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	PreTransformMatrix = XMMatrixScaling(2.8f, 2.8f, 2.8f) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	/* For.Prototype_Component_Model_Minion11 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Minion11");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Models/Monster/Minion/11/Minion11.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_BanacleA */
	PreTransformMatrix = XMMatrixScaling(2.8f, 2.8f, 2.8f) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_BanacleA");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Models/Monster/Banacle/CH_M_NA_08.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Tentacle */
	PreTransformMatrix = XMMatrixScaling(2.8f, 2.8f, 2.8f) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tentacle");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Models/Monster/Tentacle/Tentacle.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_DroidTurret */
	PreTransformMatrix = XMMatrixScaling(2.8f, 2.8f, 2.8f) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_DroidTurret");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Models/Monster/DroidTurret/TurretDroid.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_StatueA */
	PreTransformMatrix = XMMatrixScaling(2.8f, 2.8f, 2.8f) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StatueA");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Models/Monster/Statue/A/CH_M_NA_40.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_StatueB */
	PreTransformMatrix = XMMatrixScaling(2.8f, 2.8f, 2.8f) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StatueB");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Models/Monster/Statue/B/CH_M_NA_40_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Gorilla");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Models/Gorilla/SuperGorilla_v01.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Boxes(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	/* For.Prototype_Component_Model_Box_1A */
	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_1B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_1B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_1B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_2C
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_2C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_2C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_4A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_4A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_4A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_5A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_5A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_5A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_6A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_6A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_6A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_11A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_11A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_11A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_13A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_13A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_13A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_14A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_14A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_14A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_16A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_16A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_16A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_16B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_16B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_16B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_19A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_19A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_19A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_20A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_20A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_20A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_20B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_20B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_20B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_20C
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_20C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_20C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_20D
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_20D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_20D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_20E
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_20E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_20E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_20F
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_20F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_20F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_21A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_21A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_21A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_21B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_21B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_21B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// BOX_26A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Box_26A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Box/Box_26A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// DisplayBox
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_DisplayBox");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Maps/Xion/Deco/Box/DisplayBox.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_DisplayBox */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_DisplayBox");
	pProtoDesc.pPrototype = CDisplayBox::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Duct(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	/* For.Prototype_Component_Model_Duct_1A */
	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_1B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_1B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_1B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_1C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_1C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_1C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_1D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_1D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_1D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_1E */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_1E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_1E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_1G */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_1G");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_1G.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_1H */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_1H");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_1H.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_3A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_3A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_3A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_3B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_3B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_3B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_3C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_3C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_3C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_4A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_4A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_4A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_4B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_4B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_4B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_4C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_4C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_4C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_6A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_6A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_6A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_8A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_8A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_8A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_9A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_9A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_9A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_9B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_9B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_9B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_9C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_9C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_9C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_9D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_9D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_9D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_10A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_10A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_10A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Duct_13A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Duct_13A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Duct/Duct_13A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Garden_And_Poster(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	/* For.Prototype_Component_Model_Garden_1A */
	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Garden_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Garden/Garden_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Garden_1B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Garden_1B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Garden/Garden_1B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Garden_1C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Garden_1C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Garden/Garden_1C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	// --- Restroom Group ---
	/* For.Prototype_Component_Model_Restroom_4A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Restroom_4A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Restroom/Restroom_4A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Poster_2A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Poster_2A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Poster/Poster_2A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Poster_2B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Poster_2B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Poster/Poster_2B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Poster_3A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Poster_3A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Poster/Poster_3A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Poster_3B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Poster_3B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Poster/Poster_3B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Poster_4A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Poster_4A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Poster/Poster_4A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Poster_4B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Poster_4B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Poster/Poster_4B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Poster_4C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Poster_4C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Poster/Poster_4C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Poster_4E */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Poster_4E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Poster/Poster_4E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Door_And_Statue(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Door_3A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_3A");			// Layer_Architecture
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Door/Door_3A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Shutter */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Shutter");			// Layer_Shutter
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Door/Door_3C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_Controller */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_Controller");	// Layer_Interaction
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Door/Door_3D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Statue_1A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Statue_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Statue/Statue_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Statue_19B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Statue_19B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Statue/Statue_19B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Statue_24B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Statue_24B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Statue/Statue_24B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Statue_31B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Statue_31B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Statue/Statue_31B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Statue_40B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Statue_40B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Statue/Statue_40B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Shutter */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Shutter");
	pProtoDesc.pPrototype = CShutter::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Furniture(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Furniture_7A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Furniture_7A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Furniture/Furniture_7A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Furniture_9A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Furniture_9A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Furniture/Furniture_9A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Furniture_47A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Furniture_47A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Furniture/Furniture_47A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Furniture_50A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Furniture_50A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Furniture/Furniture_50A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Furniture_50B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Furniture_50B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Furniture/Furniture_50B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Furniture_50C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Furniture_50C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Furniture/Furniture_50C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Furniture_50D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Furniture_50D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Furniture/Furniture_50D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Furniture_51A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Furniture_51A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Furniture/Furniture_51A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Furniture_57A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Furniture_57A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Furniture/Furniture_57A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Furniture_59A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Furniture_59A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Furniture/Furniture_59A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Furniture_77A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Furniture_77A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Furniture/Furniture_77A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Furniture_79A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Furniture_79A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Furniture/Furniture_79A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Furniture_83A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Furniture_83A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Furniture/Furniture_83A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Furniture_87B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Furniture_87B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Furniture/Furniture_87B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Furniture_87D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Furniture_87D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Deco/Furniture/Furniture_87D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Stair(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Stair_1A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stair_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Stair/Stair_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stair_1B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stair_1B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Stair/Stair_1B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stair_1C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stair_1C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Stair/Stair_1C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stair_2A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stair_2A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Stair/Stair_2A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stair_3A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stair_3A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Stair/Stair_3A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stair_3B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stair_3B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Stair/Stair_3B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Xion_Wall(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Xion_Wall_1C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_1C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_1C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_1D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_1D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_1D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_1Db */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_1Db");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_1Db.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_1G */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_1G");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_1G.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_1I */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_1I");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_1I.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_1K */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_1K");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_1K.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_1Q */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_1Q");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_1Q.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_1R */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_1R");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_1R.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_1S */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_1S");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_1S.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_1T */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_1T");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_1T.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_2A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_2A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_2A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_2B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_2B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_2B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_4H */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_4H");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_4H.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_5A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_5A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_5A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_8A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_8A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_8A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_9A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_9A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_9A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_9B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_9B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_9B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_10A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_10A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_10A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_10B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_10B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_10B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_10C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_10C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_10C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Wall_10D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Wall_10D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Wall/Wall_10D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Xion_Building(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	///* For.Prototype_Component_Model_Xion_Building_3A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building_3A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building/Building_3A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Building_4A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building_4A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building/Building_4A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Building_5A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building_5A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building5/Building_5A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Building_6A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building_6A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building6/Building_6A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Building_8 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building_8");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building/Building_8.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Building_9A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building_9A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building4/Building_9A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Building_17 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building_17");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building4/Building_17.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Building4_1A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building4_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building4/Building4_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Building4_1B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building4_1B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building4/Building4_1B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Building4_1C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building4_1C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building4/Building4_1C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Building4_1D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building4_1D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building4/Building4_1D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Building4_1E */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building4_1E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building4/Building4_1E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Building4_1F */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building4_1F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building4/Building4_1F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Building4_1G */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building4_1G");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building4/Building4_1G.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Building5_4 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building5_4");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building4/Building5_4.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Building5_callD4 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building5_callD4");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building4/Building5_callD4.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Xion_Building5_callD5 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Xion_Building5_callD5");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Building4/Building5_callD5.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Store_3 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Store_3");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Store/Store_3.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Store_4 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Store_4");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Xion/Store/Store_4.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) 
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Sky_Texture(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	/* For.Prototype_Component_Texture_Sky_Desert */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Texture_galaxy+X"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Sky/galaxy+X.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky_Desert */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Texture_DororongMask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Dororong/Jonnadaechung.png"), 1))))
		return E_FAIL;

	/* For.Prototype_GameObject_Pad */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Pad");
	pProtoDesc.pPrototype = CPad::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Rail */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Rail");
	pProtoDesc.pPrototype = CRail::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Beat_Indicator */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Beat_Indicator");
	pProtoDesc.pPrototype = CBeat_Indicator::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_DororongBox */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_DororongBox");
	pProtoDesc.pPrototype = CDororongBox::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Building_Ruin(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	/* For.Prototype_Component_Model_Door_A */
	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Door_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Door_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Door_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Door_D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_E */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Door_E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_G */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_G");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Door_G.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_H */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_H");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Door_H.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_I */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_I");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Door_I.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_J */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_J");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Door_J.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_N */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_N");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Door_N.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_P */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_P");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Door_P.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_R */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_R");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Door_R.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Floor_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Floor_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Floor_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Floor_D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_E */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Floor_E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_F */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Floor_F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_G */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_G");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Floor_G.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_H */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_H");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Floor_H.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Frame_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Frame_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Frame_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Frame_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Frame_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Frame_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Ruin_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Ruin_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Ruin_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Ruin_D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_Building_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_Building_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Ruin_Building_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_Building_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_Building_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Ruin_Building_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_Building_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_Building_C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Ruin_Building_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stone009_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stone009_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Stone009_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stone009_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stone009_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Stone009_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall_D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_D_1 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_D_1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall_D_1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_E */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall_E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_F */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall_F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_G */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_G");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall_G.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_H */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_H");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall_H.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_I */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_I");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall_I.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall007_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall007_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall007_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall007_D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_E */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall007_E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_F */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall007_F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_G */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_G");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Building_Ruin/Wall007_G.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Building_Ruin */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Building_Ruin");
	pProtoDesc.pPrototype = CBuilding_Ruin::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Canyon1(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);
	_matrix PreTransformMatrix = {};

	/* For.Prototype_Component_Model_Canyon_1A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_2A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_2A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_2A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_3A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_3A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_3A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_4A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_4A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_4A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_5A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_5A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_5A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_6A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_6A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_6A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_12A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_12A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_12A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_14A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_14A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_14A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_14B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_14B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_14B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_15A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_15A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_15A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_16A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_16A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_16A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_16B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_16B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_16B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_17A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_17A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_17A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_17B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_17B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_17B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_18A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_18A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_18A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_50A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_50A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Canyon1_1(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = {};

	/* For.Prototype_Component_Model_Canyon_52A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_52A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_52A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_55A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_55A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_55A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_58A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_58A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_58A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_59A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_59A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_59A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_Component_Model_Canyon_60A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_60A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_60A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_Component_Model_Canyon_61A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_61A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_61A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_65A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_65A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_65A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_66A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_66A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_66A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_67A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_67A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_67A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_69A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_69A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_69A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_71A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_71A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_71A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Canyon1_2(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::DESERT);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Canyon_20A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_20A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_20A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_21A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_21A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_21A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_22A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_22A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_22A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_23A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_23A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_23A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_24A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_24A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_24A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_35A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_35A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_35A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_39A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_39A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_39A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_39C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_39C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_39C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_39D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_39D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_39D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_43A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_43A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_43A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_44A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_44A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_44A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_46A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_46A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_46A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Texture_Terrain */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Terrain_Desert_Normal");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Desert/Terrain/009_A_TN.dds"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert()
{
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Texture_Terrain_Desert"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Desert/Terrain/009_A_TD.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Terrain_Red */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Texture_Terrain_Desert_Red"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Desert/Terrain/004_A_TD.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Terrain_Green */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Texture_Terrain_Desert_Green"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Desert/Terrain/003_A_TD.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain_Blue */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Texture_Terrain_Desert_Blue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Xion/Terrain/008_A_TD.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_ORM_Texture_Terrain_Desert"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Desert/Terrain/002_A_TAoRM.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_ORM_Texture_Terrain_Desert_Red"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Desert/Terrain/004_A_TAoRM.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_ORM_Texture_Terrain_Desert_Green"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Desert/Terrain/003_A_TAoRM.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Desert/Terrain/Terrain_Mask.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation2.bin")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_VIBuffer_Terrain_Desert"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Desert/Terrain/Height2.bmp")))))
		return E_FAIL;

	_matrix PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationY(XMConvertToRadians(270.0f)) * XMMatrixRotationZ(XMConvertToRadians(90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Model_Eve"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Models/Character/PC/Eve/CH_P_EVE_Model/Eve_Body_24_TypeB.binx", PreTransformMatrix))))
		return E_FAIL;

	//../Bin/Resources/Models/Character/PC/Eve/CH_P_EVE_Model/Eve_Body_24_TypeB.binx
	/* For.Prototype_Component_Shader_Instance_Model */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Shader_Instance_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/VTX_InstnaceMesh.hlsl"), VTX_NONEANIM_INSTANCE_DESC::Elements, VTX_NONEANIM_INSTANCE_DESC::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Shader_VtxRectParticle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxRectParticle.hlsl"), VTX_POSTEX_INSTANCE_PARTICLE::Elements, VTX_POSTEX_INSTANCE_PARTICLE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Shader_VtxPointParticle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxPointParticle.hlsl"), VTX_POS_INSTANCE_PARTICLE::Elements, VTX_POS_INSTANCE_PARTICLE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Collider_AABB"),
		CBoxCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Collider_OBB"),
		COBBCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Collider_Sphere"),
		CSphereCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_CinemaComponent */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_CinemaComponent"),
		CCinemaTrack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_GameObject_Terrain_Desert"),
		CTerrain_Desert::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_GameObject_Player_Test"),
		CPlayer_Test::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	
	/* For.Prototype_Component_Texture_Sky_Desert */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Texture_Sky_Desert6"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Sky/Panorama_Sky_06-512x512.png"), 1))))
		return E_FAIL;


	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_GameObject_Sky_Desert"),
		CSky_Desert::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	/*if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Model_Sky_Desert"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Sky/Sky3.binx", PreTransformMatrix))))
		return E_FAIL;*/

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Model_Sky_Desert"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Sky/Sky4.fbx", PreTransformMatrix))))
		return E_FAIL;

	while (m_pGameInstance->IsWorkThread())
		int a = 10;

	m_strMessage = TEXT("완료 되었습니다..");
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