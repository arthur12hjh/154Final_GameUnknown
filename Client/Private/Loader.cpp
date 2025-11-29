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
#include "AttackHitBox.h"

#include "Effect.h"
#include "Trail.h"
#include "TrailEffect.h"

#include "GameInstance.h"

#pragma region PLAYER
#include "PlayerFSM.h"
#include "Body_Player.h"
#include "Face_Player.h"
#include "Hair_Player.h"
#include "PonyTail_Player.h"
#pragma endregion

#pragma region Monster

#include "MonsterFSM.h"
#include "Nayitba.h"
#include "NayitbaPartBody.h"

#include "MonsterController.h"
#include "MonsterMimesisController.h"

#pragma region BOSS
#include "Gorilla.h"
#include "Gorilla_Body.h"
#pragma endregion

#pragma endregion

#pragma region Client Component
#include "Interaction_Component.h"
#include "TargetComponent.h"
#include "AISenceComponent.h"
#pragma endregion

#pragma region Prob
#include "Prob_Box.h"

#pragma region Interaction
#include "Vending.h"
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

#include "Instance_Model.h"
#include "PxTestProp.h"

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

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY LoadingMain(void* pArg)
{
	CLoader*		pLoader = static_cast<CLoader*>(pArg);

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
		m_strMessage = TEXT("메시 로딩중.");
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Mesh(pArg); });

		m_strMessage = TEXT("셰이더 로딩중.");
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Shader(pArg); });

		m_strMessage = TEXT("이펙트 로딩중.");
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Effect(pArg); });


		m_strMessage = TEXT("맵 로딩중.");
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Map(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Map_Scarlet_Building(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Map_Scarlet_Environment(pArg); });

		m_strMessage = TEXT("인스턴싱중.");
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_InstanceMesh(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Components(pArg); });

		m_strMessage = TEXT("플레이어가 재훈이형 잡으러 가는중.");
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { Loading_For_GamePlay_Player(pArg); });

		hr = Loading_For_GamePlay();
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
	m_strMessage = TEXT("완료..");
	
	m_isFinished = true;

	Sleep(1000);
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	string szFrontPath = "../Bin/Resources/Models/Character/PC/Eve/Animation/";
	_wstring szPlayerTag = TEXT("Prototype_Component_Model_Eve_Body_24_TypeB");
	_matrix PreMatrix = XMMatrixScaling(0.03, 0.03, 0.03) * XMMatrixRotationY(XMConvertToRadians(270.f));

	//                        "../Bin/Resources/Models/Character/Eve_body_psk7th/CH_P_EVE_09_nosimplify.bin", 
	//                        "../Bin/Resources/Models/Character/PC/Eve/CH_P_Eve_CombinedAnimationTest.bin", 

	vector<_wstring> szPartPrototypeTagList;
	vector<string> szPartModelFilePathList;

	szPartPrototypeTagList.push_back(TEXT("Prototype_Component_Model_Face_Eve"));
	szPartPrototypeTagList.push_back(TEXT("Prototype_Component_Model_Hair_Eve"));
	szPartPrototypeTagList.push_back(TEXT("Prototype_Component_Model_PonyTail_Eve"));
	

	szPartModelFilePathList.push_back("../Bin/Resources/Models/Character/PC/Eve/CH_P_HEAD_EVE/Eve_Head_v01.binx");
	szPartModelFilePathList.push_back("../Bin/Resources/Models/Character/PC/Eve/CH_HR_EVE/Eve_Hair.binx");
	szPartModelFilePathList.push_back("../Bin/Resources/Models/Character/PC/Eve/CH_HR_EVE/Eve_PonyTail.binx");

	if (FAILED(m_pGameInstance->Add_SkeletalPrototype(ENUM_CLASS(LEVEL::GAMEPLAY), m_pDevice, m_pContext,
		szPlayerTag, "../Bin/Resources/Models/Character/PC/Eve/CH_P_EVE_Model/Eve_Body_24_TypeB.binx",
		szFrontPath, szPartPrototypeTagList, szPartModelFilePathList, PreMatrix)))
		return E_FAIL;

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

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.bin", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky1 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Sky1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Sky/T_Sky_RockyHills.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Sky */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Sky"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Sky/Sky1.bin", PreTransformMatrix))))
		return E_FAIL;

	

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

	/* For.Prototype_GameObject_PonyTail_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_PonyTail_Player"),
		CPonyTail_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster"),
		CGorilla::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(Loading_UI_For_GamePlay_Level()))
		return E_FAIL;

	while (m_pGameInstance->IsWorkThread());
	m_strMessage = TEXT("완료 되었습니다..");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Player(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Shader_Instance_Model");
	pProtoDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/VTX_InstnaceMesh.hlsl"), VTX_NONEANIM_INSTANCE_DESC::Elements, VTX_NONEANIM_INSTANCE_DESC::iNumElements);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_Component_Shader_VtxNorTex */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxNorTex");
	pProtoDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Shader_VtxMesh */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxMesh");
	pProtoDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxAnimMesh");
	pProtoDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Shader_VtxCube */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxCube");
	pProtoDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Shader_VtxRectParticle */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxRectParticle");
	pProtoDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectParticle.hlsl"), VTX_POSTEX_INSTANCE_PARTICLE::Elements, VTX_POSTEX_INSTANCE_PARTICLE::iNumElements);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Shader_VtxPointParticle */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxPointParticle");
	pProtoDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointParticle.hlsl"), VTX_POS_INSTANCE_PARTICLE::Elements, VTX_POS_INSTANCE_PARTICLE::iNumElements);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Collider_AABB */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Collider_AABB");
	pProtoDesc.pPrototype = CBoxCollider::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	//Desc->OnCompleted(this_thread::get_id());

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

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Terrain_ORM");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Scarlet/Terrain/005_A_TAoRM.dds"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Texture_Terrain_Mask */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Terrain_Mask");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Texture_Reed_Mask */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Reed_Mask");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Scarlet/Terrain/ReedMask2.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Texture_Snow */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Snow");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Texture_Sky */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Sky");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Texture_Explosion*/
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Explosion");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Texture_Explosion_Test*/
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Explosion_Test");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Test.png"), 1);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Weapon */
	_matrix PreTransformMatrix = XMMatrixScaling(0.0003f, 0.0003f, 0.0003f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Weapon");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Character/PC/Eve/CH_W_Sword/CH_W_Sword.binx", PreTransformMatrix);
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

	/* For.Prototype_Component_Model_Prob_CanBox */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Prob_CanBox");
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

	/* For.Prototype_Component_Model_CM_Rock1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_Component_Model_BanacleA */
	PreTransformMatrix = XMMatrixScaling(0.028f, 0.028f, 0.028f) * XMMatrixRotationY(XMConvertToRadians(-90.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_BanacleA");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Monster/Banacle/CH_M_NA_08.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock2.binx", PreTransformMatrix);

	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_StatueA */
	PreTransformMatrix = XMMatrixScaling(0.028f, 0.028f, 0.028f) * XMMatrixRotationY(XMConvertToRadians(-90.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StatueA");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Monster/Statue/A/CH_M_NA_40.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock3 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock3");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock3.binx", PreTransformMatrix);
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
	
	/* For.Prototype_Component_Model_CM_Rock4 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock4");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock4.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock6 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock6");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock6.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock7 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock7");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock7.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock8 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock8");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock8.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock9 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock9");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock9.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock10 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock10");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock10.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock11 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock11");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock11.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock12 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock12");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock12.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock13 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock13");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock13.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CM_Rock14 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CM_Rock14");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Common/Rock/CM_Rock14.binx", PreTransformMatrix);

	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	///* For.Prototype_Component_Model_Prob_Vending */
	//pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Prob_Vending");
	//pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Prob/Vending/Vending.fbx", PreTransformMatrix);
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

	/* SKY BOX*/
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Sky");
	pProtoDesc.pPrototype = CSky::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* Gorilla Body */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Gorilla_Body");
	pProtoDesc.pPrototype = CGorilla_Body::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Snow */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Snow");
	pProtoDesc.pPrototype = CSnow::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Explosion */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Explosion");
	pProtoDesc.pPrototype = CExplosion::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Sprite_Explosion */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Sprite_Explosion");
	pProtoDesc.pPrototype = CSpriteEffect::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_Test_InstanceModel */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Test_InstanceModel");
	pProtoDesc.pPrototype = CInstance_Model::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_GameObject_ForkLift */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_ForkLift");
	pProtoDesc.pPrototype = CForkLift::Create(m_pDevice, m_pContext);
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

	/* For.Prototype_Component_Shader_VtxMeshEffect */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxMeshEffect");
	PrototypeDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshEffect.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements);
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Shader_VtxSpriteUVEffect */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxSpriteUVEffect");
	PrototypeDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxSpriteEffect.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Shader_VtxSpriteParticle */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxSpriteParticle");
	PrototypeDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxSpriteParticle.hlsl"), VTX_POS_INSTANCE_PARTICLE::Elements, VTX_POS_INSTANCE_PARTICLE::iNumElements);
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Shader_VtxTrail */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Shader_VtxTrail");
	PrototypeDesc.pPrototype = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTrailEffect.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Slash */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Slash");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Slash.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Hit_Spark */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_SheildBreak_Yellow");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/SheildBreakYellow.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Hit_Spark */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Hit_Spark");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/Spark.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_Effect_Spectrum_Test */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_Effect_Spectrum_Test");
	PrototypeDesc.pPrototype = CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/SpectrumTest.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	/* For.Prototype_Component_TrailEffect_Test */
	PrototypeDesc.szPrototypeName = TEXT("Prototype_Component_TrailEffect_Test");
	PrototypeDesc.pPrototype = CTrailEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/TrailEffect/SaveEffect.binx");
	Desc->pAddObejct.push_back(PrototypeDesc);

	//Desc->OnCompleted(this_thread::get_id());

	return S_OK;
}


