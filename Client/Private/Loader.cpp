#include "pch.h"
#include "Loader.h"

#include "SpriteEffect.h"
#include "BackGround.h"
#include "Explosion.h"
#include "ForkLift.h"

#include "Terrain.h"
#include "Weapon.h"
#include "Player.h"
#include "Snow.h"
#include "Sky.h"
#include "Sky_Scarlet.h"
#include "AttackHitBox.h"

#include "Effect.h"
#include "Trail.h"
#include "TrailData.h"
#include "TrailEffect.h"

#include "GameInstance.h"
#include "GameManager.h"

#pragma region Item
#include "Item.h"
#pragma endregion

#pragma region PLAYER
#include "PlayerFSM.h"
#include "Body_Player.h"
#include "Face_Player.h"
#include "Hair_Player.h"
#include "PonyTail_Player.h"
#include "CameraBone_Player.h"
#pragma endregion

#pragma region Bullet
#include "Bullet_Rock.h"
#include "Bullet_Scarlet.h"
#pragma endregion

#pragma region Monster

#include "MonsterFSM.h"
#include "Nayitba.h"

#include "NayitbaPartBody.h"
#include "NaytibaLeftWeaponPart.h"
#include "NaytibaRightWeaponPart.h"

#include "Spawner.h"
#include "MonsterController.h"
#include "MonsterMimesisController.h"

#include "BossController.h"
#include "GorillaBehaviorTree.h"
#include "ScarletBehaviorTree.h"

#pragma endregion

#pragma region CinematicModel

#include "LinkAttackTester.h"
#include "Body_LinkAttackTester.h"

#include "CinematicModel_Gorilla.h"
#include "CinematicModel_Eve.h"
#include "CinematicModel_Scarlet.h"


#pragma endregion

#pragma region Client Component
#include "Interaction_Component.h"
#include "TargetComponent.h"
#include "DropComponent.h"
#include "AISenceComponent.h"
#pragma endregion

#pragma region Prob
#include "Prob_Static.h"
#include "Prob_Destory.h"

#pragma region Interaction
#include "StaticInteraction.h"
#include "CanBox.h"
#pragma endregion
#pragma endregion

#pragma region Map_Scarlet
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
#include "CherryBlossom1.h"
#include "CherryBlossom2.h"
#include "CherryBlossom3.h"
#include "CherryBlossom4.h"
#include "Grass.h"
#include "DryGrass1.h"
#include "DryGrass2.h"
#include "DryGrass3.h"
#include "Moon.h"
#pragma endregion

#pragma region Map Common
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
#include "InstanceModel.h"
#pragma endregion
#pragma region Map_Desert
#include "Canyon.h"
#include "Building_Ruin.h"
#include "Terrain_Desert.h"
#include "Instance_Desert.h"
#include "Lift_Body.h"
#include "Lift_Controller.h"
#include "Lift_Platform.h"
#include "Iron_Floor.h"
#include "Deco.h"
#include "SciFi_Door.h"
#include "Top_Roof.h"
#include "RepairConsole.h"
#include "Desert_Architecture.h"
#include "Desert_Tree.h"
#include "Desert_Grass.h"
#pragma endregion

#include "Instance_Model.h"
#include "PxTestProp.h"

#pragma region UI Header
#include "UIWrapper.h"
#include "UIPanel.h"
#include "UIButton.h"
#include "UIText.h"
#include "UIImage.h"
#include "UIPotion.h"
#include "UIPotionCount.h"
#include "UIPotionStack.h"
#include "UIHPBar.h"
#include "UIHPFX.h"
#include "UIShield.h"
#include "UIBeta.h"
#include "UIBetaFX.h"
#include "UISkillWrapper.h"
#include "UISkillWrapperLineFX.h"
#include "UISkillWrapperOnFX.h"
#include "UISkillSlot.h"
#include "UIRushSlot.h"

#include "UIWorldWrapper.h"
#include "UIMonsterHPBar.h"
#include "UIMonsterHPFX.h"
#include "UIMonsterShield.h"
#include "UIMonsterStamina.h"
#include "UIMonsterStaminaFX.h"
#include "UIBossVitalWrapper.h"
#include "UIBossHPBar.h"
#include "UIBossHPBarFX.h"
#include "UIBossShield.h"
#include "UIBossStamina.h"
#include "UIBossStaminaFX.h"
#include "UIBossName.h"
#include "UISimpleKey.h"
#include "UIInteractionFX.h"
#include "UILockOn.h"
#include "UIGetterQueue.h"
#include "UIOwnGold.h"
#include "UIScript.h"

#include "UIPopup.h"
#include "UICostumePuzzleAnswer.h"
#include "UICostumePuzzleButtons.h"

