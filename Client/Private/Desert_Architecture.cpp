#include "pch.h"
#include "Desert_Architecture.h"
#include "GameInstance.h"

CDesert_Architecture::CDesert_Architecture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CDesert_Architecture::CDesert_Architecture(const CDesert_Architecture& Prototype)
	: CActor{ Prototype }
{
}

HRESULT CDesert_Architecture::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDesert_Architecture::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ACTOR_DESC* pDesc = static_cast<ACTOR_DESC*>(pArg);
	if (FAILED(Ready_Components(pDesc->szVIBuffer_PrototypeName)))
		return E_FAIL;

	//pair<wstring, wstring> result = Extract_ComponentTag(pDesc->szVIBuffer_PrototypeName);

	//Prototype_Component_Model_Bridge_4A

	/*m_ComponentCategory = result.first;
	m_ComponentNumber = result.second;

	if (m_ComponentCategory == L"Bridge")
	{
		if (m_ComponentNumber == L"14A" || m_ComponentNumber == L"14B")
		{
				return E_FAIL;
		}
	}*/
	Ready_Col(pDesc->szVIBuffer_PrototypeName);

	return S_OK;
}

void CDesert_Architecture::Priority_Update(_float fTimeDelta)
{
}

void CDesert_Architecture::Update(_float fTimeDelta)
{
	//SetCullingCollider(m_iObjectID);
	//_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	//m_pCullingCollider->UpdateColiision(worldMatrix);
}

void CDesert_Architecture::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
}

HRESULT CDesert_Architecture::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_ORMTexture", aiTextureType_METALNESS, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CDesert_Architecture::Ready_Components(const _tchar* pComponentTag)
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDesert_Architecture::Ready_Col(const _tchar* pComponentTag)
{
	/* Com_Model_COL */
	_wstring strComponentTag = pComponentTag;
	strComponentTag += TEXT("_COL");


	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), strComponentTag,
		TEXT("Com_Model_COL"), reinterpret_cast<CComponent**>(&m_pColModelCom))))
		return E_FAIL;

	// 충돌용 메시 피직스 세팅 조건
	// 1. 충돌용 메시 생성 이후에 RigidBody를 세팅해주셔야합니다.
	// 2. 해당 객체의 위치가 설정된 뒤에 RigidBody를 세팅해주셔야 합니다.

	// 세팅 방법 보고도 잘 이해 안되면 물어봐주세요 키네마틱, 다이나믹, 스태틱 세팅 중요해요
	PxUserData tUserData;
	// 엘레베이터 식별용 문자열. 이건 나중에 엘베말고 다른데에 넣을떄 저랑 얘기하고 정해서 넣어주세요
	tUserData.szActorTag = TEXT("Architecture_Actor");

	// 리지드 바디 Desc 세팅. 
	// F12 타고 들어가서 머테리얼이랑 Mass, userdata, shape, type 부분 위주로 살펴보세요.
	CRigidBody::RIGIDBODY_DESC RigidBodyDesc;
	// 콜라이더 모양.
	// ->> 메시는 TRIANGLE, 박스나 캡슐은 BOX, CAPSULE 다 따로 있으니까
	// F12 타고 들어가서 한번 확인해보세요.
	RigidBodyDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::TRIANGLE;

	// 충돌처리를 할지말지 
	// DYNAMIC : 충돌 
	// KINEMATIC : 충돌 O, 제한적으로 움직일 방법이 있음.
	// STATIC : 충돌 O, 대신 고정되어 있음. 아예 절대 못움직임 (지형같은거)
	// ->> 엘레베이터는 고정되어있으니까 STATIC으로 세팅 해주는거에요.

	RigidBodyDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::STATIC;

	RigidBodyDesc.StartWorldMatrix = *m_pTransformCom->Get_WorldMatrixPtr();
	RigidBodyDesc.tUserData = tUserData;
	RigidBodyDesc.vMaterial = _float3(0.5f, 0.5f, 0.3f);
	RigidBodyDesc.vSize = m_pTransformCom->Get_Scale();
	RigidBodyDesc.fMass = { 0.3f };
	// TRIANGLE로 세팅하고 충돌용 메시 작업하는거니까, 충돌용 메시 넣어줘야돼요 
	// TRIANGLE 타입이 아닌 (충돌용 메시가 아닌) 녀석들은 굳이 안넣어줘도 됩니다.
	RigidBodyDesc.pColModel = m_pColModelCom;

	/* Com_RigidBody */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
		return E_FAIL;

	// 리지드 바디 세팅 끝났으면 Physx 매니저에 집어넣는 과정도 있어야돼요.
	// 없으면 충돌 안됨
	m_pGameInstance->Add_RigidBody_ToPhysx(this, m_pRigidBody);

	return S_OK;
}

