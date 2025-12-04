#include "pch.h"
#include "Loader.h"

#include "BackGround.h"

#include "Character.h"
#include "Dororong.h"
#include "Gigas.h"
#include "Extra.h"
#include "Weapon.h"
#include "Sky.h"
#include "ModelTerrain.h"

#include "GameInstance.h"
#include "StringHelper.h"
#include "Grid.h"

#include "Body_Character.h"
#include "Face_Character.h"
#include "Hair_Character.h"
#include "PonyTail_Character.h"
#include "Body_Dororong.h"
#include "Body_Gigas.h"
#include "Body_Extra.h"


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

	/* ��������, ���ؽ�, ũ��Ƽ�ü��� */

	/* �Ӱ迵��(��, ������, �ڵ�)�� �����ϱ����� Ű�� �����Ѵ�. */
	InitializeCriticalSection(&m_CriticalSection);

	/* ���� �ε��� �����ϱ����� �����带 �����Ѵ�. */
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
		m_pGameInstance->Add_ThreadjobList([&](void* pArg) { this->Loading_For_Player(pArg); });

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
	m_strMessage = TEXT("�ؽ��ĸ�(��) �ε� �� �Դϴ�.");

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	m_strMessage = TEXT("�𵨸�(��) �ε� �� �Դϴ�.");

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix			PreTransformMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Grid */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Grid"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/GridModel/Plane_Grid.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Dororong */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Dororong"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Dororong/CH_NPC_Dororong.bin", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Gigas */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Gigas"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Monster/Gigas/SuperGorilla.binx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_StatueA */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_StatueA"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/Models/Character/Monster/Statue/A/CH_M_NA_40.binx", PreTransformMatrix))))
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



	m_strMessage = TEXT("���̴���(��) �ε� �� �Դϴ�.");
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

	m_strMessage = TEXT("�ݶ��̴���(��) �ε� �� �Դϴ�.");
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

	m_strMessage = TEXT("��ü������(��) �ε� �� �Դϴ�.");
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

	/* For.Prototype_GameObject_ModelTerrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_ModelTerrain"),
		CModelTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_Monster */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Monster"),
	//	CMonster::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	//
	///* For.Prototype_GameObject_Weapon */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Weapon"),
	//	CWeapon::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	while (m_pGameInstance->IsWorkThread());
	m_strMessage = TEXT("�ε��� �Ϸ�Ǿ����ϴ�..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Player(void* pArg)
{
	THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);

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
		szPlayerTag, "../Bin/Resources/Models/Character/PC/Eve/CH_P_EVE_Model/Eve_Body_24_TypeB.binx",
		szFrontPath, szPartPrototypeTagList, szPartModelFilePathList, PreMatrix)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Eve_CombinedAnimationTest"),
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