#pragma endregion


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
	, m_pGameManager{ CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
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

	InitializeCriticalSection(&m_CriticalSection);

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
	case LEVEL::GAMEPLAY:
	{
		m_strMessage = TEXT("Loading Map Resource");
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Map(pArg); });

		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Map_DesertA(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Map_DesertA_Col(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Map_DesertB(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Map_DesertB_Col(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Map_DesertC(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Map_DesertC_Col(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Map_DesertD(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Map_DesertD_Col(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Map_DesertE(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Map_DesertF(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Map_DesertF_Col(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Camp(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Corpse_And_Container(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Container_Col(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Fence_And_Ruin(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Fence_And_Ruin_Col(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Vehicle_And_Wheel(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Building_And_Trash(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Deco_Sign_And_Crane(pArg); });
		
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Environment_Tree1(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Environment_Tree2(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Environment_Grass1(pArg); });
		
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Archi(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Archi_Col(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Bridge(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Bridge_Col(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Building_Ruin(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Building_Ruin_Col(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Building_Ruin_B(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Desert_Building_Ruin_B_Col(pArg); });

		if (m_pGameInstance->bIsClearLevelResource(ENUM_CLASS(m_eNextLevelID)))
		{
			m_strMessage = TEXT("Loading Mesh");
			m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Mesh(pArg); });

			m_strMessage = TEXT("Loading Shader");
			m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Shader(pArg); });

			m_strMessage = TEXT("Loading Effect");
			m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Effect(pArg); });

			m_strMessage = TEXT("Loading Instancing");
			m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_InstanceMesh(pArg); });
			m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Components(pArg); });

			m_strMessage = TEXT("Loading UI");
			m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_UI_For_GamePlay_Level(pArg); });
			m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_UI_For_Combat_HUD_Vitals(pArg); });
			m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_UI_For_Combat_HUD_Skills(pArg); });
			m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_UI_For_World(pArg); });
			m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_UI_For_Popup(pArg); });

			m_strMessage = TEXT("Player Hide and Seek with King JaeHoon");
			m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Scarlet(pArg); });
			m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Player(pArg); });

			hr = Loading_For_GamePlay();
		}
		else
		{
			while (m_pGameInstance->IsWorkThread());
			m_strMessage = TEXT("Loading Complete");
			m_isFinished = true;
			hr = S_OK;
		}
	}
	break;
	case LEVEL::SCARLET:
	{
		m_strMessage = TEXT("Yeahs~  It Just Feeling Like Fudking Sex with My Sexy Girl!");
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Map_Scarlet_Building(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Map_Scarlet_Environment(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Map_Scarlet_Environment2(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_Scarlet_SKY(pArg); });


		if (m_pGameInstance->bIsClearLevelResource(ENUM_CLASS(m_eNextLevelID)))
			hr = Loading_For_Scarlet();
		else
		{
			while (m_pGameInstance->IsWorkThread());
			m_strMessage = TEXT("Loading Complete");
			m_isFinished = true;
			hr = S_OK;
		}
	}

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
	m_strMessage = TEXT("Loading.");

	m_strMessage = TEXT("Loading.");

	m_strMessage = TEXT("Loading.");

	m_strMessage = TEXT("Loading.");
	/*if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	if (FAILED(Loading_UI_For_Logo_Level()))
		return E_FAIL;

	while (m_pGameInstance->IsWorkThread());
	m_strMessage = TEXT("Loading Complete");

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Scarlet()
{
	Sleep(1000.f);

	while (m_pGameInstance->IsWorkThread());
	m_strMessage = TEXT("Loading Complete");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Scarlet_SKY(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Sky_Scarlet");
	pProtoDesc.pPrototype = CSky_Scarlet::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Sky_Scarlet */
	_matrix	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Sky_Scarlet");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Sky/Sky1.bin", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Texture_Sky_Scarlet */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Sky_Scarlet");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Map_Editor/Bin/Resources/Maps/Sky/T_Sky_RockyHills.dds"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Reed */
	PreTransformMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	CVIBuffer_Instance_Model::MODEL_INSTANCE_DESC ReedDesc{};
	ReedDesc.iNumInstance = 10000;
	ReedDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	ReedDesc.vRange = _float3(0.f, 0.f, 0.f);
	ReedDesc.pModelFilePath = "../../Map_Editor/Bin/Resources/Maps/Scarlet/Reed/Reed5.binx";
	ReedDesc.PreModelMatrix = PreTransformMatrix;

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Reed");
	pProtoDesc.pPrototype = CVIBuffer_Instance_Model::Create(m_pDevice, m_pContext, &ReedDesc);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bamboo */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	CVIBuffer_Instance_Model::MODEL_INSTANCE_DESC BambooDesc{};
	BambooDesc.iNumInstance = 1;
	BambooDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	BambooDesc.vRange = _float3(0.f, 0.f, 0.f);
	BambooDesc.pModelFilePath = "../../Map_Editor/Bin/Resources/Maps/Scarlet/Tree/Tree1.binx";
	BambooDesc.PreModelMatrix = PreTransformMatrix;

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bamboo");
	pProtoDesc.pPrototype = CVIBuffer_Instance_Model::Create(m_pDevice, m_pContext, &BambooDesc);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Rock5 */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	CVIBuffer_Instance_Model::MODEL_INSTANCE_DESC Rock5Desc{};
	Rock5Desc.iNumInstance = 1;
	Rock5Desc.vCenter = _float3(0.0f, 0.f, 0.0f);
	Rock5Desc.vRange = _float3(0.f, 0.f, 0.f);
	Rock5Desc.pModelFilePath = "../../Map_Editor/Bin/Resources/Maps/Scarlet/Rock/Rock5.binx";
	Rock5Desc.PreModelMatrix = PreTransformMatrix;

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Rock5");
	pProtoDesc.pPrototype = CVIBuffer_Instance_Model::Create(m_pDevice, m_pContext, &Rock5Desc);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	string szFrontPath = "../Bin/Resources/Models/Character/PC/Eve/Animation/";
	_wstring szPlayerTag = TEXT("Prototype_Component_Model_Eve_Body_24_TypeB");
	_matrix PreMatrix = XMMatrixScaling(0.03, 0.03, 0.03) * XMMatrixRotationY(XMConvertToRadians(270.f));

	vector<_wstring> szPartPrototypeTagList;
	vector<string> szPartModelFilePathList;

	szPartPrototypeTagList.push_back(TEXT("Prototype_Component_Model_Face_Eve"));
	szPartPrototypeTagList.push_back(TEXT("Prototype_Component_Model_Hair_Eve"));
	szPartPrototypeTagList.push_back(TEXT("Prototype_Component_Model_PonyTail_Eve"));


	szPartModelFilePathList.push_back("../Bin/Resources/Models/Character/PC/Eve/CH_P_HEAD_EVE/Eve_Head_v01.binx");
	szPartModelFilePathList.push_back("../Bin/Resources/Models/Character/PC/Eve/CH_HR_EVE/Eve_Hair.binx");
	szPartModelFilePathList.push_back("../Bin/Resources/Models/Character/PC/Eve/CH_PonyTail_EVE/Eve_PonyTail.binx");

	//// 크리스마스
	//if (FAILED(m_pGameInstance->Add_SkeletalPrototype(ENUM_CLASS(LEVEL::GAMEPLAY), m_pDevice, m_pContext,
	//	szPlayerTag, "../Bin/Resources/Models/Character/PC/Eve/CH_P_EVE_Christmas/Eve_Body_Christmas.binx",
	//	szFrontPath, szPartPrototypeTagList, szPartModelFilePathList, PreMatrix)))
	//	return E_FAIL;

	// 오피스 스타일
	if (FAILED(m_pGameInstance->Add_SkeletalPrototype(ENUM_CLASS(LEVEL::GAMEPLAY), m_pDevice, m_pContext,
		szPlayerTag, "../Bin/Resources/Models/Character/PC/Eve/CH_P_EVE_OfficeStyle/Eve_Body_OfficeStyle.binx",
		szFrontPath, szPartPrototypeTagList, szPartModelFilePathList, PreMatrix)))
		return E_FAIL;
	
	dynamic_cast<CModel*>(m_pGameInstance->Get_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), szPlayerTag))->Import_Texture(2, TEXTURE_TYPE::ORSS,
		"../Bin/Resources/Models/Character/PC/Eve/CH_P_EVE_OfficeStyle/CH_EVE_BaseBody_V02_F1_ORSS.dds",
		"g_ORSSTexture", TRUE);

	/// < 모델에 텍스쳐 맵 바인딩 하는 함수 >
	/// 
	/// -> 선행 조건 : 모델을 Add_Prototype하고, Get_Prototype을 통해 가져온다.
	/// -> _uint iMeshIndex = 텍스쳐를 집어넣을 메쉬(머티리얼)의 순서
	/// -> TEXTURE_TYPE eType = 머티리얼의 어느 텍스쳐 타입인지(디퓨즈, 노말, ORM ...)
	/// -> const _char * pTextureFilePath = 텍스쳐 경로
	/// -> const _char * pBindTag = Render에서 머티리얼 셰이더에 바인딩 해줄 때 글로벌 값 어디에 심어줄 지
	/// -> _bool bIsSaved = 갱신된 정보를 binx 파일에 저장해줄지 or 그냥 이 빌드버전에서만 사용할지
	/// 
	/// < /End >

	// ** 예시 테스트 텍스쳐맵 **
	//dynamic_cast<CModel*>(m_pGameInstance->Get_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), szPlayerTag))->Import_Texture(0, TEXTURE_TYPE::DIFFUSE,
	//	"../Bin/Resources/Models/Character/PC/Eve/CH_P_EVE_Model/CH_P_EVE_24_Upper_A.png",
	//	"g_DiffuseTexture", FALSE);
	// 
	//dynamic_cast<CModel*>(m_pGameInstance->Get_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), szPlayerTag))->Import_Texture(1, TEXTURE_TYPE::ORSS,
	//	"../Bin/Resources/Models/Character/PC/Eve/CH_P_EVE_Model/CH_EVE_BaseBody_V02_F1_ORSS.png",
	//	"g_ORSSTexture", TRUE);
	// 
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Eve_CombinedAnimationTest"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Eve_body_psk7th/CH_P_EVE_09_nosimplify.bin", PreMatrix))))
	//	return E_FAIL;

	/// < 모델의 특정 본 이름 바꿔주는 함수 >
	///
	/// -> 선행 조건 : 모델을 Add_Prototype하고, Get_Prototype을 통해 가져온다.
	/// -> const _char* szAfterBoneTag = 바꿀 본 이름
	/// -> vector<string> szTargetTagList = 찾아서 변경해버릴 본 이름들
	/// 
	/// 단, szAfterBoneTag 이름을 가진 본이 이미 있을 경우 이 요청사항은 무시된다.
	/// 또한 szTargetTagList에 있는 본들 중 가장 첫 번째로 탐색된 본만 이름이 변경된다.
	/// 
	/// < /End >

	// ** 예시 코드 **
	// 
	// _wstring szPlayerTag = TEXT("Prototype_Component_Model_Eve_Body_24_TypeB");
	// 
	// vector<string> szTargetTagList;
	// szTargetTagList.push_back("DaeJaeHoon`");
	// szTargetTagList.push_back("sunho");
	// szTargetTagList.push_back("jindol");
	// szTargetTagList.push_back("Chanbin");
	// szTargetTagList.push_back("minseok");
	// szTargetTagList.push_back("jeonghwan");
	// 
	// dynamic_cast<CModel*>(m_pGameInstance->Get_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), szPlayerTag))->Change_BoneTag("Bip001-Head", szTargetTagList);

	

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
		COBBCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		CSphereCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Player*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Face_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Face_Player"),
		CFace_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Hair_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Hair_Player"),
		CHair_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Gorilla"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Gorilla/SuperGorilla.binx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Cinematic_Gorilla"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Cinematic/Gorilla/Cine_Gorilla.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma region Nayitba
	/* For.Prototype_GameObject_Nayitba */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
		CNayitba::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Nayitba_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba_Body"),
		CNayitbaPartBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster_Controller */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_MonsterController"),
		CMonsterController::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MonsterMimesisController */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_MonsterMimesisController"),
		CMonsterMimesisController::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region CINEMATIC_MODEL

	/* For.Prototype_GameObject_CinematicModel_Gorilla */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CinematicModel_Gorilla"),
		CCinematicModel_Gorilla::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CinematicModel_Eve */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CinematicModel_Eve"),
		CCinematicModel_Eve::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	/* For.Prototype_GameObject_Body_LinkAttackTester */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_LinkAttackTester"),
		CBody_LinkAttackTester::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_PonyTail_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_PonyTail_Player"),
		CPonyTail_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*if (FAILED(Loading_UI_For_GamePlay_Level()))
		return E_FAIL;*/

	while (m_pGameInstance->IsWorkThread());
	m_strMessage = TEXT("Loading Complete");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Scarlet(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	string szFrontPath = "../Bin/Resources/Models/Scarlet/Animation/";

	_wstring szPlayerTag = TEXT("Prototype_Component_Model_Scarlet_Body");

	_matrix PreMatrix = XMMatrixScaling(0.035f, 0.035f, 0.035f) * XMMatrixRotationY(XMConvertToRadians(-90.f));

	vector<_wstring> szPartPrototypeTagList;
	vector<string> szPartModelFilePathList;

	if (FAILED(m_pGameInstance->Add_SkeletalPrototype(pProtoDesc.iLevelID, m_pDevice, m_pContext,
		szPlayerTag, "../Bin/Resources/Models/Scarlet/CH_M_Scarlet_Body/CH_M_Scarlet_Body_test03.binx",
		szFrontPath, szPartPrototypeTagList, szPartModelFilePathList, PreMatrix)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Player(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	/* For.Prototype_GameObject_Attack_Hit_Box */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_AttackHitBox");
	pProtoDesc.pPrototype = CAttackHitBox::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	
	/* For.Prototype_Component_Shader_Eve_Face */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Shader_Eve_Face");
	pProtoDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Eve_Face.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	m_pGameManager->Add_Shader(m_eNextLevelID, pProtoDesc.szPrototypeName, static_cast<CShader*>(pProtoDesc.pPrototype));

	/* For.Prototype_Component_Shader_Eve_Hair */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Shader_Eve_Hair");
	pProtoDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Eve_Hair.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	m_pGameManager->Add_Shader(m_eNextLevelID, pProtoDesc.szPrototypeName, static_cast<CShader*>(pProtoDesc.pPrototype));

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Shader_Instance_Model");
	pProtoDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/VTX_InstnaceMesh.hlsl"), VTX_NONEANIM_INSTANCE_DESC::Elements, VTX_NONEANIM_INSTANCE_DESC::iNumElements);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	m_pGameManager->Add_Shader(m_eNextLevelID, pProtoDesc.szPrototypeName, static_cast<CShader*>(pProtoDesc.pPrototype));


	/* For.Prototype_Component_Shader_VtxNorTex */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxNorTex");
	pProtoDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	m_pGameManager->Add_Shader(m_eNextLevelID, pProtoDesc.szPrototypeName, static_cast<CShader*>(pProtoDesc.pPrototype));

	/* For.Prototype_Component_Shader_VtxMesh */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxMesh");
	pProtoDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	m_pGameManager->Add_Shader(m_eNextLevelID, pProtoDesc.szPrototypeName, static_cast<CShader*>(pProtoDesc.pPrototype));


	/* For.Prototype_Component_Shader_VtxAnimMesh */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxAnimMesh");
	pProtoDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	m_pGameManager->Add_Shader(m_eNextLevelID, pProtoDesc.szPrototypeName, static_cast<CShader*>(pProtoDesc.pPrototype));


	/* For.Prototype_Component_Shader_VtxCube */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxCube");
	pProtoDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	m_pGameManager->Add_Shader(m_eNextLevelID, pProtoDesc.szPrototypeName, static_cast<CShader*>(pProtoDesc.pPrototype));


	/* For.Prototype_Component_Shader_VtxRectParticle */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxRectParticle");
	pProtoDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectParticle.hlsl"), VTX_POSTEX_INSTANCE_PARTICLE::Elements, VTX_POSTEX_INSTANCE_PARTICLE::iNumElements);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	m_pGameManager->Add_Shader(m_eNextLevelID, pProtoDesc.szPrototypeName, static_cast<CShader*>(pProtoDesc.pPrototype));

	/* For.Prototype_Component_Shader_VtxPointParticle */
	//pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxPointParticle");
	//pProtoDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointParticle.hlsl"), VTX_POS_INSTANCE_PARTICLE::Elements, VTX_POS_INSTANCE_PARTICLE::iNumElements);
	//if (nullptr == pProtoDesc.pPrototype)
	//	return E_FAIL;
	//Desc->pAddObejct.push_back(pProtoDesc);
	//m_pGameManager->Add_Shader(m_eNextLevelID, pProtoDesc.szPrototypeName, static_cast<CShader*>(pProtoDesc.pPrototype));

	/* For.Prototype_Component_Collider_AABB */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Collider_AABB");
	pProtoDesc.pPrototype = CBoxCollider::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Mesh(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	/* For.Prototype_Component_Texture_Terrain */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Terrain");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Scarlet/Terrain/005_A_TD.dds"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Nayitba_Left_Weapon */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Nayitba_Left_Weapon");
	pProtoDesc.pPrototype = CNaytibaLeftWeaponPart::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Nayitba_Right_Weapon */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Nayitba_Right_Weapon");
	pProtoDesc.pPrototype = CNaytibaRightWeaponPart::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Terrain_ORM");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Scarlet/Terrain/005_A_TAoRM.dds"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_ItemObject */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GamePlay_ItemObject");
	pProtoDesc.pPrototype = CItem::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Texture_Reed_Mask */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Reed_Mask");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Map_Editor/Bin/Resources/Maps/Scarlet/Terrain/ReedMask2.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Moon */
	_matrix PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f)* XMMatrixRotationY(XMConvertToRadians(180.0f));
 
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Item");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Item/Glod/Gold.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	
	/* For.Prototype_Component_Model_Scarlet_Weapon */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Scarlet_Weapon");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Scarlet/CH_M_Scarlet_Weapon/CH_M_Scarlet_Weapon.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Scarlet_Scabbard */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Scarlet_Scabbard");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Scarlet/CH_M_Scarlet_Weapon/CH_M_Scarlet_Scabbard.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Scarlet_Dummy_A */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Scarlet_Dummy_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Scarlet/CH_M_Scarlet_Dummy/Scarlet_Dummy_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Scarlet_Dummy_B */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Scarlet_Dummy_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Scarlet/CH_M_Scarlet_Dummy/Scarlet_Dummy_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Weapon */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f);
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Weapon");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Character/PC/Eve/CH_W_Sword/CH_W_Sword.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CameraBone */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CameraBone");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Camera/CameraBone.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Prob_Box */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Prob_Box1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Box/TypeA/1/Box1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Prob_Box */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Prob_Box2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Box/TypeA/2/Box2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	/* For.Prototype_Component_Model_Prob_CanBox */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CanBox");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Box/CanBox/CanBox.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	PreTransformMatrix = XMMatrixScaling(0.028f, 0.028f, 0.028f) * XMMatrixRotationY(XMConvertToRadians(-90.0f));
	/* For.Prototype_Component_Model_Beholder */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Beholder");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Monster/Beholder/CH_M_NA_51.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	PreTransformMatrix = XMMatrixScaling(0.028f, 0.028f, 0.028f) * XMMatrixRotationY(XMConvertToRadians(-90.0f));
	/* For.Prototype_Component_Model_SunFlower */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_SunFlower");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Monster/SunFlower/SunFlower.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	vector<string> szTargetTagList;
	szTargetTagList.push_back("Haed");
	
	CModel* pModel = static_cast<CModel*>(pProtoDesc.pPrototype);
	pModel->Change_BoneTag("Bip001-Head", szTargetTagList);
	
	PreTransformMatrix = XMMatrixScaling(0.028f, 0.028f, 0.028f) * XMMatrixRotationY(XMConvertToRadians(-90.0f));
	/* For.Prototype_Component_Model_Minion11 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Minion11");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Monster/Minion/11/Minion11.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	
	/* For.Prototype_Component_Model_Bullet_Rock1 */
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bullet_Rock1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Gorilla/Rock/SM_B_GorillaRock_01.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_BanacleA */
	PreTransformMatrix = XMMatrixScaling(0.028f, 0.028f, 0.028f) * XMMatrixRotationY(XMConvertToRadians(-90.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_BanacleA");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Monster/Banacle/CH_M_NA_08.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	
	szTargetTagList.clear();
	szTargetTagList.push_back("Haed001_end");
	
	pModel = static_cast<CModel*>(pProtoDesc.pPrototype);
	pModel->Change_BoneTag("Bip001-Head", szTargetTagList);



	

	/* For.Prototype_Component_Model_StatueA */
	PreTransformMatrix = XMMatrixScaling(0.028f, 0.028f, 0.028f) * XMMatrixRotationY(XMConvertToRadians(-90.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StatueA");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Monster/Statue/A/CH_M_NA_40.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	
	
	/* For.Prototype_Component_Model_StatueB */
	PreTransformMatrix = XMMatrixScaling(0.028f, 0.028f, 0.028f) * XMMatrixRotationY(XMConvertToRadians(-90.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StatueB");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Monster/Statue/B/CH_M_NA_40_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	

	///* For.Prototype_Component_Model_Prob_Vending */
	//pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Prob_Vending");
	//pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Prob/Vending/Vending.binx", PreTransformMatrix);
	//if (nullptr == pProtoDesc.pPrototype)
	//	return E_FAIL;
	//Desc->pAddObejct.push_back(pProtoDesc);

	//Desc->OnCompleted(this_thread::get_id());
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Shader(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);

	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	/* For.Prototype_GameObject_Weapon */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Weapon");
	pProtoDesc.pPrototype = CWeapon::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CameraBone_Player */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CameraBone_Player");
	pProtoDesc.pPrototype = CCameraBone_Player::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Spawner */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Spawner");
	pProtoDesc.pPrototype = CSpawner::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);



	/* For.Prototype_GameObject_Test_InstanceModel */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Test_InstanceModel");
	pProtoDesc.pPrototype = CInstance_Model::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_PxTestProp */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_PxTestProp");
	pProtoDesc.pPrototype = CPxTestProp::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	//Desc->OnCompleted(this_thread::get_id());
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Effect(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	char pattern[MAX_PATH] = {};

	memset(pattern, 0, sizeof(pattern));
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

				PROTOTYPE_DESC PrototypeDesc = {};
				PrototypeDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);
				PrototypeDesc.szPrototypeName = fileName;
				PrototypeDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, szFilePath, PreTransformMatrix);
				Desc->pAddObejct.push_back(PrototypeDesc);
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
				PROTOTYPE_DESC PrototypeDesc = {};
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

				PrototypeDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);
				PrototypeDesc.szPrototypeName = sztProtoName;
				PrototypeDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, szPath, 1);
				Desc->pAddObejct.push_back(PrototypeDesc);
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
				PROTOTYPE_DESC PrototypeDesc = {};
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

				PrototypeDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);
				PrototypeDesc.szPrototypeName = sztProtoName;
				PrototypeDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, szPath, 1);
				Desc->pAddObejct.push_back(PrototypeDesc);
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
				PROTOTYPE_DESC PrototypeDesc = {};
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

				PrototypeDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);
				PrototypeDesc.szPrototypeName = sztProtoName;
				PrototypeDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, szPath, 1);
				Desc->pAddObejct.push_back(PrototypeDesc);
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
				PROTOTYPE_DESC PrototypeDesc = {};
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

				PrototypeDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);
				PrototypeDesc.szPrototypeName = sztProtoName;
				PrototypeDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, szPath, 1);
				Desc->pAddObejct.push_back(PrototypeDesc);
			}
		} while (FindNextFileA(h, &fd));
		FindClose(h);
	}

	PROTOTYPE_DESC PrototypeDesc = {};
	PrototypeDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	/* For.Prototype_Component_Trail */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Trail");
	PrototypeDesc.pPrototype = CTrail::Create(m_pDevice, m_pContext);
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_TrailData */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_TrailData");
	PrototypeDesc.pPrototype = CTrailData::Create(m_pDevice, m_pContext);
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Shader_VtxMeshEffect */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxMeshEffect");
	PrototypeDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshEffect.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements);
	Desc->pAddObejct.push_back(PrototypeDesc);
	m_pGameManager->Add_Shader(m_eNextLevelID, PrototypeDesc.szPrototypeName, static_cast<CShader*>(PrototypeDesc.pPrototype));

	/* For.Prototype_Component_Shader_VtxSpriteParticle */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxSpriteParticle");
	PrototypeDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxSpriteParticle.hlsl"), VTX_POS_INSTANCE_PARTICLE::Elements, VTX_POS_INSTANCE_PARTICLE::iNumElements);
	Desc->pAddObejct.push_back(PrototypeDesc);
	m_pGameManager->Add_Shader(m_eNextLevelID, PrototypeDesc.szPrototypeName, static_cast<CShader*>(PrototypeDesc.pPrototype));

	/* For.Prototype_Component_Shader_VtxMeshParticle */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxMeshParticle");
	PrototypeDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModelParticle.hlsl"), VTX_NONEANIM_INSTANCE_PARTICLE_DESC::Elements, VTX_NONEANIM_INSTANCE_PARTICLE_DESC::iNumElements);
	Desc->pAddObejct.push_back(PrototypeDesc);
	m_pGameManager->Add_Shader(m_eNextLevelID, PrototypeDesc.szPrototypeName, static_cast<CShader*>(PrototypeDesc.pPrototype));

	/* For.Prototype_Component_Shader_VtxTrail */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxTrail");
	PrototypeDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTrailEffect.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	Desc->pAddObejct.push_back(PrototypeDesc);
	m_pGameManager->Add_Shader(m_eNextLevelID, PrototypeDesc.szPrototypeName, static_cast<CShader*>(PrototypeDesc.pPrototype));

	///* For.Prototype_Component_Effect_Slash */
	//PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Slash");
	//PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Slash.binx");
	//Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_SheildBreak_Yellow */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_SheildBreak_Yellow");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/SheildBreakYellow.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Power_Yellow */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Power_Yellow");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/PowerYellow.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);


	///* For.Prototype_Component_Effect_Hit_Spark */
	//PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Hit_Spark");
	//PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Spark.binx");
	//Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Slash_Spark */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Slash_Spark");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/SlashSpark.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Heal */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Heal");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Heal.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Evasion */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Evasion");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Evasion.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_SwordCharge */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_SwordCharge");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/SwordCharge.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_ChargeAttack_AfterSword */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_ChargeAttack_AfterSword");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/ChargeAttackAfterSword.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_SlashSkill */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_SlashSkill");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/SlashSkill.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Shock */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Shock");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Shock.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Shock */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Shock_Final");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Shock_Final.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Parrying */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Parrying");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Parrying.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Perfect_Parrying */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Perfect_Parrying");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Perfect_Parrying.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Sand_Hit */
	// 모래 내려찍었을때 나오는 이펙트
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Sand_Hit");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/SandHit.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Gorilla_Jump_Attack */
	// 고릴라가 점프해서 내려찍는 공격 이펙트
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Gorilla_Jump_Attack");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/GorillaMarioAttack.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Stone_Shrowing */
	// 고릴라가 던지는 돌에 붙이는 이펙트
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Stone_Shrowing");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Stone_Shrowing.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Stone_Shrowing */
	// 고릴라가 던지는 돌에 붙이는 이펙트
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Stone_Floor_Shrowing");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Stone_Floor_Shrowing.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);
	
	/* For.Prototype_Component_Effect_Sword_TakeIn */
	// 검 집어 넣었을때 나오는 파티클
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Sword_TakeIn");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/SwordTakeIn.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Sword_TakeOut */
	// 검 꺼냈을때 검에 붙는 전기
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Sword_TakeOut");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/SwordTakeOut.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Blood */
	// 방향 상관없는 피
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Blood");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Blood.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Slash_Blood */
	// 검으로 벴을때 검 방향으로 넣어야하는 피
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Slash_Blood");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/SlashBlood.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Monster_Club */
	// 몬스터 둔기 피 이펙트
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Monster_Club");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Monster_Club.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Blood_Hit */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Blood_Hit");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Blood_Hit.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Monster_Sting */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Monster_Sting");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Monster_Sting.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Ashes */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Ashes");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Ashes.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Gigas_Dead */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Gigas_Dead");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/GigasDead.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Item_Aura */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Item_Aura");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/ItemAura.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Item_Get */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Item_Get");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/ItemGet.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Rock_Hit */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Rock_Hit");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/RockHit.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Sakura */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Sakura");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Sakura.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);
	
	
	/* For.Prototype_Component_TrailEffect_Default_Slash */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_TrailEffect_Default_Slash");
	PrototypeDesc.pPrototype = CTrailEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/TrailEffect/Default_Slash.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);


	/* For.Prototype_Component_TrailEffect_Monster_Slash */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_TrailEffect_Monster_Slash");
	PrototypeDesc.pPrototype = CTrailEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/TrailEffect/Monster_Slash.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	//Desc->OnCompleted(this_thread::get_id());

	return S_OK;
}


