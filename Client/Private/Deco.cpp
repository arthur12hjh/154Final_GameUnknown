#include "pch.h"
#include "Deco.h"
#include "GameInstance.h"

CDeco::CDeco(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CDeco::CDeco(const CDeco& Prototype)
	: CActor{ Prototype }
{
}

HRESULT CDeco::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDeco::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ACTOR_DESC* pDesc = static_cast<ACTOR_DESC*>(pArg);

    SetCullingCollider(pDesc->iObjectID);
    _matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    m_pCullingCollider->UpdateColiision(worldMatrix);

	if (FAILED(Ready_Components(pDesc->szVIBuffer_PrototypeName)))
		return E_FAIL;

    if (wcsstr(pDesc->szVIBuffer_PrototypeName, TEXT("Container")) || 
        wcsstr(pDesc->szVIBuffer_PrototypeName, TEXT("Fence")) || 
        wcsstr(pDesc->szVIBuffer_PrototypeName, TEXT("Base")) ||
        wcsstr(pDesc->szVIBuffer_PrototypeName, TEXT("Garden")) || 
        wcsstr(pDesc->szVIBuffer_PrototypeName, TEXT("Box")) ||
        wcsstr(pDesc->szVIBuffer_PrototypeName, TEXT("Furniture")))
    {
        if (FAILED(Ready_Col(pDesc->szVIBuffer_PrototypeName)))
            return S_OK;
    }

	return S_OK;
}

void CDeco::Priority_Update(_float fTimeDelta)
{
}

void CDeco::Update(_float fTimeDelta)
{
}

void CDeco::Late_Update(_float fTimeDelta)
{
    if (!m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
    {
        return;
    }

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(RENDER::OCCLUSION, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
}

HRESULT CDeco::Render()
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

HRESULT CDeco::Render_Shadow()
{

    return S_OK;
}

HRESULT CDeco::Ready_Components(const _tchar* pComponentTag)
{
    auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
    _float3 vColliderSize = pCullingCollider->GetBounding().Extents;

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDeco::Ready_Col(const _tchar* pComponentTag)
{
    /* Com_Model_COL */
    _wstring strComponentTag = pComponentTag;
    strComponentTag += TEXT("_COL");


    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), strComponentTag,
        TEXT("Com_Model_COL"), reinterpret_cast<CComponent**>(&m_pColModelCom))))
        return E_FAIL;

    // 충돌용 메시 피직스 세팅 조건
    // 1. 충돌용 메시 생성 이후에 RigidBody를 세팅해주셔야합니다.
    // 2. 해당 객체의 위치가 설정된 뒤에 RigidBody를 세팅해주셔야 합니다.

    // 세팅 방법 보고도 잘 이해 안되면 물어봐주세요 키네마틱, 다이나믹, 스태틱 세팅 중요해요
    PxUserData tUserData;
    // 엘레베이터 식별용 문자열. 이건 나중에 엘베말고 다른데에 넣을떄 저랑 얘기하고 정해서 넣어주세요
    tUserData.szActorTag = TEXT("Deco_Actor");

    // 리지드 바디 Desc 세팅. 
    // F12 타고 들어가서 머테리얼이랑 Mass, userdata, shape, type 부분 위주로 살펴보세요.
    CRigidBody::RIGIDBODY_DESC RigidBodyDesc;
    // 콜라이더 모양.
    // ->> 메시는 TRIANGLE, 박스나 캡슐은 BOX, CAPSULE 다 따로 있으니까
    // F12 타고 들어가서 한번 확인해보세요.
    RigidBodyDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::TRIANGLE;

    // 충돌처리를 할지말지 
    // DYNAMIC : 충돌 
    // KINEMATIC : 충돌 X
    // STATIC : 충돌 O, 대신 고정되어 있음.
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

HRESULT CDeco::Bind_ShaderResources()
{
    _bool bFlag = { false };
    if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthB", &bFlag, sizeof(_bool))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthW", &bFlag, sizeof(_bool))))
        return E_FAIL;

	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