HRESULT CLoader::Loading_For_GamePlay_Map(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Navigation");
	pProtoDesc.pPrototype = CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation2.bin"));
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* Prob. Box */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Prob_Box");
	pProtoDesc.pPrototype = CProb_Box::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* Prob. Vending */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Prob_Vending");
	pProtoDesc.pPrototype = CVending::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/*CanBox*/
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Prob_CanBox");
	pProtoDesc.pPrototype = CCanBox::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/*InstanceModel*/
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_InstanceModel");
	pProtoDesc.pPrototype = CInstanceModel::Create(m_pDevice, m_pContext);
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


	//Desc->OnCompleted(this_thread::get_id());
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Map_Scarlet_Environment(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	/* For.Prototype_Component_Model_Stone1 */
	_matrix PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stone1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/Stone1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stone2 */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stone2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/Stone2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stone3 */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stone3");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/Stone3.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stone4 */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stone4");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/Stone4.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_StoneWall1 */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StoneWall1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/StoneWall1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_StoneWall2 */
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StoneWall2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stone/StoneWall2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Rock1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Rock1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Rock/Rock1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Rock2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Rock2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Rock/Rock2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Rock3 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Rock3");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Rock/Rock3.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Rock4 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Rock4");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Rock/Rock4.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Rock6 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Rock6");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Rock/Rock6.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Rock7 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Rock7");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Rock/Rock7.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Rock8 */
	PreTransformMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Rock8");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Rock/Rock8.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_StoneTile */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StoneTile");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Tile/StoneTile.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CherryBlossom1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CherryBlossom1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Tree/CherryBlossom1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CherryBlossom2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CherryBlossom2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Tree/CherryBlossom2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CherryBlossom3 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CherryBlossom3");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Tree/CherryBlossom3.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_CherryBlossom4 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_CherryBlossom4");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Tree/CherryBlossom4.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Grass */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Grass");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Grass/Grass.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_DryGrass1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_DryGrass1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Grass/DryGrass1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_DryGrass2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_DryGrass2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Grass/DryGrass2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_DryGrass3 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_DryGrass3");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Grass/DryGrass3.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Moon */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Moon");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Sky/Moon2.binx", PreTransformMatrix);
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

	/* For.Prototype_GameObject_Moon */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Moon");
	pProtoDesc.pPrototype = CMoon::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	//Desc->OnCompleted(this_thread::get_id());