HRESULT CLoader::Loading_For_GamePlay_Map(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	/* SKY BOX*/
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Sky");
	pProtoDesc.pPrototype = CSky::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Sky");
	


	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Texture_Sky1 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Sky1");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Map_Editor/Bin/Resources/Maps/Sky/Panorama_Sky_06-512x512.dds"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);



	/* For.Prototype_Component_Model_Sky */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Sky");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Sky/Sky3.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	


	/* For.Prototype_Component_Texture_Terrain */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Terrain_Desert");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Map_Editor/Bin/Resources/Maps/Desert/Terrain/009_A_TD.dds"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Texture_Terrain_Red */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Terrain_Desert_Red");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Map_Editor/Bin/Resources/Maps/Desert/Terrain/004_A_TD.dds"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Texture_Terrain_Green */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Terrain_Desert_Green");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Map_Editor/Bin/Resources/Maps/Desert/Terrain/003_A_TD.dds"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Texture_Terrain_Mask */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Terrain_Mask");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Map_Editor/Bin/Resources/Maps/Desert/Terrain/TerrainMask2.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Texture_Terrain_ORM */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_ORM_Texture_Terrain_Desert");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Map_Editor/Bin/Resources/Maps/Desert/Terrain/002_A_TAoRM.dds"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Terrain */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Terrain_Desert");
	pProtoDesc.pPrototype = CTerrain_Desert::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Navigation");
	pProtoDesc.pPrototype = CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation2.bin"));
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* Prototype_GameObject_Prob_Destory */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Prob_Destory");
	pProtoDesc.pPrototype = CProb_Destory::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* Prototype_GameObject_Prob_Static */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Prob_Static");
	pProtoDesc.pPrototype = CProb_Static::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* Prototype_GameObject_Interaction_NonAnim */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Interaction_NonAnim");
	pProtoDesc.pPrototype = CStaticInteraction::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/*CanBox*/
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CanBox");
	pProtoDesc.pPrototype = CCanBox::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	//Desc->OnCompleted(this_thread::get_id());
	return S_OK;
}