HRESULT CDesert_Architecture::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

void CDesert_Architecture::SetCullingCollider(_uint iObjectID)
{
	auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
	switch (iObjectID)
	{
	case 1:
		pCullingCollider->SetCollision({ 0.f, 1.2f, 0.f }, {}, { 4, 2, 7 });
		break;
	case 2:
		pCullingCollider->SetCollision({ 0.f, 3.f, 3.5f }, {}, { 4, 5, 7 });
		break;
	case 3:
		pCullingCollider->SetCollision({ 0.f, 1.8f, 1.5f }, {}, { 10, 3, 3 });
		break;
	case 4:
		pCullingCollider->SetCollision({ 0.f, 1.2f, 1.5f }, {}, { 2, 2, 3 });
		break;
	case 5:
		pCullingCollider->SetCollision({ 0.f, 108.f, 22.5f }, {}, { 60, 180, 45 });
		break;
	case 6:
		pCullingCollider->SetCollision({ 0.f, 0.6f, 1.f }, {}, { 4, 1, 2 });
		break;
	}

}

pair<wstring, wstring> CDesert_Architecture::Extract_ComponentTag(const _tchar* pComponentTag)
{
	wstring strComponentTag = pComponentTag;

	vector<wstring> tokens;
	wstring token;
	wstringstream tokenStream(strComponentTag);

	while (getline(tokenStream, token, L'_'))
	{
		tokens.push_back(token);
	}

	if (token.size() >= 2)
	{
		wstring tagCategoryPart = tokens[token.size() - 2];
		wstring numberPart = tokens[token.size() - 1];
		return { tagCategoryPart, numberPart };
	}

	return { L"", L"" };
}

//_uint CDesert_Architecture::Object_Number(const _tchar* pComponentTag)
//{
//	if (pComponentTag == nullptr)
//		return 0;
//
//	const _tchar* pLastUnderscore = wcsrchr(pComponentTag, L'_');
//
//	if (pLastUnderscore == nullptr || *(pLastUnderscore + 1) == L'\0')
//		return 0;
//
//	const _tchar* pSuffix = pLastUnderscore + 1;
//
//	if (wcscmp(pSuffix, TEXT("A")) == 0)
//		return 1;
//	else if (wcscmp(pSuffix, TEXT("B")) == 0)
//		return 2;
//	else if (wcscmp(pSuffix, TEXT("C")) == 0)
//		return 3;
//	else if (wcscmp(pSuffix, TEXT("D")) == 0)
//		return 4;
//	else if (wcscmp(pSuffix, TEXT("E")) == 0)
//		return 5;
//	else if (wcscmp(pSuffix, TEXT("F")) == 0)
//		return 6;
//	else
//		return 0;
//}

CDesert_Architecture* CDesert_Architecture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDesert_Architecture* pInstance = new CDesert_Architecture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDesert_Architecture::Clone(void* pArg)
{
	CDesert_Architecture* pInstance = new CDesert_Architecture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDesert_Architecture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDesert_Architecture::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
