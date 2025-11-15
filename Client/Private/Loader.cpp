#include "pch.h"
#include "Loader.h"

#include "SpriteEffect.h"
#include "Camera_Free.h"
#include "BackGround.h"
#include "Explosion.h"
#include "ForkLift.h"

#include "Terrain.h"
#include "Weapon.h"
#include "Player.h"
#include "Snow.h"
#include "Sky.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Terrain_Sand.h"


#pragma region PLAYER
#include "Body_Player.h"
#include "Face_Player.h"
#include "Hair_Player.h"
#include "PonyTail_Player.h"
#pragma endregion

#pragma region Monster


#pragma region BOSS
#include "Gorilla.h"
#include "Gorilla_Body.h";
#pragma endregion

#pragma endregion


#include "Instance_Model.h"
#include "PxTestProp.h"

#include "Effect.h"

#ifdef _DEBUG
#include "ShaderTestModel.h"
#endif

#include "UIWrapper.h"
#include "UIPanel.h"
#include "UIButton.h"
#include "UIText.h"

#include "Vil_Bui03_04.h"

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

	m_pGameManager = CGameManager::GetInstance();

	if (!m_pGameManager)
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
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { this->Loading_For_GamePlay_Mesh(pArg); });

		m_strMessage = TEXT("셰이더 로딩중.");
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { this->Loading_For_GamePlay_Shader(pArg); });

		m_strMessage = TEXT("이펙트 로딩중.");
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { this->Loading_For_GamePlay_Effect(pArg); });

		m_strMessage = TEXT("네비게이션 로딩중.");
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { this->Loading_For_GamePlay_Navigation(pArg); });
		
		m_strMessage = TEXT("인스턴싱중.");
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { this->Loading_For_GamePlay_InstanceMesh(pArg); });

		m_strMessage = TEXT("플레이어가 재훈이형 잡으러 가는중.");
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { this->Loading_For_GamePlay_Player(pArg); });

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
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Loading_UI_For_Logo_Level();

	while (m_pGameInstance->IsWorkThread());
	m_strMessage = TEXT("완료..");
	
	m_isFinished = true;

	Sleep(1000);
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	string szFrontPath = "../Bin/Resources/Models/Character/PC/Eve/Animation/";
	_wstring szPlayerTag = TEXT("Prototype_Component_Model_Eve_CombinedAnimationTest");
	_matrix PreMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(270.f));

	//                        "../Bin/Resources/Models/Character/Eve_body_psk7th/CH_P_EVE_09_nosimplify.bin", 
	//                        "../Bin/Resources/Models/Character/PC/Eve/CH_P_Eve_CombinedAnimationTest.bin", 

	if (FAILED(m_pGameInstance->Add_SkeletalPrototype(ENUM_CLASS(LEVEL::GAMEPLAY), m_pDevice, m_pContext,
		szPlayerTag, "../Bin/Resources/Models/Character/CH_P_EVE_09_nosimplify.bin",
		szFrontPath, PreMatrix)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Eve_CombinedAnimationTest"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Eve_body_psk7th/CH_P_EVE_09_nosimplify.bin", PreMatrix))))
	//	return E_FAIL;

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.bin", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky1 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Sky1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/MapObj/Sky/T_skybox_06.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Face_Eve */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(270.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Face_Eve"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Eve_head_nonAnimTest/CH_p_EVE_Face_NodeTest.bin", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Hair_Eve */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(270.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Hair_Eve"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Eve_hair_Test/CH_P_EVE_Hair_Test.bin", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_PonyTail_Eve */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(270.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_PonyTail_Eve"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Eve_hair_PonyTailTest_2/CH_P_EVE_Hair_PonyTail_2_Test.bin", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Terrain_Sand */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Terrain_Sand"),
	CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/MapObj/Terrain/006_A_SM/006_A_SM.bin", PreTransformMatrix))))
	return E_FAIL;

	/* For.Prototype_Component_Model_Vil_Bui03_04 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Vil_Bui03_04"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/MapObj/Village/Object/Vil_Bui03_04.bin", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Sky */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Sky"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/MapObj/Sky/Sky1.bin", PreTransformMatrix))))
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

	/* For.Prototype_GameObject_Terrain_Sand */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain_Sand"),
		CTerrain_Sand::Create(m_pDevice, m_pContext))))
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

	Desc->OnCompleted(this_thread::get_id());

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Mesh(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	/* For.Prototype_Component_Texture_Terrain */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Terrain");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Texture_Terrain_Mask */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Texture_Terrain_Mask");
	pProtoDesc.pPrototype = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1);
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

	Desc->OnCompleted(this_thread::get_id());
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

	/* For.Prototype_GameObject_Vil_Bui03_04 */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_Vil_Bui03_04");
	pProtoDesc.pPrototype = CVil_Bui03_04::Create(m_pDevice, m_pContext);
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

#ifdef _DEBUG
	/* For.Prototype_GameObject_ShaderTestModel */
	pProtoDesc.szPrototypeName = TEXT("Prototype_GameObject_ShaderTestModel");
	pProtoDesc.pPrototype = CShaderTestModel::Create(m_pDevice, m_pContext);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);
#endif

	Desc->OnCompleted(this_thread::get_id());
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Effect(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);

	char pattern[MAX_PATH] = {};

	memset(pattern, 0, sizeof(pattern));
	strcpy_s(pattern, MAX_PATH, "../Bin/Resources/Models/EffectMesh/*.bin");

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
				if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), fileName,
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

				if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), sztProtoName,
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

				if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), sztProtoName,
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

				if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), sztProtoName,
					CTexture::Create(m_pDevice, m_pContext, szPath, 1))))
					return E_FAIL;
			}
		} while (FindNextFileA(h, &fd));
		FindClose(h);
	}

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMeshEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshEffect.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Test"),
		CEffect::Create(m_pDevice, m_pContext, "../Bin/Resources/Effect/bin.bin"))))
		return E_FAIL;
	Desc->OnCompleted(this_thread::get_id());

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Navigation(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Navigation");
	pProtoDesc.pPrototype = CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation2.bin"));
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;

	Desc->pAddObejct.push_back(pProtoDesc);

	Desc->OnCompleted(this_thread::get_id());
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_InstanceMesh(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::GAMEPLAY);

	/* For.Prototype_Component_VIBuffer_Terrain */
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_VIBuffer_Terrain");
	pProtoDesc.pPrototype = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp"));
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
	ExplosionDesc.iNumInstance = 5000;
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
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Eve");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/CH_P_EVE_09_body_idleTest.bin", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

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

	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Gorilla");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Gorilla/Gorilla.bin", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	Desc->OnCompleted(this_thread::get_id());
	return S_OK;
}

HRESULT CLoader::Loading_UI_For_Logo_Level()
{
	m_strMessage = TEXT("UI 로딩중 입니다..");
	/* For.Prototype_Component_UI_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/BackGround/BackGround_%d.png"), 1))))
		return E_FAIL;

	m_pGameManager->Add_UI_Texture(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_BackGround"),
		TEXT("Com_Texture_UI_BackGround"), TEXT("../../Client/Bin/Resources/Textures/UI/BackGround/BackGround_%d.png"), 1);

	/* For.Prototype_Component_UI_Texture_Default */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Default0.png"), 1))))
		return E_FAIL;

	m_pGameManager->Add_UI_Texture(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_Default"),
		TEXT("Com_Texture_UI_Default"), TEXT("../../Client/Bin/Resources/Textures/Default0.png"), 1);

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