HRESULT CLoader::Loading_For_Map_Scarlet_Environment(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	/* For.Prototype_Component_Model_Stone1 */
	_matrix PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stone1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Stone/Stone1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);



	/* For.Prototype_Component_Model_Stone2 */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stone2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Stone/Stone2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stone3 */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stone3");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Stone/Stone3.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stone4 */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stone4");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Stone/Stone4.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_StoneWall1 */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StoneWall1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Stone/StoneWall1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_StoneWall2 */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StoneWall2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Stone/StoneWall2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Rock1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Rock1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Rock/Rock1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Rock2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Rock2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Rock/Rock2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Rock3 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Rock3");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Rock/Rock3.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Rock4 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Rock4");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Rock/Rock4.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Rock6 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Rock6");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Rock/Rock6.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Rock7 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Rock7");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Rock/Rock7.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Rock8 */
	PreTransformMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Rock8");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Rock/Rock8.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_StoneTile */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StoneTile");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Tile/StoneTile.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CherryBlossom1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CherryBlossom1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Tree/CherryBlossom1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CherryBlossom2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CherryBlossom2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Tree/CherryBlossom2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CherryBlossom3 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CherryBlossom3");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Tree/CherryBlossom3.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CherryBlossom4 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CherryBlossom4");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Tree/CherryBlossom4.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Grass */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Grass");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Grass/Grass.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_DryGrass1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_DryGrass1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Grass/DryGrass1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_DryGrass2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_DryGrass2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Grass/DryGrass2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_DryGrass3 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_DryGrass3");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Grass/DryGrass3.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Rock1 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Rock1");
	pProtoDesc.pPrototype = CRock1::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Rock2 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Rock2");
	pProtoDesc.pPrototype = CRock2::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Rock3 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Rock3");
	pProtoDesc.pPrototype = CRock3::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Rock4 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Rock4");
	pProtoDesc.pPrototype = CRock4::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Rock5 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Rock5");
	pProtoDesc.pPrototype = CRock5::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Rock6 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Rock6");
	pProtoDesc.pPrototype = CRock6::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Rock7 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Rock7");
	pProtoDesc.pPrototype = CRock7::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Rock8 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Rock8");
	pProtoDesc.pPrototype = CRock8::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Reed */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Reed");
	pProtoDesc.pPrototype = CReed::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Bamboo */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Bamboo");
	pProtoDesc.pPrototype = CBamboo::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_StoneWall1 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_StoneWall1");
	pProtoDesc.pPrototype = CStoneWall1::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_StoneWall2 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_StoneWall2");
	pProtoDesc.pPrototype = CStoneWall2::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Grass */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Grass");
	pProtoDesc.pPrototype = CGrass::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_DryGrass1 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_DryGrass1");
	pProtoDesc.pPrototype = CDryGrass1::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_DryGrass2 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_DryGrass2");
	pProtoDesc.pPrototype = CDryGrass2::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_DryGrass3 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_DryGrass3");
	pProtoDesc.pPrototype = CDryGrass3::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CherryBlossom1 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CherryBlossom1");
	pProtoDesc.pPrototype = CCherryBlossom1::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CherryBlossom2 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CherryBlossom2");
	pProtoDesc.pPrototype = CCherryBlossom2::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CherryBlossom3 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CherryBlossom3");
	pProtoDesc.pPrototype = CCherryBlossom3::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CherryBlossom4 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CherryBlossom4");
	pProtoDesc.pPrototype = CCherryBlossom4::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	

	//Desc->OnCompleted(this_thread::get_id());