#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Map_Scarlet_Building(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	/* For.Prototype_Component_Model_TombStone */
	_matrix PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_TombStone");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/TombStone/tombstone.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Inscription_L */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Inscription_L");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/TombStone/TombStone_L.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Inscription_R */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Inscription_R");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/TombStone/TombStone_R.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_TombStoneBase1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_TombStoneBase1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/TombStone/TombStoneBase1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_TombStoneBase2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_TombStoneBase2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/TombStone/TombStoneBase2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Stair */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Stair");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Stair/Stair.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Giwajip */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Giwajip");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Giwajip/Giwajip.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Giwajip2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Giwajip2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/Giwajip/Giwajip2.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_StoneLantern1 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StoneLantern1");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/StoneLantern/StoneLantern1.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_StoneLantern2 */
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_StoneLantern2");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Maps/Scarlet/StoneLantern/StoneLantern2.binx", PreTransformMatrix);
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

	/* For.Prototype_GameObject_Attack_Hit_Box */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_AttackHitBox");
	pProtoDesc.pPrototype = CAttackHitBox::Create(m_pDevice, m_pContext);
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



	/* For.Prototype_Component_VIBuffer_Terrain */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_VIBuffer_Terrain");
	pProtoDesc.pPrototype = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Maps/Scarlet/Terrain/Height2.bmp"));
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

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

	/* For.Prototype_Component_Model_Eve */
	PreTransformMatrix = XMMatrixScaling(0.0003f, 0.0003f, 0.0003f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Eve");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/CH_P_EVE_09_body_idleTest.bin", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	//PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	CVIBuffer_Instance_Model::MODEL_INSTANCE_DESC ModelDesc{};
	ModelDesc.iNumInstance = 100;
	ModelDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	ModelDesc.vRange = _float3(50.f, 5.f, 50.f);
	ModelDesc.pModelFilePath = "../Bin/Resources/Models/Dororong/CH_NPC_Dororong.bin";
	ModelDesc.PreModelMatrix = PreTransformMatrix;

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Instance_Model_Dororong");
	pProtoDesc.pPrototype = CVIBuffer_Instance_Model::Create(m_pDevice, m_pContext, &ModelDesc);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_Component_Model_Reed */
	PreTransformMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	CVIBuffer_Instance_Model::MODEL_INSTANCE_DESC ReedDesc{};
	ReedDesc.iNumInstance = 10000;
	ReedDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	ReedDesc.vRange = _float3(0.f, 0.f, 0.f);
	ReedDesc.pModelFilePath = "../Bin/Resources/Maps/Scarlet/Reed/Reed5.binx";
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
	BambooDesc.pModelFilePath = "../Bin/Resources/Maps/Scarlet/Tree/Tree1.binx";
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
	Rock5Desc.pModelFilePath = "../Bin/Resources/Maps/Scarlet/Rock/Rock5.binx";
	Rock5Desc.PreModelMatrix = PreTransformMatrix;

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Rock5");
	pProtoDesc.pPrototype = CVIBuffer_Instance_Model::Create(m_pDevice, m_pContext, &Rock5Desc);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Gorilla");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Gorilla/Gorilla.bin", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	//Desc->OnCompleted(this_thread::get_id());
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

	//Desc->OnCompleted(this_thread::get_id());

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

	/* For.Prototype_Component_UI_Texture_Default */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Default0.png"), 1))))
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

HRESULT CLoader::Loading_UI_For_GamePlay_Level()
{
	m_strMessage = TEXT("UI 로딩중 입니다..");

	if (FAILED(Loading_UI_For_Combat_HUD_Vitals()))
		return E_FAIL;

	if (FAILED(Loading_UI_For_Combat_HUD_Skills()))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Center_Pivot */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Center_Pivot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Aim/Center_Pivot.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Number */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Number"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Number/Number_%d.png"), 10))))
		return E_FAIL;

	// 객체 원형
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Panel"),
		CUIPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Wrapper"),
		CUIWrapper::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Button"),
		CUIButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Text"),
		CUIText::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Image"),
		CUIImage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_UI_For_Combat_HUD_Vitals()
{
	/* For.Prototype_Component_UI_Texture_Vital */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Vital"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Vital/Vital_Tag_%d.png"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Player_Hp */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_Hp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Player_HP/Player_HP_%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Player_Hp_FX */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_Hp_FX"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Player_HP/Player_HP_FX.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Player_Beta */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_Beta"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Player_HP/Player_Beta_%d.png"), 6))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Potion */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Potion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Potion/Potion.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Potion_Stack */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Potion_Stack"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Potion/Potion_Stack.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Btn_Empty */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Btn_Empty"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/ETC/Btn_Empty.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_HPBar"),
		CUIHPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_HP_FX"),
		CUIHPFX::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Potion"),
		CUIPotion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_PotionCount"),
		CUIPotionCount::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_PotionStack"),
		CUIPotionStack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Shield"),
		CUIShield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Beta"),
		CUIBeta::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_UI_For_Combat_HUD_Skills()
{
	/* For.Prototype_Component_UI_Texture_SkillFrame */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_SkillFrame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/SkillFrame.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_SkillFrame_Shadow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_SkillFrame_Shadow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/SkillFrame_Shadow_%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Skill_Focus_Glow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skill_Focus_Glow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Ring_Focus_OutGlow.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Skill_On_Fx */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skill_On_Fx"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Skill_On_Fx_%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Skills */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skills"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Skills/Skill_%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Rush */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Rush_Glow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush_Glow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush_Glow.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Rush_Frame_Glow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush_Frame_Glow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush_Frame_Glow.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_On_Ring */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_On_Ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/On_Ring.png"), 1))))
		return E_FAIL;

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
