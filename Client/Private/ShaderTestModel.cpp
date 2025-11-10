#include "pch.h"
#include "ShaderTestModel.h"

#include "GameInstance.h"
#include "PlayerCCTHitReporter.h"

CShaderTestModel::CShaderTestModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CShaderTestModel::CShaderTestModel(const CShaderTestModel& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CShaderTestModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShaderTestModel::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC	Desc{};
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fSpeedPerSec = 10.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(0, true);

	//m_pGameInstance->Add_RigidBody_ToPhysx(this, m_pRigidBody);

	return S_OK;
}

void CShaderTestModel::Priority_Update(_float fTimeDelta)
{
}

void CShaderTestModel::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_NUMPAD8))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
		m_pModelCom->Set_AnimationIndex(1, true);
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_NUMPAD6))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		m_pModelCom->Set_AnimationIndex(1, true);
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_NUMPAD4))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
		m_pModelCom->Set_AnimationIndex(1, true);
	}

 	m_pCCT->Update_PxPosition(fTimeDelta, m_pTransformCom);
}

void CShaderTestModel::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CShaderTestModel::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CShaderTestModel::Render_Shadow()
{
	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CShaderTestModel::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_ShaderTestModel"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	
	/* Com_CCT */
	CCharacterController::CCT_DESC Desc;

	Desc.eCharacterControllerType = CCharacterController::CCT_SHAPE::CAPSULE;
	Desc.szControllerTag = TEXT("Player_Controller");
	//캡슐 컨트롤러에서 x는 구 성분 y는 기둥 성분
	Desc.vSize = _float3(1.f, 1.f, 0.f);
	XMStoreFloat4(&Desc.vStartPos, m_pTransformCom->Get_State(STATE::POSITION));
	Desc.vMaterial = _float3(0.5f, 0.5f, 0.f);
	Desc.pHitReporter = CPlayerCCTHitReporter::Create();

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterController"),
		TEXT("Com_CCT"), reinterpret_cast<CComponent**>(&m_pCCT), &Desc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CShaderTestModel::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CShaderTestModel* CShaderTestModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShaderTestModel* pInstance = new CShaderTestModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CShaderTestModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShaderTestModel::Clone(void* pArg)
{
	CShaderTestModel* pInstance = new CShaderTestModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CShaderTestModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShaderTestModel::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pCCT);
}