#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_For_Map_Scarlet_Environment2(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	/* For.Prototype_Component_Model_CM_Rock1 */
	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Common/Rock/CM_Rock1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Common/Rock/CM_Rock2.binx", PreTransformMatrix);

	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	/* For.Prototype_Component_Model_CM_Rock3 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock3");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Common/Rock/CM_Rock3.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	/* For.Prototype_Component_Model_CM_Rock4 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock4");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Common/Rock/CM_Rock4.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock6 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock6");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Common/Rock/CM_Rock6.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock7 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock7");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Common/Rock/CM_Rock7.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock8 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock8");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Common/Rock/CM_Rock8.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);



	/* For.Prototype_Component_Model_CM_Rock9 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock9");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Common/Rock/CM_Rock9.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock10 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock10");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Common/Rock/CM_Rock10.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock11 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock11");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Common/Rock/CM_Rock11.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock12 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock12");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Common/Rock/CM_Rock12.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock13 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock13");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Common/Rock/CM_Rock13.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock14 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock14");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Common/Rock/CM_Rock14.binx", PreTransformMatrix);

	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

#pragma region Map_Common
	/* For.Prototype_GameObject_CM_Rock1 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CM_Rock1");
	pProtoDesc.pPrototype = CCM_Rock1::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CM_Rock2 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CM_Rock2");
	pProtoDesc.pPrototype = CCM_Rock2::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CM_Rock3 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CM_Rock3");
	pProtoDesc.pPrototype = CCM_Rock3::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CM_Rock4 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CM_Rock4");
	pProtoDesc.pPrototype = CCM_Rock4::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CM_Rock5 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CM_Rock5");
	pProtoDesc.pPrototype = CCM_Rock5::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CM_Rock6 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CM_Rock6");
	pProtoDesc.pPrototype = CCM_Rock6::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CM_Rock7 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CM_Rock7");
	pProtoDesc.pPrototype = CCM_Rock7::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CM_Rock8 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CM_Rock8");
	pProtoDesc.pPrototype = CCM_Rock8::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CM_Rock9 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CM_Rock9");
	pProtoDesc.pPrototype = CCM_Rock9::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CM_Rock10 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CM_Rock10");
	pProtoDesc.pPrototype = CCM_Rock10::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CM_Rock11 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CM_Rock11");
	pProtoDesc.pPrototype = CCM_Rock11::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CM_Rock12 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CM_Rock12");
	pProtoDesc.pPrototype = CCM_Rock12::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CM_Rock13 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CM_Rock13");
	pProtoDesc.pPrototype = CCM_Rock13::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_CM_Rock14 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_CM_Rock14");
	pProtoDesc.pPrototype = CCM_Rock14::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
#pragma endregion 
	return S_OK;
}

HRESULT CLoader::Loading_For_Map_Scarlet_Building(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	/*InstanceModel*/
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_InstanceModel");
	pProtoDesc.pPrototype = CInstanceModel::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_TombStone */
	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_TombStone");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/TombStone/tombstone.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Inscription_L */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Inscription_L");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/TombStone/TombStone_L.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Inscription_R */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Inscription_R");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/TombStone/TombStone_R.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_TombStoneBase1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_TombStoneBase1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/TombStone/TombStoneBase1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_TombStoneBase2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_TombStoneBase2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/TombStone/TombStoneBase2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stair */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stair");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Stair/Stair.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Giwajip */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Giwajip");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Giwajip/Giwajip.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Giwajip2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Giwajip2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/Giwajip/Giwajip2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_StoneLantern1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StoneLantern1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/StoneLantern/StoneLantern1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_StoneLantern2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StoneLantern2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Scarlet/StoneLantern/StoneLantern2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Stone1 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Stone1");
	pProtoDesc.pPrototype = CStone1::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);



	/* For.Prototype_GameObject_Stone2 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Stone2");
	pProtoDesc.pPrototype = CStone2::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Stone3 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Stone3");
	pProtoDesc.pPrototype = CStone3::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Stone4 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Stone4");
	pProtoDesc.pPrototype = CStone4::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Inscription_L */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Inscription_L");
	pProtoDesc.pPrototype = CInscription_L::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Inscription_R */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Inscription_R");
	pProtoDesc.pPrototype = CInscription_R::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_TombStone */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_TombStone");
	pProtoDesc.pPrototype = CTombStone::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_TombStoneBase1 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_TombStoneBase1");
	pProtoDesc.pPrototype = CTombStoneBase1::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_TombStoneBase2 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_TombStoneBase2");
	pProtoDesc.pPrototype = CTombStoneBase2::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Stair */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Stair");
	pProtoDesc.pPrototype = CStair::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_StoneTile */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_StoneTile");
	pProtoDesc.pPrototype = CStoneTile::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Giwajip */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Giwajip");
	pProtoDesc.pPrototype = CGiwajip::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Giwajip2 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Giwajip2");
	pProtoDesc.pPrototype = CGiwajip2::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_StoneLantern1 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_StoneLantern1");
	pProtoDesc.pPrototype = CStoneLantern1::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_StoneLantern2 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_StoneLantern2");
	pProtoDesc.pPrototype = CStoneLantern2::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	//Desc->OnCompleted(this_thread::get_id());

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_InstanceMesh(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	/* For.Prototype_Component_VIBuffer_Cube */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_VIBuffer_Cube");
	pProtoDesc.pPrototype = CVIBuffer_Cube::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_VIBuffer_Particle_Snow */
	CVIBuffer_Rect_Instance::RECT_INSTANCE_DESC		SnowDesc{};
	SnowDesc.iNumInstance = 3000;
	SnowDesc.vCenter = _float3(64.0f, 20.f, 64.0f);
	SnowDesc.vRange = _float3(128.f, 2.f, 128.f);
	SnowDesc.vSize = _float2(0.2f, 0.6f);
	SnowDesc.vLifeTime = _float2(3.f, 7.f);
	SnowDesc.vSpeed = _float2(0.f, 0.f);
	SnowDesc.isLoop = true;

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_VIBuffer_Particle_Snow");
	pProtoDesc.pPrototype = CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, &SnowDesc);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_ComputeShader_Snow");
	pProtoDesc.pPrototype = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_Rect_Drop.hlsl"), "CS", SnowDesc.iNumInstance);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_VIBuffer_Particle_Explosion */
	CVIBuffer_Point_Instance::POINT_INSTANCE_DESC		ExplosionDesc{};
	ExplosionDesc.iNumInstance = 1000;
	ExplosionDesc.vCenter = _float3(0.0f, 1.f, 0.0f);
	ExplosionDesc.vPivot = _float3(0.0f, 0.f, 0.0f);
	ExplosionDesc.vRange = _float3(0.5f, 0.5f, 0.5f);
	ExplosionDesc.vSize = _float2(0.2f, 0.6f);
	ExplosionDesc.vLifeTime = _float2(3.f, 7.f);
	ExplosionDesc.vSpeed = _float2(2.f, 5.f);
	ExplosionDesc.isLoop = true;

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_VIBuffer_Particle_Explosion");
	pProtoDesc.pPrototype = CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &ExplosionDesc);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_ComputeShader_Expolosion");
	pProtoDesc.pPrototype = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_Spread.hlsl"), "CS", ExplosionDesc.iNumInstance);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	_matrix PreTransformMatrix = XMMatrixIdentity();
	/* For.Prototype_Component_Model_Fiona */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Fiona");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Fiona/Fiona.bin", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	//PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//CVIBuffer_Instance_Model::MODEL_INSTANCE_DESC ModelDesc{};
	//ModelDesc.iNumInstance = 100;
	//ModelDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	//ModelDesc.vRange = _float3(50.f, 5.f, 50.f);
	//ModelDesc.pModelFilePath = "../Bin/Resources/Models/Dororong/CH_NPC_Dororong.bin";
	//ModelDesc.PreModelMatrix = PreTransformMatrix;
	//
	//pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Instance_Model_Dororong");
	//pProtoDesc.pPrototype = CVIBuffer_Instance_Model::Create(m_pDevice, m_pContext, &ModelDesc);
	//if (nullptr == pProtoDesc.pPrototype)
	//	return E_FAIL;
	//Desc->pAddObejct.push_back(pProtoDesc);


	


	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Components(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	/* For.Prototype_Component_Interaction */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Interaction");
	pProtoDesc.pPrototype = CInteraction_Component::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_MonsterFSM */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Monster_FSM");
	pProtoDesc.pPrototype = CMonsterFSM::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_AISence */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_AISence");
	pProtoDesc.pPrototype = CAISenceComponent::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_TargetComponent */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_TargetComponent");
	pProtoDesc.pPrototype = CTargetComponent::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_BossController */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_BossController");
	pProtoDesc.pPrototype = CBossController::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_GorillaBehavior */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_GorillaBehaviorTree");
	pProtoDesc.pPrototype = CGorillaBehaviorTree::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_ScarletBehavior */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_ScarletBehaviorTree");
	pProtoDesc.pPrototype = CScarletBehaviorTree::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Bullet_Rock */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_RockBullet");
	pProtoDesc.pPrototype = CBullet_Rock::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_ScarletBullet_Rock */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_ScarletBullet");
	pProtoDesc.pPrototype = CBullet_Scarlet::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_VIBuffer_Terrain */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_VIBuffer_Terrain");
	pProtoDesc.pPrototype = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Scarlet/Terrain/Height3.bmp"));
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_VIBuffer_Terrain_Desert");
	pProtoDesc.pPrototype = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Map_Editor/Bin/Resources/Maps/Desert/Terrain/Height2.bmp"));
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_DropComponent */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_DropComponent");
	pProtoDesc.pPrototype = CDropComponent::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Map_DesertA(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);
	_matrix PreTransformMatrix = {};

	/* For.Prototype_Component_Model_Canyon_1A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_2A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_2A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_2A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Pallet02 */
	PreTransformMatrix = XMMatrixScaling(0.0003f, 0.0003f, 0.0003f);
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Pallet02");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Pallet/Pallet02.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Poster02 */
	PreTransformMatrix = XMMatrixScaling(0.0003f, 0.0003f, 0.0003f);
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Poster02");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Desert/Deco/Poster/Poster02.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_3A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_3A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_3A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_4A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_4A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_4A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_5A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_5A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_5A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_6A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_6A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_6A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_12A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_12A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_12A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_14A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_14A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_14A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_14B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_14B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_14B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_15A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_15A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_15A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_16A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_16A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_16A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_16B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_16B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_16B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_17A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_17A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_17A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_17B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_17B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_17B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_18A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_18A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_18A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_50A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_50A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Map_DesertB(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Canyon_52A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_52A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_52A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_55A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_55A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_55A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_58A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_58A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_58A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_59A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_59A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_59A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_Component_Model_Canyon_60A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_60A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_60A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_Component_Model_Canyon_61A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_61A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_61A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_65A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_65A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_65A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_66A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_66A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_66A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_67A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_67A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_67A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_69A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_69A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_69A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_71A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_71A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_71A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Map_DesertC(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Canyon_20A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_20A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_20A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_21A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_21A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_21A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_22A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_22A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_22A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_23A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_23A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_23A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_24A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_24A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_24A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_35A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_35A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_35A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_39A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_39A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_39A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_39C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_39C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_39C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_39D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_39D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_39D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_43A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_43A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_43A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_44A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_44A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_44A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_46A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_46A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_46A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Map_DesertD(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Canyon_100A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_100A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_100A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_101A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_101A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_101A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_103A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_103A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_103A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_104A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_104A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_104A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_105A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_105A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_105A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_106A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_106A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_106A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_108A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_108A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_108A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_109A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_109A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_109A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_110A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_110A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_110A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_111A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_111A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_111A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_112A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_112A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_112A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_113A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_113A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_113A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_115A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_115A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_115A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_116A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_116A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_116A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_117A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_117A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_117A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_121A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_121A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_121A.binx", PreTransformMatrix);
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

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Map_DesertE(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Canyon_127A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_127A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_127A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_127B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_127B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_127B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_127C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_127C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_127C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_127D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_127D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_127D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_127E */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_127E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_127E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_128A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_128A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_128A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_128B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_128B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_128B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_128C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_128C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_128C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_131A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_131A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_131A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_132A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_132A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_132A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_132B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_132B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_132B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_132C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_132C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_132C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_133B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_133B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_133B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Map_DesertF(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);
	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Canyon_80A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_80A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_80A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_81A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_81A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_81A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_93A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_93A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_93A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_95A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_95A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_95A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_96A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_96A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_96A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_97A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_97A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_97A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_98A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_98A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_98A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_122A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_122A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_122A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_123A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_123A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_123A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_125A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_125A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_125A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_126A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_126A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_126A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Building_Ruin(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	/* For.Prototype_Component_Model_Door_A */
	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_E */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_G */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_G");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_G.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_H */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_H");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_H.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_I */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_I");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_I.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_J */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_J");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_J.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_N */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_N");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_N.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_P */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_P");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_P.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_R */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_R");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_R.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Floor_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Floor_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Floor_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Floor_D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_E */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Floor_E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_F */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Floor_F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_G */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_G");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Floor_G.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_H */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_H");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Floor_H.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Frame_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Frame_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Frame_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Frame_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Frame_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Frame_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Ruin_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Ruin_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Ruin_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Ruin_D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Building_Ruin_Col(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	/* For.Prototype_Component_Model_Door_A */
	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_B_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_C_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_C_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_D_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_D_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_E */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_E_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_E_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_G */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_G_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_G_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_H */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_H_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_H_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_I */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_I_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_I_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_J */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_J_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_J_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_N */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_N_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_N_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_P */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_P_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_P_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Door_R */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Door_R_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Door_R_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Floor_A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Floor_B_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_C_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Floor_C_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_D_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Floor_D_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_E */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_E_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Floor_E_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_F */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_F_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Floor_F_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_G */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_G_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Floor_G_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor_H */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor_H_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Floor_H_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Frame_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Frame_A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Frame_A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Frame_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Frame_B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Frame_B_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Ruin_A.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Ruin_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_C_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Ruin_C_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_D_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Ruin_D_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Building_Ruin_B(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Ruin_Building_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_Building_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Ruin_Building_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_Building_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_Building_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Ruin_Building_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_Building_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_Building_C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Ruin_Building_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stone009_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stone009_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Stone009_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stone009_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stone009_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Stone009_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_D_1 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_D_1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_D_1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_E */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_F */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_G */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_G");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_G.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_H */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_H");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_H.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_I */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_I");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_I.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall007_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall007_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall007_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall007_D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_E */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall007_E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_F */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall007_F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_G */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_G");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall007_G.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Building_Ruin_B_Col(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Ruin_Building_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_Building_A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Ruin_Building_A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_Building_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_Building_B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Ruin_Building_B_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_Building_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_Building_C_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Ruin_Building_C_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stone009_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stone009_A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Stone009_A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stone009_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stone009_B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Stone009_B_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_B_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_C_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_C_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_D_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_D_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_D_1 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_D_1_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_D_1_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_E */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_E_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_E_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_F */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_F_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_F_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_G */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_G_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_G_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_H */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_H_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_H_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall_I */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall_I_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall_I_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall007_A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_B */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall007_B_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_C */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_C_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall007_C_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_D */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_D_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall007_D_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_E */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_E_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall007_E_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_F */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_F_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall007_F_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wall007_G */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wall007_G_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Building_Ruin/Wall007_G_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Interaction_Vending7A */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Interaction_Vending7A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Interaction/VendingMachine/VendingMachine_7A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Interaction_Chair117 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Interaction_Chair117");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Interaction/Chair/Chair117_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Lamp_47A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Lamp_47A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Lamp/Lamp_47A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_VendingMachine_6A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_VendingMachine_6A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/VendingMachine/VendingMachine_6A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_VendingMachine_7A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_VendingMachine_7A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/VendingMachine/VendingMachine_7A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_SciFi_Door */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_SciFi_Door");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Door/SciFi_Door.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_SciFi_Door_COL */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_SciFi_Door_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Door/SciFi_Door_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Poster_4D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Poster_4D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Poster/Poster_4D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_GameObject_Deco */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Deco");
	pProtoDesc.pPrototype = CDeco::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_GameObject_SciFi_Door */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_SciFi_Door");
	pProtoDesc.pPrototype = CSciFi_Door::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Desert_Architecture */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Architecture");
	pProtoDesc.pPrototype = CDesert_Architecture::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Camp(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Base_1A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Base_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Base_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- CAMP_1B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Camp_1B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1D
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Camp_1D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1E
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Camp_1E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1F
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Camp_1F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1G
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1G");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Camp_1G.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1H
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1H");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Camp_1H.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1I
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1I");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Camp_1I.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1J
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1J");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Camp_1J.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1K
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1K");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Camp_1K.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1L
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1L");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Camp_1L.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1M
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1M");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Camp_1M.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1N
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1N");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Camp_1N.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1R
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1R");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Camp_1R.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1S
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1S");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Camp_1S.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1T
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1T");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Camp_1T.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- Camp_1W
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Camp_1W");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Camp_1W.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- REPAIRCONSOLE
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_RepairConsole");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/RepairConsole.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TOP_ROOF
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Top_Roof");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Top_Roof.binx", PreTransformMatrix);
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
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Corpse_1A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Corpse_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Corpse/Corpse_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Corpse_1B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Corpse_1B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Corpse/Corpse_1B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Corpse_2A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Corpse_2A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Corpse/Corpse_2A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Corpse_2B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Corpse_2B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Corpse/Corpse_2B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Corpse_2C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Corpse_2C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Corpse/Corpse_2C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Corpse_3A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Corpse_3A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Corpse/Corpse_3A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_2A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_2A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_2A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_2B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_2B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_2B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_2C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_2C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_2C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_2D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_2D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_2D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_3A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_3A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_3A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_4B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_4B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_4B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_5A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_5A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_5A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_5B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_5B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_5B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_5C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_5C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_5C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_5D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_5D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_5D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_5E */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_5E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_5E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_7B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_7B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_7B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_7F */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_7F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_7F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Container_Col(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	/* For.Prototype_Component_Model_Base_1A */
	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Base_1A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Camp/Base_1A_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_2A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_2A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_2A_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_2B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_2B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_2B_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_2C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_2C_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_2C_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_2D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_2D_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_2D_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_3A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_3A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_3A_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_4B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_4B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_4B_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_5A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_5A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_5A_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_5B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_5B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_5B_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_5C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_5C_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_5C_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_5D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_5D_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_5D_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_5E */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_5E_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_5E_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_7B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_7B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_7B_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Container_7F */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Container_7F_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Container/Container_7F_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Fence_And_Ruin(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Fence_1A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Fence_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Fence/Fence_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Fence_1B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Fence_1B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Fence/Fence_1B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Fence_1F */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Fence_1F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Fence/Fence_1F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Fence_1H */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Fence_1H");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Fence/Fence_1H.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_7A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_7A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Ruin/Ruin_7A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_7B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_7B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Ruin/Ruin_7B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_7C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_7C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Ruin/Ruin_7C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_7D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_7D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Ruin/Ruin_7D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Ruin_7E */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Ruin_7E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Ruin/Ruin_7E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Fence_And_Ruin_Col(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Fence_1A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Fence_1A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Fence/Fence_1A_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Fence_1B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Fence_1B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Fence/Fence_1B_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Fence_1F */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Fence_1F_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Fence/Fence_1F_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Fence_1H */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Fence_1H_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Fence/Fence_1H_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Vehicle_And_Wheel(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Vehicle_2A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Vehicle_2A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Vehicle/Vehicle_2A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Vehicle_2B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Vehicle_2B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Vehicle/Vehicle_2B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Vehicle_3B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Vehicle_3B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Vehicle/Vehicle_3B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Vehicle_4C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Vehicle_4C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Vehicle/Vehicle_4C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Vehicle_6A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Vehicle_6A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Vehicle/Vehicle_6A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Vehicle_8B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Vehicle_8B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Vehicle/Vehicle_8B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Vehicle_14A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Vehicle_14A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Vehicle/Vehicle_14A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Vehicle_14B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Vehicle_14B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Vehicle/Vehicle_14B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wheel_1A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wheel_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Wheel/Wheel_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wheel_1B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wheel_1B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Wheel/Wheel_1B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Wheel_1C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Wheel_1C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Wheel/Wheel_1C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Building_And_Trash(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Building_4B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Building_4B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Building/Building_4B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Building_4C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Building_4C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Building/Building_4C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Building_4D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Building_4D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Building/Building_4D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Trash_1A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Trash_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Trash/Trash_1A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Trash_2A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Trash_2A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Trash/Trash_2A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Trash_2B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Trash_2B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Trash/Trash_2B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Trash_4A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Trash_4A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Trash/Trash_4A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Trash_9A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Trash_9A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Trash/Trash_9A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Trash_17A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Trash_17A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Trash/Trash_17A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Deco_Sign_And_Crane(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Sign_11A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Sign_11A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Sign/Sign_11A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Sign_11B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Sign_11B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Sign/Sign_11B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Sign_11F */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Sign_11F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Sign/Sign_11F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Sign_12B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Sign_12B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Sign/Sign_12B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Sign_12C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Sign_12C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Sign/Sign_12C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Sign_36B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Sign_36B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Sign/Sign_36B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Crane_1C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Crane_1C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Crane/Crane_1C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Crane_11 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Crane_11");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Crane/Crane_11.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Crane_13 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Crane_13");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Deco/Crane/Crane_13.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Environment_Tree1(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Tree_1A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_1A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree1_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_2A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_2A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree2_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_3A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_3A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree3_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_4A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_4A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree4_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_5B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_5B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree5_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_5C
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_5C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree5_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_6A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_6A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree6_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_6B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_6B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree6_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_7A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_7A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree7_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_7B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_7B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree7_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_8A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_8A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree8_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_8Aa
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_8Aa");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree8_Aa.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_8B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_8B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree8_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_8Ba
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_8Ba");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree8_Ba.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_10A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_10A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree10_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_11B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_11B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree11_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_15A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_15A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree15_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_15B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_15B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree15_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_16A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_16A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree16_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_17A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_17A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree17_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_18A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_18A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree18_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_19A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_19A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree19_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_20A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_20A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree20_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_21A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_21A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree21_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_23A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_23A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree23_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_25A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_25A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree25_A.binx", PreTransformMatrix);
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
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	// --- TREE_26A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_26A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree26_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_27A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_27A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree27_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_29A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_29A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree29_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_30A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_30A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree30_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_31A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_31A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree31_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_32A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_32A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree32_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_33A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_33A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree33_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_34A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_34A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree34_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree40_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40C
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree40_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40D
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree40_D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40E
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree40_E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40F
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree40_F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40G
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40G");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree40_G.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40H
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40H");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree40_H.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40I
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40I");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree40_I.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_40J
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_40J");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree40_J.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_42A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_42A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree42_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_43A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_43A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree43_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_43B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_43B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree43_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_44A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_44A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree44_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_44B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_44B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree44_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_44D
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_44D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree44_D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_44F
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_44F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree44_F.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_45A
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_45A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree45_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_45B
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_45B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree45_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// --- TREE_45C
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Tree_45C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Tree/Tree45_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype) return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Environment_Grass1(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	CVIBuffer_Instance_Model::MODEL_INSTANCE_DESC GrassDesc{};
	GrassDesc.iNumInstance = 1;
	GrassDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	GrassDesc.vRange = _float3(0.f, 0.f, 0.f);
	GrassDesc.pModelFilePath = "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/DeadShrubs/DeadShrubs_A.binx";
	GrassDesc.PreModelMatrix = PreTransformMatrix;

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_DeadShrubs_A");
	pProtoDesc.pPrototype = CVIBuffer_Instance_Model::Create(m_pDevice, m_pContext, &GrassDesc);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_Component_Model_DeadShrubs_B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	GrassDesc.iNumInstance = 1;
	GrassDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	GrassDesc.vRange = _float3(0.f, 0.f, 0.f);
	GrassDesc.pModelFilePath = "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/DeadShrubs/DeadShrubs_B.binx";
	GrassDesc.PreModelMatrix = PreTransformMatrix;

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_DeadShrubs_B");
	pProtoDesc.pPrototype = CVIBuffer_Instance_Model::Create(m_pDevice, m_pContext, &GrassDesc);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_GameObject_Desert_Grass */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Desert_Grass");
	pProtoDesc.pPrototype = CDesert_Grass::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Archi(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Lift_Controller */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Lift_Controller");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Lift/Lift_Controller.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	
	// 충돌용 메시입니다 다른 모델 넣듯이 똑같이 추가해주시면 돼요 
	// 텍스쳐같은건 신경 안쓰셔도됩니다.
	// 어차피 렌더링 안되고 그냥 충돌처리만 할 메시라서 
	
	

	/* For.Prototype_Component_Model_Lift_Body */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Lift_Body");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Lift/Lift_Body.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Lift_Platform */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Lift_Platform");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Lift/Lift_Platform.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Floor/Floor7_A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Floor/Floor7_B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_C");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Floor/Floor7_C.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Floor/Floor7_D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_E */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_E");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Floor/Floor7_E.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_F */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_F");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Floor/Floor7_F.binx", PreTransformMatrix);
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

