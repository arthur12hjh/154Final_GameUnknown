#include "pch.h"
#include "Loader.h"

#include "BackGround.h"

#include "Character.h"
#include "Scarlet.h"
#include "Dororong.h"
#include "Gigas.h"
#include "Extra.h"
#include "Weapon.h"
#include "Weapon_Scarlet.h"
#include "Scabbard_Scarlet.h"
#include "Bottle_Scarlet.h"
#include "Glass_Scarlet.h"
#include "Sky.h"
#include "ModelTerrain.h"

#include "GameInstance.h"
#include "StringHelper.h"
#include "Grid.h"

#include "Body_Scarlet.h"
#include "Face_Scarlet.h"
#include "Body_Character.h"
#include "Face_Character.h"
#include "Hair_Character.h"
#include "PonyTail_Character.h"
#include "HairPin_Character.h"
#include "Body_Dororong.h"
#include "Body_Gigas.h"
#include "Body_Extra.h"
#include "Facial_Extra.h"


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
	case LEVEL::EDITOR:
	{
		//m_pGameInstance->Add_ThreadjobList([&](void* pArg) { this->Loading_For_Player(pArg); });
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { this->Loading_For_Scarlet(pArg); });

		hr = Loading_For_Editor();
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

HRESULT CLoader::Loading_For_Editor()	
{
	m_strMessage = TEXT("누가 내 Loader에 똥쌌어!");

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	m_strMessage = TEXT("누가 내 Loader에 똥쌌어!");

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix			PreTransformMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Grid */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Grid"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/GridModel/Plane_Grid.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Dororong */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Dororong"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Dororong/CH_NPC_Dororong.bin", PreTransformMatrix))))
		return E_FAIL;
	
	/* For.Prototype_Component_Model_NPC05 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_NPC05"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/NPC/CH_NPC_05_B/CH_NPC_05_B.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Gigas */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Gigas"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Monster/Gigas/SuperGorilla_v01.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Beholder */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Beholder"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Monster/Beholder/CH_M_NA_51.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_StatueA */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_StatueA"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Monster/Statue/A/CH_M_NA_40.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_SunFlower */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_SunFlower"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Monster/SunFlower/SunFlower.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Minion11 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Minion11"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Monster/Minion11/Minion11.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_StatueB */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_StatueB"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Monster/Statue/B/CH_M_NA_40_B.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Banacle */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Banacle"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Monster/Banacle/CH_M_NA_08.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Tentacle */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Tentacle"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Monster/Tentacle/Tentacle.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DroidTurret */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_TurretDroid"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Monster/DroidTurret/TurretDroid.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Cinematic_Gorilla */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Cinematic_Gorilla"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Cinematic/Gorilla/Cine_Gorilla.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Scarlet_Face_Morph */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Scarlet_Face_Morph"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::FACIAL, "../Bin/Resources/Models/Character/Monster/Scarlet/CH_M_Scarlet_Face/CH_M_Scarlet_Face_Morph.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Scarlet_Weapon */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Scarlet_Weapon"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Character/Monster/Scarlet/CH_M_Scarlet_Weapon/CH_M_Scarlet_Weapon.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Scarlet_Scabbard */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Scarlet_Scabbard"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Character/Monster/Scarlet/CH_M_Scarlet_Weapon/CH_M_Scarlet_Scabbard.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Map_Village_Mou */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Map_Village_Mou"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Village_Mou/Village_Mou.bin", PreTransformMatrix))))
		return E_FAIL;

	///* For.Prototype_Component_Model_ForkLift */
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_ForkLift"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", PreTransformMatrix))))
	//	return E_FAIL;



	m_strMessage = TEXT("모름");
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Scarlet_Face */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_Scarlet_Face"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Scarlet_Face.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Eve_Face */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_Eve_Face"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Eve_Face.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_VtxRectParticle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectParticle.hlsl"), VTX_POSTEX_INSTANCE_PARTICLE::Elements, VTX_POSTEX_INSTANCE_PARTICLE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_VtxPointParticle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointParticle.hlsl"), VTX_POS_INSTANCE_PARTICLE::Elements, VTX_POS_INSTANCE_PARTICLE::iNumElements))))
		return E_FAIL;

	m_strMessage = TEXT("절대 콜라이더를 로드하고 있지 않습니다.");
	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Collider_AABB"),
		CBoxCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Collider_OBB"),
		COBBCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Collider_Sphere"),
		CSphereCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strMessage = TEXT("절대 객체원형을 로드하고 있지 않습니다.");
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_BackGround"),
	//	CBackGround::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	//
	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Grid */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Grid"),
		CGrid::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Scarlet */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Body_Scarlet"),
		CBody_Scarlet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Face_Scarlet */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Face_Scarlet"),
		CFace_Scarlet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Character */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Body_Character"),
		CBody_Character::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Face_Character */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Face_Character"),
		CFace_Character::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Hair_Character */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Hair_Character"),
		CHair_Character::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_PonyTail_Character */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_PonyTail_Character"),
		CPonyTail_Character::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Dororong */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Body_Dororong"),
		CBody_Dororong::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Gigas */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Body_Gigas"),
		CBody_Gigas::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Scarlet */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Scarlet"),
		CScarlet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Scarlet */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Weapon_Scarlet"),
		CWeapon_Scarlet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Scabbard_Scarlet */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Scabbard_Scarlet"),
		CScabbard_Scarlet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Bottle_Scarlet */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Bottle_Scarlet"),
		CBottle_Scarlet::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Glass_Scarlet */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Glass_Scarlet"),
		CGlass_Scarlet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Character */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Character"),
		CCharacter::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Dororong */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Dororong"),
		CDororong::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Gigas */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Gigas"),
		CGigas::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Extra*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Extra"),
		CExtra::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Extra */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Body_Extra"),
		CBody_Extra::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Facial_Extra */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Facial_Extra"),
		CFacial_Extra::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ModelTerrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_ModelTerrain"),
		CModelTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_Monster */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Monster"),
	//	CMonster::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	//
	/* For.Prototype_GameObject_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Weapon"),
		CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_HairPin */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_HairPin"),
		CHairPin_Character::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	while (m_pGameInstance->IsWorkThread());
	m_strMessage = TEXT("절대 로딩이 완료되지 않았습니다...");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Player(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::EDITOR);

	string szFrontPath = "../Bin/Resources/Models/Character/PC/Eve/Animation/";
	_wstring szPlayerTag = TEXT("Prototype_Component_Model_Eve_Body_24_TypeB");
	_matrix PreMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	//                        		szPlayerTag, "../Bin/Resources/Models/Character/Eve_body_psk7th/CH_P_EVE_09_nosimplify.bin",
	//                        		szPlayerTag, "../Bin/Resources/Models/Character/PC/Eve/CH_P_EVE_Default.bin",
	//                        		szPlayerTag, "../Bin/Resources/Models/Character/PC/Eve/CH_P_EVE_Nikke06.bin",
	//                        		szPlayerTag, "../Bin/Resources/Models/Character/PC/Eve/CH_P_EVE_63.bin",

	vector<_wstring> szPartPrototypeTagList;
	vector<string> szPartModelFilePathList;

	szPartPrototypeTagList.push_back(TEXT("Prototype_Component_Model_Face_Eve"));
	szPartPrototypeTagList.push_back(TEXT("Prototype_Component_Model_Hair_Eve"));
	szPartPrototypeTagList.push_back(TEXT("Prototype_Component_Model_PonyTail_Eve"));

	szPartModelFilePathList.push_back("../Bin/Resources/Models/Character/PC/Eve/CH_P_HEAD_EVE/Eve_Head_v01.binx");
	szPartModelFilePathList.push_back("../Bin/Resources/Models/Character/PC/Eve/CH_HR_EVE/Eve_Hair.binx");
	szPartModelFilePathList.push_back("../Bin/Resources/Models/Character/PC/Eve/CH_HR_EVE/Eve_PonyTail.binx");

	if (FAILED(m_pGameInstance->Add_SkeletalPrototype(ENUM_CLASS(LEVEL::EDITOR), m_pDevice, m_pContext,
		szPlayerTag, "../Bin/Resources/Models/Character/PC/Eve/CH_P_EVE_Model/Eve_Body_20.binx",
		szFrontPath, szPartPrototypeTagList, szPartModelFilePathList, PreMatrix)))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon */
	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Weapon");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/PC/Eve/CH_W_Sword/CH_W_Sword_ANI01.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);


	/* For.Prototype_Component_Model_HairPin */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_HairPin");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Character/PC/Eve/CH_W_Sword/CH_ACC_HairPin.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Eve_CombinedAnimationTest"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Eve_body_psk7th/CH_P_EVE_09_nosimplify.bin", PreMatrix))))
	//	return E_FAIL;

	//Desc->OnCompleted(this_thread::get_id());

	return S_OK;
}