void CDeco::SetCullingCollider(_uint iObjectID)
{
    auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
    switch (iObjectID)
    {
    case 1: // Box_1A (1, 0.5, 0.8)
        pCullingCollider->SetCollision({ 0.f, 0.4f, 0.f }, {}, { 1.f, 0.5f, 0.8f });
        break;
    case 2: // Box_1B (1, 0.5, 0.8)
        pCullingCollider->SetCollision({ 0.f, 0.4f, 0.f }, {}, { 1.f, 0.5f, 0.8f });
        break;
    case 3: // Box_2C (0.8, 0.5, 0.8)
        pCullingCollider->SetCollision({ 0.f, 0.4f, 0.3f }, {}, { 0.8f, 0.5f, 0.8f });
        break;
    case 4: // Box_4A (1, 0.8, 0.8)
        pCullingCollider->SetCollision({ 0.f, 0.6f, 0.f }, {}, { 1.f, 0.8f, 0.8f });
        break;
    case 5: // Box_5A (0.8, 0.5, 0.8)
        pCullingCollider->SetCollision({ 0.f, 0.4f, 0.f }, {}, { 0.8f, 0.5f, 0.8f });
        break;
    case 6: // Box_6A (2.5, 0.8, 2.2)
        pCullingCollider->SetCollision({ 0.f, 0.65f, 0.f }, {}, { 2.5f, 0.8f, 2.2f });
        break;
    case 7: // Box_11A (2.5, 0.8, 2.2)
        pCullingCollider->SetCollision({ 0.f, 0.65f, 0.f }, {}, { 2.5f, 0.8f, 2.2f });
        break;
    case 8: // Box_13A (1.5, 1.2, 1)
        pCullingCollider->SetCollision({ 0.f, 1.f, 0.f }, {}, { 1.5f, 1.2f, 1.f });
        break;
    case 9: // Box_14A (1.5, 0.5, 0.8)
        pCullingCollider->SetCollision({ 0.f, 0.4f, 0.f }, {}, { 1.5f, 0.5f, 0.8f });
        break;
    case 10: // Box_16A (1.4, 0.6, 1.2) // TODO
        pCullingCollider->SetCollision({ 0.f, 0.4f, 0.f }, {}, { 1.4f, 0.6f, 1.2f });
        break;
    case 11: // Box_16B (1.5, 0.8, 1.4)
        pCullingCollider->SetCollision({ 0.f, 0.7f, 0.f }, {}, { 1.5f, 0.8f, 1.4f });
        break;
    case 12: // Box_19A (1.5, 1.4, 1.4)
        pCullingCollider->SetCollision({ -1.3f, 1.2f, 1.1f }, {}, { 1.5f, 1.4f, 1.4f });
        break;
    case 13: // Box_20A (3.5, 4, 2.5)
        pCullingCollider->SetCollision({ -0.2f, 3.5f, -0.2f }, {}, { 3.8f, 4.f, 2.5f });
        break;
    case 14: // Box_20B (3.5, 2.5, 3.5)
        pCullingCollider->SetCollision({ 0.f, 1.8f, 0.f }, {}, { 3.5f, 2.2f, 3.5f });
        break;
    case 15: // Box_20C (3.5, 2.5, 3.5)
        pCullingCollider->SetCollision({ 0.f, 2.f, 0.3f }, {}, { 3.5f, 2.5f, 3.2f });
        break;
    case 16: // Box_20D (3.5, 3, 2.5)
        pCullingCollider->SetCollision({ 0.f, 2.5f, 0.f }, {}, { 3.5f, 3.f, 2.5f });
        break;
    case 17: // Box_20E (3, 2.5, 2.5)
        pCullingCollider->SetCollision({ 0.f, 2.f, 0.f }, {}, { 3.f, 2.5f, 2.5f });
        break;
    case 18: // Box_20F (2, 2, 2)
        pCullingCollider->SetCollision({ 0.f, 1.6f, 0.f }, {}, { 2.f, 2.f, 2.f });
        break;
    case 19: // Box_21A (3, 2, 3)
        pCullingCollider->SetCollision({ 0.f, 1.6f, 0.f }, {}, { 3.f, 2.f, 3.f });
        break;
    case 20: // Box_21B (3, 4, 2.5)
        pCullingCollider->SetCollision({ 0.f, 3.5f, 0.f }, {}, { 3.f, 4.f, 2.5f });
        break;
    case 21: // Box_26A (2.5, 1.5, 2.5)
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 2.5f, 1.5f, 2.5f });
        break;
    case 22: // Garden_1A (3, 1.5, 3)
        pCullingCollider->SetCollision({ 0.f, 1.2f, 0.f }, {}, { 3.f, 1.5f, 3.f });
        break;
    case 23: // Garden_1B (8, 2.5, 18)
        pCullingCollider->SetCollision({ 0.f, 2.1f, 0.f }, {}, { 8.f, 2.5f, 18.f });
        break;
    case 24: // Garden_1C (25, 2.5, 4)
        pCullingCollider->SetCollision({ 0.f, 2.2f, 0.f }, {}, { 25.f, 2.5f, 4.f });
        break;
    case 25: // Restroom_4A (0.5, 0.8, 1.2)
        pCullingCollider->SetCollision({ -0.25f, 0.6f, 1.05f }, {}, { 0.5f, 0.8f, 1.2f });
        break;
    case 26: // Poster_1A (1, 1, 1)
        pCullingCollider->SetCollision({ 0.f, 0.f, 0.f }, {}, { 1.f, 1.f, 1.f });
        break;
    case 27: // Poster_2A (3.5, 4.5, 0.6)
        pCullingCollider->SetCollision({ 0.f, -4.f, 0.2f }, {}, { 3.5f, 4.5f, 0.6f });
        break;
    case 28: // Poster_2B (3.5, 4, 0.6)
        pCullingCollider->SetCollision({ 0.f, -4.f, 0.2f }, {}, { 3.5f, 4.f, 0.6f });
        break;
    case 29: // Poster_3A (1.5, 2.2, 0.4)
        pCullingCollider->SetCollision({ 0.f, -2.f, 0.1f }, {}, { 1.5f, 2.2f, 0.4f });
        break;
    case 30: // Poster_3B (1.5, 2.2, 0.5)
        pCullingCollider->SetCollision({ 0.f, -2.f, 0.1f }, {}, { 1.5f, 2.2f, 0.5f });
        break;
    case 31: // Poster_4A (1.5, 2.3, 0.5)
        pCullingCollider->SetCollision({ 0.f, -2.f, 0.1f }, {}, { 1.5f, 2.3f, 0.5f });
        break;
    case 32: // Poster_4B (1.5, 2.3, 0.5)
        pCullingCollider->SetCollision({ 0.f, -2.1f, 0.1f }, {}, { 1.5f, 2.3f, 0.5f });
        break;
    case 33: // Poster_4C (1.5, 2.3, 0.5)
        pCullingCollider->SetCollision({ 0.f, -2.1f, 0.1f }, {}, { 1.5f, 2.3f, 0.5f });
        break;
    case 34: // Poster_4E (1.5, 2.3, 0.5)
        pCullingCollider->SetCollision({ 0.f, -2.1f, 0.1f }, {}, { 1.5f, 2.3f, 0.5f });
        break;
    case 35: // Duct_1A (15, 6.5, 8.5)
        pCullingCollider->SetCollision({ -12.f, 6.f, 6.f }, {}, { 15.f, 6.5f, 8.5f });
        break;
    case 36: // Duct_1B (5.5, 3.5, 7.5)
        pCullingCollider->SetCollision({ 0.f, 3.f, 0.f }, {}, { 5.5f, 3.5f, 7.5f });
        break;
    case 37: // Duct_1C (2, 2, 2)
        pCullingCollider->SetCollision({ 0.f, 1.5f, 0.f }, {}, { 2.f, 2.f, 2.f });
        break;
    case 38: // Duct_1D (2, 5, 2)
        pCullingCollider->SetCollision({ 0.f, 4.6f, 0.f }, {}, { 2.f, 5.f, 2.f });
        break;
    case 39: // Duct_1E (2.5, 2.5, 2)
        pCullingCollider->SetCollision({ -0.5f, 2.f, 0.f }, {}, { 2.5f, 2.5f, 2.f });
        break;
    case 40: // Duct_1G (4.5, 6.2, 2)
        pCullingCollider->SetCollision({ 2.5f, 5.7f, 0.f }, {}, { 4.5f, 6.2f, 2.f });
        break;
    case 41: // Duct_1H (6, 2.5, 46)
        pCullingCollider->SetCollision({ -5.f, 0.f, 43.f }, {}, { 6.f, 2.5f, 46.f });
        break;
    case 42: // Duct_3A (1, 0.8, 0.5)
        pCullingCollider->SetCollision({ 0.f, 0.6f, 0.f }, {}, { 1.f, 0.8f, 0.5f });
        break;
    case 43: // Duct_3B (1, 0.8, 0.5)
        pCullingCollider->SetCollision({ 0.f, 0.6f, 0.f }, {}, { 1.f, 0.8f, 0.5f });
        break;
    case 44: // Duct_3C (1, 0.4, 0.5)
        pCullingCollider->SetCollision({ 0.05f, -0.3f, 0.f }, {}, { 1.f, 0.4f, 0.5f });
        break;
    case 45: // Duct_4A (1.8, 2.4, 1.5)
        pCullingCollider->SetCollision({ 0.f, 2.f, 0.f }, {}, { 1.8f, 2.4f, 1.5f });
        break;
    case 46: // Duct_4B (2.5, 2.4, 1.5)
        pCullingCollider->SetCollision({ -0.05f, 2.1f, 0.f }, {}, { 2.6f, 2.4f, 1.5f });
        break;
    case 47: // Duct_4C (3.5, 2.4, 1.5)
        pCullingCollider->SetCollision({ 0.f, 2.1f, 0.f }, {}, { 3.5f, 2.4f, 1.5f });
        break;
    case 48: // Duct_6A (20.5, 4, 10)
        pCullingCollider->SetCollision({ 0.f, 17.5f, -16.f }, {}, { 20.5f, 4.f, 10.f });
        break;
    case 49: // Duct_8A (2.5, 2, 4.5)
        pCullingCollider->SetCollision({ 0.f, 1.7f, 0.f }, {}, { 2.5f, 2.f, 4.5f });
        break;
    case 50: // Duct_9A (1.5, 0.5, 0.8)
        pCullingCollider->SetCollision({ -1.2f, -0.4f, 0.6f }, {}, { 1.5f, 0.5f, 0.8f });
        break;
    case 51: // Duct_9B (0.8, 0.5, 0.8)
        pCullingCollider->SetCollision({ -0.6f, -0.4f, 0.6f }, {}, { 0.8f, 0.5f, 0.8f });
        break;
    case 52: // Duct_9C (1.5, 0.5, 0.8)
        pCullingCollider->SetCollision({ -1.2f, -0.4f, 0.6f }, {}, { 1.5f, 0.5f, 0.8f });
        break;
    case 53: // Duct_9D (0.8, 0.5, 0.8)
        pCullingCollider->SetCollision({ -0.6f, -0.4f, 0.6f }, {}, { 0.8f, 0.5f, 0.8f });
        break;
    case 54: // Duct_10A (1.5, 0.5, 0.8)
        pCullingCollider->SetCollision({ -1.2f, -0.4f, 0.6f }, {}, { 1.5f, 0.5f, 0.8f });
        break;
    case 55: // Duct_13A (3, 3, 1.5)
        pCullingCollider->SetCollision({ 0.f, 2.9f, 0.f }, {}, { 3.f, 3.f, 1.5f });
        break;
    case 56: // Statue_1A (2.3, 3.5, 1.5)
        pCullingCollider->SetCollision({ -0.2f, 3.3f, 0.15f }, {}, { 2.3f, 3.5f, 1.5f });
        break;
    case 57: // Statue_19B (27, 31, 15)
        pCullingCollider->SetCollision({ -7.f, 30.f, -2.f }, {}, { 27.f, 31.f, 15.f });
        break;
    case 58: // Statue_24B (5.5, 5.5, 3.3)
        pCullingCollider->SetCollision({ 0.f, 5.f, 0.f }, {}, { 5.5f, 5.5f, 3.3f });
        break;
    case 59: // Statue_31B (4.5, 2.5, 2.3)
        pCullingCollider->SetCollision({ 0.f, 2.3f, 0.f }, {}, { 4.5f, 2.5f, 2.3f });
        break;
    case 60: // Statue_40B (2.3, 3.5, 1.5)
        pCullingCollider->SetCollision({ 0.f, 3.15f, 0.f }, {}, { 2.3f, 3.3f, 1.5f });
        break;
    case 61: // Furniture_7A (1.5, 1.2, 0.7)
        pCullingCollider->SetCollision({ 0.f, 1.f, 0.f }, {}, { 1.5f, 1.1f, 0.7f });
        break;
    case 62: // Furniture_9A (1.4, 1, 1)
        pCullingCollider->SetCollision({ 0.f, 0.8f, 0.f }, {}, { 1.4f, 0.9f, 1.f });
        break;
    case 63: // Furniture_47A (0.8, 1.4, 0.8)
        pCullingCollider->SetCollision({ 0.f, 1.2f, 0.f }, {}, { 0.8f, 1.4f, 0.8f });
        break;
    case 64: // Furniture_50A (0.2, 0.5, 0.2)
        pCullingCollider->SetCollision({ 0.f, 0.4f, 0.f }, {}, { 0.2f, 0.5f, 0.2f });
        break;
    case 65: // Furniture_50B (0.2, 0.4, 0.2)
        pCullingCollider->SetCollision({ 0.f, 0.3f, 0.f }, {}, { 0.2f, 0.4f, 0.2f });
        break;
    case 66: // Furniture_50C (0.2, 0.4, 0.2)
        pCullingCollider->SetCollision({ 0.f, 0.3f, 0.f }, {}, { 0.2f, 0.4f, 0.2f });
        break;
    case 67: // Furniture_50D (0.15, 0.25, 0.15)
        pCullingCollider->SetCollision({ 0.f, 0.2f, 0.f }, {}, { 0.15f, 0.25f, 0.15f });
        break;
    case 68: // Furniture_51A (0.7, 1.5, 0.4)
        pCullingCollider->SetCollision({ 0.f, 0.3f, 0.f }, {}, { 0.7f, 1.5f, 0.4f });
        break;
    case 69: // Furniture_57A (1, 0.6, 0.55)
        pCullingCollider->SetCollision({ 0.f, 0.5f, 0.f }, {}, { 1.f, 0.6f, 0.55f });
        break;
    case 70: // Furniture_59A (1.3, 1.2, 1.3)
        pCullingCollider->SetCollision({ 0.f, 1.f, 0.f }, {}, { 1.3f, 1.2f, 1.3f });
        break;
    case 71: // Furniture_77A (1.8, 1.4, 1)
        pCullingCollider->SetCollision({ 0.f, 1.2f, 0.f }, {}, { 1.8f, 1.4f, 1.f });
        break;
    case 72: // Furniture_79A (0.8, 1.4, 0.8)
        pCullingCollider->SetCollision({ 0.f, 1.2f, 0.f }, {}, { 0.8f, 1.4f, 0.8f });
        break;
    case 73: // Furniture_83A (1.5, 1.1, 1.5)
        pCullingCollider->SetCollision({ 0.f, 1.05f, 0.f }, {}, { 1.5f, 1.1f, 1.5f });
        break;
    case 74: // Furniture_87B (0.5, 0.5, 0.7)
        pCullingCollider->SetCollision({ 0.f, 0.47f, 0.f }, {}, { 0.5f, 0.5f, 0.7f });
        break;
    case 75: // Furniture_87D (0.5, 0.5, 0.7)
        pCullingCollider->SetCollision({ 0.f, 0.47f, 0.f }, {}, { 0.5f, 0.5f, 0.7f });
        break;
    case 80: // Lamp_47A
        pCullingCollider->SetCollision({ 0.f, 4.f, 1.f }, {}, { 2.f, 4.f, 2.f });
        break;
    case 81: // Poster_4D
        pCullingCollider->SetCollision({ 0.f, -2.f, 0.f }, {}, { 2.f, 2.5f, 1.f });
        break;
    case 82: // Wheel_1A
    case 83: // Wheel_1B
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.5f }, {}, { 3.f, 2.f, 3.f });
        break;
    case 84: // Wheel_1C
        pCullingCollider->SetCollision({ 0.f, 3.5f, 0.f }, {}, { 3.f, 4.f, 3.f });
        break;
    case 85: // Base_1A
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 10.f, 2.f, 10.f });
        break;
    case 86: // Camp_1B
        pCullingCollider->SetCollision({ 0.f, 0.7f, 0.f }, {}, { 2.f, 1.f, 2.f });
        break;
    case 87: // Camp_1D
        pCullingCollider->SetCollision({ 0.5f, 1.4f, 0.f }, {}, { 1.2f, 2.f, 1.2f });
        break;
    case 88: // Camp_1E
        pCullingCollider->SetCollision({ 0.f, 1.05f, 0.f }, {}, { 1.1f, 1.5f, 1.1f });
        break;
    case 89: // Camp_1F
        pCullingCollider->SetCollision({ 0.f, 1.f, 0.f }, {}, { 1.f, 1.f, 1.f });
        break;
    case 90: // Camp_1G
        pCullingCollider->SetCollision({ 0.f, 1.3f, 0.f }, {}, { 1.f, 1.3f, 1.f });
        break;
    case 91: // Camp_1H
        pCullingCollider->SetCollision({ 0.f, 0.5f, 0.f }, {}, { 0.8f, 0.5f, 0.8f });
        break;
    case 92: // Camp_1J
        pCullingCollider->SetCollision({ 0.f, 0.49f, 0.f }, {}, { 1.f, 0.7f, 1.5f });
        break;
    case 93: // Camp_1K
        pCullingCollider->SetCollision({ 0.f, 0.56f, 0.f }, {}, { 1.f, 0.8f, 1.8f });
        break;
    case 94: // Camp_1L
        pCullingCollider->SetCollision({ 0.f, 0.35f, 0.f }, {}, { 0.8f, 0.5f, 0.8f });
        break;
    case 95: // Camp_1N
        pCullingCollider->SetCollision({ 0.f, 0.5f, 0.f }, {}, { 0.5f, 0.5f, 0.5f });
        break;
    case 96: // Camp_1R
        pCullingCollider->SetCollision({ 0.f, 0.49f, 0.f }, {}, { 0.7f, 0.7f, 0.7f });
        break;
    case 97: // Camp_1S
        pCullingCollider->SetCollision({ 0.f, 0.7f, 0.f }, {}, { 0.7f, 1.f, 0.7f });
        break;
    case 98: // Camp_1T
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 1.f, 2.f, 1.f });
        break;
    case 99: // Container_2A
        pCullingCollider->SetCollision({ 0.f, 2.1f, 0.f }, {}, { 3.f, 3.f, 5.f });
        break;
    case 100: // Container_2B
    case 102: // Container_2D
        pCullingCollider->SetCollision({ 0.f, 1.05f, 0.f }, {}, { 2.f, 1.5f, 2.f });
        break;
    case 101: // Container_2C
    case 103: // Container_3A
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 2.f, 2.f, 2.f });
        break;
    case 104: // Container_4B
        pCullingCollider->SetCollision({ 0.f, 6.f, 0.f }, {}, { 4.f, 7.f, 4.f });
        break;
    case 105: // Container_5A
    case 107: // Container_5C
    case 108: // Container_5D
        pCullingCollider->SetCollision({ 0.f, 2.8f, 0.f }, {}, { 4.f, 4.f, 8.f });
        break;
    case 106: // Container_5B
        pCullingCollider->SetCollision({ 0.f, 2.8f, 0.f }, {}, { 6.f, 4.f, 8.f });
        break;
    case 109: // Container_5E
        pCullingCollider->SetCollision({ 1.f, 2.1f, 0.f }, {}, { 2.f, 3.f, 1.f });
        break;
    case 110: // Container_7B
        pCullingCollider->SetCollision({ 0.f, 8.f, 0.f }, {}, { 10.f, 2.f, 20.f });
        break;
    case 111: // Container_7F
        pCullingCollider->SetCollision({ 0.f, 7.f, 0.f }, {}, { 12.f, 10.f, 25.f });
        break;
    case 112: // Fence_1A
    case 113: // Fence_1B
    case 114: // Fence_1F
    case 115: // Fence_1H
        pCullingCollider->SetCollision({ -3.f, 3.5f, 0.f }, {}, { 5.f, 4.f, 1.f });
        break;
    case 116: // Vehicle_2A
        pCullingCollider->SetCollision({ -2.f, 3.5f, 0.f }, {}, { 7.f, 4.f, 5.f });
        break;
    case 117: // Vehicle_2B
        pCullingCollider->SetCollision({ 0.f, 4.5f, 0.f }, {}, { 12.f, 5.f, 5.f });
        break;
    case 118: // Vehicle_3B
        pCullingCollider->SetCollision({ 0.f, 2.1f, 0.f }, {}, { 3.f, 3.f, 7.f });
        break;
    case 119: // Vehicle_4C
        pCullingCollider->SetCollision({ 0.f, 2.1f, 0.f }, {}, { 7.f, 3.f, 4.f });
        break;
    case 120: // Vehicle_6A
        pCullingCollider->SetCollision({ 0.f, 2.1f, 0.f }, {}, { 4.f, 3.f, 7.f });
        break;
    case 121: // Vehicle_8B
        pCullingCollider->SetCollision({ 0.f, 3.5f, 0.f }, {}, { 15.f, 5.f, 5.f });
        break;
    case 122: // Vehicle_14A
    case 123: // Vehicle_14B
        pCullingCollider->SetCollision({ 0.f, 3.5f, 0.f }, {}, { 12.f, 5.f, 6.f });
        break;
    case 124: // Sign_11A
        pCullingCollider->SetCollision({ 0.f, 7.f, 0.f }, {}, { 6.f, 10.f, 4.f });
        break;
    case 125: // Sign_11B
        pCullingCollider->SetCollision({ 0.f, 7.f, 0.f }, {}, { 10.f, 10.f, 4.f });
        break;
    case 126: // Sign_11F
        pCullingCollider->SetCollision({ 0.f, 4.2f, 0.f }, {}, { 15.f, 6.f, 4.f });
        break;
    case 127: // Sign_12B
        pCullingCollider->SetCollision({ 0.f, 0.f, 0.f }, {}, { 4.f, 2.f, 2.f });
        break;
    case 128: // Sign_12C
        pCullingCollider->SetCollision({ 0.f, 2.1f, 0.f }, {}, { 2.f, 3.f, 2.f });
        break;
    case 129: // Sign_36B
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 4.f, 2.f, 1.f });
        break;
    case 130: // Crane_1C
        pCullingCollider->SetCollision({ -1.f, -5.f, 0.f }, {}, { 4.f, 7.f, 2.f });
        break;
    case 131: // Crane_11
        pCullingCollider->SetCollision({ 0.f, 35.f, 20.f }, {}, { 15.f, 40.f, 55.f });
        break;
    case 132: // Crane_13
        pCullingCollider->SetCollision({ 0.f, 22.f, 10.f }, {}, { 10.f, 28.f, 20.f });
        break;
    case 133: // Trash_1A
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 4.f, 2.f, 3.f });
        break;
    case 134: // Trash_2A
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 5.f, 2.f, 3.f });
        break;
    case 135: // Trash_2B
        pCullingCollider->SetCollision({ 0.f, 2.1f, 0.f }, {}, { 6.f, 3.f, 3.f });
        break;
    case 136: // Trash_4A
        pCullingCollider->SetCollision({ 0.f, 0.7f, 0.f }, {}, { 2.f, 1.f, 2.f });
        break;
    case 137: // Trash_9A
        pCullingCollider->SetCollision({ 2.f, -2.f, -2.f }, {}, { 4.f, 3.f, 4.f });
        break;
    case 138: // Trash_17A
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 4.f, 2.f, 4.f });
        break;
    }
}


CDeco* CDeco::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDeco* pInstance = new CDeco(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDeco::Clone(void* pArg)
{
	CDeco* pInstance = new CDeco(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDeco");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDeco::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