HRESULT CLoader::Loading_For_Desert_Archi_Col(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Lift_Platform_COL */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Lift_Platform_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Lift/Lift_Platform_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Lift_Body_COL */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Lift_Body_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Lift/Lift_Body_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Lift_Controller_COL */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Lift_Controller_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Lift/Lift_Controller_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_A_COL */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Floor/Floor7_A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_B_COL */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Floor/Floor7_B_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_C_COL */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_C_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Floor/Floor7_C_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_D_COL */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_D_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Floor/Floor7_D_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_E_COL */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_E_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Floor/Floor7_E_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Floor7_F_COL */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Floor7_F_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Floor/Floor7_F_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Map_DesertA_Col(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);
	_matrix PreTransformMatrix = {};

	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Moon");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Sky/Moon2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Moon */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Moon");
	pProtoDesc.pPrototype = CMoon::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_1A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_1A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_1A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_2A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_2A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_2A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_3A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_3A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_3A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_4A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_4A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_4A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_5A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_5A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_5A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_6A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_6A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_6A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_12A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_12A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_12A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_14A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_14A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_14A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_14B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_14B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_14B_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_15A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_15A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_15A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_16A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_16A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_16A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_16B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_16B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_16B_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_17A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_17A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_17A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_17B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_17B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_17B_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_18A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_18A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_18A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_50A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_50A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_50A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Map_DesertB_Col(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	/* For.Prototype_Component_Model_CM_Rock14 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock14");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock14.binx", PreTransformMatrix);

	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	/* For.Prototype_Component_Model_Canyon_52A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_52A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_52A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_55A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_55A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_55A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_58A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_58A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_58A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_59A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_59A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_59A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_Component_Model_Canyon_60A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_60A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_60A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_Component_Model_Canyon_61A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_61A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_61A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_65A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_65A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_65A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_66A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_66A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_66A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_67A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_67A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_67A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_69A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_69A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_69A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_71A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_71A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_71A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Map_DesertC_Col(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Canyon_20A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_20A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_20A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_21A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_21A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_21A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_22A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_22A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_22A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_23A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_23A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_23A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_24A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_24A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_24A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_35A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_35A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_35A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_39A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_39A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_39A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_39C */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_39C_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_39C_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_39D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_39D_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_39D_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_43A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_43A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_43A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Map_DesertD_Col(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Canyon_100A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_100A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_100A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_101A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_101A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_101A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_103A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_103A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_103A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_104A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_104A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_104A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_105A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_105A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_105A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_106A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_106A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_106A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_108A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_108A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_108A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_109A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_109A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_109A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_110A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_110A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_110A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_111A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_111A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_111A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_112A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_112A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_112A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_113A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_113A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_113A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_115A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_115A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_115A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_116A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_116A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_116A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_117A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_117A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_117A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_121A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_121A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_121A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_131A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_131A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_131A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Map_DesertF_Col(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);
	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Canyon_80A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_80A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_80A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_81A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_81A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_81A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_93A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_93A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_93A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_95A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_95A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_95A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_96A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_96A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_96A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_97A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_97A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_97A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_98A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_98A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_98A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_122A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_122A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_122A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_123A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_123A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_123A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_125A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_125A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_125A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Canyon_126A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Canyon_126A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Environment/Canyon/Canyon_126A_COL.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Bridge(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Bridge_4A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_4A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Bridge/Bridge_4A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_4B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_4B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Bridge/Bridge_4B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_4D */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_4D");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Bridge/Bridge_4D.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_4H */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_4H");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Bridge/Bridge_4H.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_4L */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_4L");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Bridge/Bridge_4L.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_4M */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_4M");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Bridge/Bridge_4M.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_4N */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_4N");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Bridge/Bridge_4N.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_5 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_5");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Bridge/Bridge_5.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_6 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_6");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Bridge/Bridge_6.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_8 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_8");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Bridge/Bridge_8.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_14A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_14A");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Bridge/Bridge_14A.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_14B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_14B");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Bridge/Bridge_14B.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_For_Desert_Bridge_Col(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::LEVEL_PROB);

	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Bridge_14A */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_14A_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Bridge/Bridge_14A_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Bridge_14B */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bridge_14B_COL");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Map_Editor/Bin/Resources/Maps/Desert/Bridge/Bridge_14B_COL.fbx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}