HRESULT CLoader::Loading_For_Scarlet(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);

	string szFrontPath = "../Bin/Resources/Models/Character/Monster/Scarlet/Animation/";
	_wstring szPlayerTag = TEXT("Prototype_Component_Model_Scarlet_Body");
	_matrix PreMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	//                        		szPlayerTag, "../Bin/Resources/Models/Character/Eve_body_psk7th/CH_P_EVE_09_nosimplify.bin",
	//                        		szPlayerTag, "../Bin/Resources/Models/Character/PC/Eve/CH_P_EVE_Default.bin",
	//                        		szPlayerTag, "../Bin/Resources/Models/Character/PC/Eve/CH_P_EVE_Nikke06.bin",
	//                        		szPlayerTag, "../Bin/Resources/Models/Character/PC/Eve/CH_P_EVE_63.bin",

	vector<_wstring> szPartPrototypeTagList;
	vector<string> szPartModelFilePathList;

	szPartPrototypeTagList.push_back(TEXT("Prototype_Component_Model_Face_Scarlet"));
	//szPartPrototypeTagList.push_back(TEXT("Prototype_Component_Model_Hair_Eve"));
	//szPartPrototypeTagList.push_back(TEXT("Prototype_Component_Model_PonyTail_Eve"));
	//
	szPartModelFilePathList.push_back("../Bin/Resources/Models/Character/Monster/Scarlet/CH_M_Scarlet_Face/CH_M_Scarlet_Face.binx");
	//szPartModelFilePathList.push_back("../Bin/Resources/Models/Character/PC/Eve/CH_HR_EVE/Eve_Hair.binx");
	//szPartModelFilePathList.push_back("../Bin/Resources/Models/Character/PC/Eve/CH_HR_EVE/Eve_PonyTail.binx");

 	if (FAILED(m_pGameInstance->Add_SkeletalPrototype(ENUM_CLASS(LEVEL::EDITOR), m_pDevice, m_pContext,
		szPlayerTag, "../Bin/Resources/Models/Character/Monster/Scarlet/CH_M_Scarlet_Body/CH_M_Scarlet_Body_test04.binx",
		szFrontPath, szPartPrototypeTagList, szPartModelFilePathList, PreMatrix)))
		return E_FAIL;

	PROTOTYPE_DESC pProtoDesc = {};
	pProtoDesc.iLevelID = ENUM_CLASS(LEVEL::EDITOR);

	/* For.Prototype_Component_Model_Bottle_Scarlet */
	_matrix	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Bottle_Scarlet");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Character/Monster/Scarlet/CH_M_Scarlet_Bottle/CH_M_Scarlet_Bottle.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	/* For.Prototype_Component_Model_Glass_Scarlet */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	pProtoDesc.szPrototypeName = TEXT("Prototype_Component_Model_Glass_Scarlet");
	pProtoDesc.pPrototype = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Character/Monster/Scarlet/CH_M_Scarlet_Glass/CH_M_Scarlet_Glass.binx", PreTransformMatrix);
	if (nullptr == pProtoDesc.pPrototype)
		return E_FAIL;
	Desc->pAddObejct.push_back(pProtoDesc);

	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(L
	// EVEL::EDITOR), TEXT("Prototype_Component_Model_Eve_CombinedAnimationTest"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Eve_body_psk7th/CH_P_EVE_09_nosimplify.bin", PreMatrix))))
	//	return E_FAIL;

	//Desc->OnCompleted(this_thread::get_id());

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