HRESULT CLoader::Loading_UI_For_Logo_Level()
{
	m_strMessage = TEXT("UI 로딩중 입니다..");
	// 텍스쳐
	/* For.Prototype_Component_UI_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/BackGround/BackGround_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Overlay */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_Overlay"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/BackGround/Loading_BG_0.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Logo_Main_Title */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_Logo_Main_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Logo/Main_Title.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Dot_Select */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_Dot_Select"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Dots/Dot_Select.png"), 1))))
		return E_FAIL;

	// 객체 원형
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_UI_Panel"),
		CUIPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_UI_Wrapper"),
		CUIWrapper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_UI_Button"),
		CUIButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_UI_Text"),
		CUIText::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_UI_Image"),
		CUIImage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_UI_For_GamePlay_Level(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	/* For.Prototype_Component_UI_Texture_Overlay */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Overlay");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/BackGround/Loading_BG_0.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Center_Pivot */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Center_Pivot");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Aim/Center_Pivot.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Number */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Number");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Number/Number_%d.png"), 10);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Icon_Gold */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Icon_Gold");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Info/Icon_Gold.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Shadow_Block */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Shadow_Block");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/ETC/Shadow_Block.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Panel */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Panel");
	pProtoDesc.pPrototype = CUIPanel::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Wrapper */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Wrapper");
	pProtoDesc.pPrototype = CUIWrapper::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Button */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Button");
	pProtoDesc.pPrototype = CUIButton::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Text */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Text");
	pProtoDesc.pPrototype = CUIText::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Image */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Image");
	pProtoDesc.pPrototype = CUIImage::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// 보스 체력바

	/* For.Prototype_Component_UI_Texture_Boss_Vital_Shadow */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Boss_Vital_Shadow");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Boss/Boss_Vital_Shadow.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Boss_Name */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Boss_Name");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Boss/Boss_Name_%d.png"), 2);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Boss_Stamina */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Boss_Stamina");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Boss/Stamina_%d.png"), 3);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Boss_StaminaFX */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Boss_StaminaFX");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Boss/Stamina_FX_%d.png"), 2);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_BossVitalWrapper */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_BossVitalWrapper");
	pProtoDesc.pPrototype = CUIBossVitalWrapper::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Boss_HPBar */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Boss_HPBar");
	pProtoDesc.pPrototype = CUIBossHPBar::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Boss_HPFX */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Boss_HPFX");
	pProtoDesc.pPrototype = CUIBossHPBarFX::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Boss_Shield */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Boss_Shield");
	pProtoDesc.pPrototype = CUIBossShield::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_BossName */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_BossName");
	pProtoDesc.pPrototype = CUIBossName::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_BossStamina */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_BossStamina");
	pProtoDesc.pPrototype = CUIBossStamina::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_BossStamina */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_BossStaminaFX");
	pProtoDesc.pPrototype = CUIBossStaminaFX::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_GetterQueue */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_GetterQueue");
	pProtoDesc.pPrototype = CUIGetterQueue::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_OwnGold */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_OwnGold");
	pProtoDesc.pPrototype = CUIOwnGold::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	// 대화 스크립트
	/* For.Prototype_GameObject_UI_Script */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Script");
	pProtoDesc.pPrototype = CUIScript::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_UI_For_Combat_HUD_Vitals(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	/* For.Prototype_Component_UI_Texture_Vital */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Vital");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Vital/Vital_Tag_%d.png"), 3);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Player_Hp */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Player_Hp");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Player_HP/Player_HP_%d.png"), 2);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Player_Hp_FX */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Player_Hp_FX");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Player_HP/Player_HP_FX.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Player_BetaFX */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Player_BetaFX");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Player_HP/BetaFx_%d.png"), 2);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Potion */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Potion");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Potion/Potion.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Potion_Stack */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Potion_Stack");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Potion/Potion_Stack.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Btn_Empty */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Btn_Empty");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/ETC/Btn_Empty.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_HPBar */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_HPBar");
	pProtoDesc.pPrototype = CUIHPBar::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_HP_FX */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_HP_FX");
	pProtoDesc.pPrototype = CUIHPFX::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Potion */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Potion");
	pProtoDesc.pPrototype = CUIPotion::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_PotionCount */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_PotionCount");
	pProtoDesc.pPrototype = CUIPotionCount::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_PotionStack */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_PotionStack");
	pProtoDesc.pPrototype = CUIPotionStack::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Shield */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Shield");
	pProtoDesc.pPrototype = CUIShield::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Beta */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Beta");
	pProtoDesc.pPrototype = CUIBeta::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_BetaFX */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_BetaFX");
	pProtoDesc.pPrototype = CUIBetaFX::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_UI_For_Combat_HUD_Skills(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	/* For.Prototype_Component_UI_Texture_SkillFrame */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_SkillFrame");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/SkillFrame.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Beta_Cost_Deco */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Beta_Cost_Deco");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Beta_Cost_Deco.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_SkillFrame_Shadow */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_SkillFrame_Shadow");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/SkillFrame_Shadow_%d.png"), 2);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Skill_Focus_Glow */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Skill_Focus_Glow");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Ring_Focus_OutGlow.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Skill_On_Fx */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Skill_On_Fx");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Skill_On_Fx_0.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_SkillWrapper_On_Fx */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_SkillWrapper_On_Fx");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/SkillWrapper_On_Fx_%d.png"), 2);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Skill_Line_Fx */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Skill_Line_Fx");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/On_Line_%d.png"), 2);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Skills */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Skills");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Skills/Skill_%d.png"), 4);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Rush */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Rush");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Rush_Frame */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Rush_Frame");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush_Frame.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Rush_CoolTime */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Rush_CoolTime");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush_CoolTime.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Rush_Glow */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Rush_Glow");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush_Glow_%d.png"), 2);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Rush_Frame_Glow */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Rush_Frame_Glow");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush_Frame_Glow.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_On_Ring */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_On_Ring");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/On_Ring.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Skill_Slot_Covor */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Skill_Slot_Covor");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/SkillCover.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_On_BetaText */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_On_BetaText");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/On_BetaText.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_SkillWrapper */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_SkillWrapper");
	pProtoDesc.pPrototype = CUISkillWrapper::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_SkillWrapperLineFX */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_SkillWrapperLineFX");
	pProtoDesc.pPrototype = CUISkillWrapperLineFX::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_SkillWrapperOnFX */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_SkillWrapperOnFX");
	pProtoDesc.pPrototype = CUISkillWrapperOnFX::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_SkillSlot */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_SkillSlot");
	pProtoDesc.pPrototype = CUISkillSlot::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_RushSlot */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_RushSlot");
	pProtoDesc.pPrototype = CUIRushSlot::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_UI_For_World(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	/* For.Prototype_Component_UI_Texture_LockOn */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_LockOn");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/LockOn/LockOnMark.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_KeyIcon_Mouse */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_KeyIcon_Mouse");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/LockOn/KeyIcon_Mouse_0.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Shadow_Ball */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Shadow_Ball");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/LockOn/Shadow_Ball.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Finish_Ring */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Finish_Ring");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/LockOn/Finish_Ring.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Interaction_Lock */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Interaction_Lock");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Interaction/Interaction_Lock.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Interaction_Key */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Interaction_Key");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Interaction/KeyIcon_%d.png"), 5);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Interaction_Hold_Gauge */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Interaction_Hold_Gauge");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Interaction/Hold_Gauge.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Interaction_Shadow */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Interaction_Shadow");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Interaction/Interaction_Shadow.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Interaction_FX */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Interaction_FX");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Interaction/Interaction_FX_%d.png"), 3);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_WorldWrapper */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_WorldWrapper");
	pProtoDesc.pPrototype = CUIWorldWrapper::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Monster_HPBar */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Monster_HPBar");
	pProtoDesc.pPrototype = CUIMonsterHPBar::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Monster_HPFX */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Monster_HPFX");
	pProtoDesc.pPrototype = CUIMonsterHPFX::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Monster_Shield */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Monster_Shield");
	pProtoDesc.pPrototype = CUIMonsterShield::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	
	/* For.Prototype_GameObject_UI_Monster_Stamina */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Monster_Stamina");
	pProtoDesc.pPrototype = CUIMonsterStamina::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Monster_StaminaFX */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Monster_StaminaFX");
	pProtoDesc.pPrototype = CUIMonsterStaminaFX::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_SimpleKey */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_SimpleKey");
	pProtoDesc.pPrototype = CUISimpleKey::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_InteractionFX */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_InteractionFX");
	pProtoDesc.pPrototype = CUIInteractionFX::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
	
	/* For.Prototype_GameObject_UI_LockOn */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_LockOn");
	pProtoDesc.pPrototype = CUILockOn::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	return S_OK;
}

HRESULT CLoader::Loading_UI_For_Popup(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	/* For.Prototype_Component_UI_Texture_PopupBG */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_PopupBG");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Popup/Popup_Bg.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Popup_Inner_Frame */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Popup_Inner_Frame");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Inner_Frame_%d.png"), 2);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Costume_Puzzle_Button */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Costume_Puzzle_Button");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Puzzle_Button_%d.png"), 4);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Suit_Icon */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Suit_Icon");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Suit_%d.png"), 12);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Suit_Icons */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Suit_Icons");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Suit_Icons.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_UI_Texture_Puzzle_Answer */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_UI_Texture_Puzzle_Answer");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Answer_%d.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Popup */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Popup");
	pProtoDesc.pPrototype = CUIPopup::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Costume_Puzzle_Answer */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Costume_Puzzle_Answer");
	pProtoDesc.pPrototype = CUICostumePuzzleAnswer::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_UI_Costume_Puzzle_Buttons */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_UI_Costume_Puzzle_Buttons");
	pProtoDesc.pPrototype = CUICostumePuzzleButtons::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

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

	Safe_Release(m_pGameManager);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
