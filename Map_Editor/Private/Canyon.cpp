#include "pch.h"
#include "Canyon.h"
#include "GameInstance.h"

CCanyon::CCanyon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDesertObject{ pDevice, pContext }
{
}

CCanyon::CCanyon(const CCanyon& Prototype)
	: CDesertObject{ Prototype }
{
}

HRESULT CCanyon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCanyon::Initialize(void* pArg)
{

    DESERT_OBJECT_DESC* pDesc = static_cast<DESERT_OBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pDesc && pDesc->pComponentTag)
	{
		wcsncpy_s(m_ComponentTag, 256, pDesc->pComponentTag, _TRUNCATE);
	}

	if (FAILED(Ready_Components(m_ComponentTag)))
		return E_FAIL;

	m_iObjectID = Object_Number(m_ComponentTag);

	SetCullingCollider(m_iObjectID);
	_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	m_pCullingCollider->UpdateColiision(worldMatrix);

	return S_OK;
}

void CCanyon::Priority_Update(_float fTimeDelta)
{
}

void CCanyon::Update(_float fTimeDelta)
{

}

void CCanyon::Late_Update(_float fTimeDelta)
{
    //if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
        m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
}

HRESULT CCanyon::Render()
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


		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CCanyon::Ready_Components(const _tchar* pComponentTag)
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	///* Com_Collider_OBB */
	//COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};
	//OBBDesc.vSize = _float3(1.f, 1.f, 1.f);
	//OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y, 0.f);

	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Collider_OBB"),
	//	TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CCanyon::Bind_ShaderResources()
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

void CCanyon::SetCullingCollider(_uint iObjectID)
{
    auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
	switch (iObjectID)
	{
	case 1:
		pCullingCollider->SetCollision({ 0.f, 2.0f, 0.f }, {}, { 3, 3, 32 });
		break;
	case 2: case 9: case 10: case 13: case 14: case 16:
	case 30: case 62: case 63:
		pCullingCollider->SetCollision({ 0.f, 1.33f, 0.f }, {}, { 3, 2, 2 });
		break;
	case 3:
		pCullingCollider->SetCollision({ 0.f, 8.67f, 0.f }, {}, { 13, 13, 3 });
		break;
	case 4: case 8: case 11: case 15: case 25:
	case 28: case 31: case 32: case 64:
		pCullingCollider->SetCollision({ 0.f, 1.33f, 0.f }, {}, { 2, 2, 2 });
		break;
	case 5:
		pCullingCollider->SetCollision({ 0.f, 3.33f, 0.f }, {}, { 10, 5, 4 });
		break;
	case 6: case 44:
		pCullingCollider->SetCollision({ 0.f, 1.33f, 0.f }, {}, { 4, 2, 1 });
		break;
	case 7:
		pCullingCollider->SetCollision({ 0.f, 43.33f, 0.f }, {}, { 18, 65, 18 });
		break;
	case 12:
		pCullingCollider->SetCollision({ 0.f, 2.0f, 0.f }, {}, { 4, 3, 4 });
		break;
	case 17:
		pCullingCollider->SetCollision({ 0.f, 4.67f, 0.f }, {}, { 15, 7, 7 });
		break;
	case 18:
		pCullingCollider->SetCollision({ 0.f, 9.33f, 0.f }, {}, { 11, 14, 11 });
		break;
	case 19:
		pCullingCollider->SetCollision({ 0.f, 8.67f, 0.f }, {}, { 13, 13, 5 });
		break;
	case 20:
		pCullingCollider->SetCollision({ 0.f, 3.33f, 0.f }, {}, { 14, 5, 4 });
		break;
	case 21:
		pCullingCollider->SetCollision({ 0.f, 2.67f, 0.f }, {}, { 18, 4, 8 });
		break;
	case 22:
		pCullingCollider->SetCollision({ 0.f, 23.33f, 0.f }, {}, { 12, 35, 9 });
		break;
	case 23:
		pCullingCollider->SetCollision({ 0.f, 16.67f, 0.f }, {}, { 22, 25, 10 });
		break;
	case 24:
		pCullingCollider->SetCollision({ 0.f, 15.33f, 0.f }, {}, { 35, 23, 10 });
		break;
	case 26: case 27:
		pCullingCollider->SetCollision({ 0.f, 0.67f, 0.f }, {}, { 4, 1, 4 });
		break;
	case 29:
		pCullingCollider->SetCollision({ 0.f, 1.33f, 0.f }, {}, { 3, 2, 3 });
		break;
	case 33:
		pCullingCollider->SetCollision({ 0.f, 1.33f, 0.f }, {}, { 4, 2, 3 });
		break;
	case 34:
		pCullingCollider->SetCollision({ 0.f, 2.67f, 0.f }, {}, { 5, 4, 5 });
		break;
	case 35: case 47:
		pCullingCollider->SetCollision({ 0.f, 13.33f, 0.f }, {}, { 55, 20, 30 });
		break;
	case 36:
		pCullingCollider->SetCollision({ 0.f, 12.0f, 0.f }, {}, { 62, 18, 30 });
		break;
	case 37:
		pCullingCollider->SetCollision({ 0.f, 20.67f, 0.f }, {}, { 58, 31, 15 });
		break;
	case 38:
		pCullingCollider->SetCollision({ 0.f, 12.0f, 0.f }, {}, { 15, 18, 8 });
		break;
	case 39:
		pCullingCollider->SetCollision({ 0.f, 22.67f, 0.f }, {}, { 50, 34, 40 });
		break;
	case 40:
		pCullingCollider->SetCollision({ 0.f, 5.33f, 0.f }, {}, { 15, 8, 10 });
		break;
	case 41:
		pCullingCollider->SetCollision({ 0.f, 26.67f, 0.f }, {}, { 25, 40, 40 });
		break;
	case 42:
		pCullingCollider->SetCollision({ 0.f, 1.33f, 0.f }, {}, { 2, 2, 1 });
		break;
	case 43:
		pCullingCollider->SetCollision({ 0.f, 2.0f, 0.f }, {}, { 5, 3, 2 });
		break;
	case 45:
		pCullingCollider->SetCollision({ 0.f, 12.0f, 0.f }, {}, { 60, 18, 25 });
		break;
	case 46:
		pCullingCollider->SetCollision({ 0.f, 3.33f, 0.f }, {}, { 18, 5, 23 });
		break;
	case 48:
		pCullingCollider->SetCollision({ 0.f, 10.0f, 0.f }, {}, { 15, 15, 10 });
		break;
	case 49:
		pCullingCollider->SetCollision({ 0.f, 13.33f, 0.f }, {}, { 20, 20, 13 });
		break;
	case 50:
		pCullingCollider->SetCollision({ 0.f, 10.0f, 0.f }, {}, { 30, 15, 18 });
		break;
	case 51:
		pCullingCollider->SetCollision({ 0.f, 16.67f, 0.f }, {}, { 35, 25, 15 });
		break;
	case 52:
		pCullingCollider->SetCollision({ 0.f, 18.67f, 0.f }, {}, { 28, 28, 17 });
		break;
	case 53:
		pCullingCollider->SetCollision({ 0.f, 8.0f, 0.f }, {}, { 105, 12, 45 });
		break;
	case 54:
		pCullingCollider->SetCollision({ 0.f, 5.33f, 0.f }, {}, { 20, 8, 10 });
		break;
	case 55:
		pCullingCollider->SetCollision({ 0.f, 4.0f, 0.f }, {}, { 28, 6, 10 });
		break;
	case 56:
		pCullingCollider->SetCollision({ 0.f, 1.33f, 0.f }, {}, { 25, 2, 7 });
		break;
	case 57:
		pCullingCollider->SetCollision({ 0.f, 2.67f, 0.f }, {}, { 22, 4, 7 });
		break;
	case 58:
		pCullingCollider->SetCollision({ 0.f, 2.67f, 0.f }, {}, { 25, 4, 10 });
		break;
	case 59:
		pCullingCollider->SetCollision({ 0.f, 2.0f, 0.f }, {}, { 15, 3, 8 });
		break;
	case 60:
		pCullingCollider->SetCollision({ 0.f, 0.67f, 0.f }, {}, { 4, 1, 1 });
		break;
	case 61:
		pCullingCollider->SetCollision({ 0.f, 0.67f, 0.f }, {}, { 3, 1, 1 });
		break;
	case 65:
		pCullingCollider->SetCollision({ 0.f, 8.0f, 0.f }, {}, { 8, 12, 8 });
		break;
	case 66:
		pCullingCollider->SetCollision({ 0.f, 8.67f, 0.f }, {}, { 15, 13, 6 });
		break;
	case 67: case 68: case 69: case 70: case 71: case 72:
	case 73: case 74: case 76: case 77: case 78: case 79:
		pCullingCollider->SetCollision({ 0.f, 333.33f, 0.f }, {}, { 1000, 500, 1000 });
		break;
	case 75:
		pCullingCollider->SetCollision({ 0.f, 0.67f, 0.f }, {}, { 6, 1, 6 }); 
		break;
	}

}

_uint CCanyon::Object_Number(const _tchar* pComponentTag)
{
	if (pComponentTag == nullptr)
		return 0;

	const _tchar* pLastUnderscore = wcsrchr(pComponentTag, L'_');

	if (pLastUnderscore == nullptr || *(pLastUnderscore + 1) == L'\0')
		return 0;

	const _tchar* pSuffix = pLastUnderscore + 1;

	// 2. 요청 목록 기반 조건문 (총 87개 항목)

	if (wcscmp(pSuffix, TEXT("1A")) == 0)
		return 1;
	else if (wcscmp(pSuffix, TEXT("2A")) == 0)
		return 2;
	else if (wcscmp(pSuffix, TEXT("3A")) == 0)
		return 3;
	else if (wcscmp(pSuffix, TEXT("4A")) == 0)
		return 4;
	else if (wcscmp(pSuffix, TEXT("5A")) == 0)
		return 5;
	else if (wcscmp(pSuffix, TEXT("6A")) == 0)
		return 6;
	else if (wcscmp(pSuffix, TEXT("12A")) == 0)
		return 7;
	else if (wcscmp(pSuffix, TEXT("14A")) == 0)
		return 8;
	else if (wcscmp(pSuffix, TEXT("14B")) == 0)
		return 9;
	else if (wcscmp(pSuffix, TEXT("15A")) == 0)
		return 10;
	else if (wcscmp(pSuffix, TEXT("16A")) == 0)
		return 11;
	else if (wcscmp(pSuffix, TEXT("16B")) == 0)
		return 12;
	else if (wcscmp(pSuffix, TEXT("17A")) == 0)
		return 13;
	else if (wcscmp(pSuffix, TEXT("17B")) == 0)
		return 14;
	else if (wcscmp(pSuffix, TEXT("18A")) == 0)
		return 15;
	else if (wcscmp(pSuffix, TEXT("20A")) == 0)
		return 16;
	else if (wcscmp(pSuffix, TEXT("21A")) == 0)
		return 17;
	else if (wcscmp(pSuffix, TEXT("22A")) == 0)
		return 18;
	else if (wcscmp(pSuffix, TEXT("23A")) == 0)
		return 19;
	else if (wcscmp(pSuffix, TEXT("24A")) == 0)
		return 20;
	else if (wcscmp(pSuffix, TEXT("35A")) == 0)
		return 21;
	else if (wcscmp(pSuffix, TEXT("39A")) == 0)
		return 22;
	else if (wcscmp(pSuffix, TEXT("39C")) == 0)
		return 23;
	else if (wcscmp(pSuffix, TEXT("39D")) == 0)
		return 24;
	else if (wcscmp(pSuffix, TEXT("43A")) == 0)
		return 25;
	else if (wcscmp(pSuffix, TEXT("44A")) == 0)
		return 26;
	else if (wcscmp(pSuffix, TEXT("46A")) == 0)
		return 27;
	else if (wcscmp(pSuffix, TEXT("50A")) == 0)
		return 28;
	else if (wcscmp(pSuffix, TEXT("52A")) == 0)
		return 29;
	else if (wcscmp(pSuffix, TEXT("55A")) == 0)
		return 30;
	else if (wcscmp(pSuffix, TEXT("58A")) == 0)
		return 31;
	else if (wcscmp(pSuffix, TEXT("59A")) == 0)
		return 32;
	else if (wcscmp(pSuffix, TEXT("60A")) == 0)
		return 33;
	else if (wcscmp(pSuffix, TEXT("61A")) == 0)
		return 34;
	else if (wcscmp(pSuffix, TEXT("65A")) == 0)
		return 35;
	else if (wcscmp(pSuffix, TEXT("66A")) == 0)
		return 36;
	else if (wcscmp(pSuffix, TEXT("67A")) == 0)
		return 37;
	else if (wcscmp(pSuffix, TEXT("69A")) == 0)
		return 38;
	else if (wcscmp(pSuffix, TEXT("71A")) == 0)
		return 39;
	else if (wcscmp(pSuffix, TEXT("80A")) == 0)
		return 40;
	else if (wcscmp(pSuffix, TEXT("81A")) == 0)
		return 41;
	else if (wcscmp(pSuffix, TEXT("93A")) == 0)
		return 42;
	else if (wcscmp(pSuffix, TEXT("95A")) == 0)
		return 43;
	else if (wcscmp(pSuffix, TEXT("96A")) == 0)
		return 44;
	else if (wcscmp(pSuffix, TEXT("97A")) == 0)
		return 45;
	else if (wcscmp(pSuffix, TEXT("98A")) == 0)
		return 46;
	else if (wcscmp(pSuffix, TEXT("100A")) == 0)
		return 47;
	else if (wcscmp(pSuffix, TEXT("101A")) == 0)
		return 48;
	else if (wcscmp(pSuffix, TEXT("103A")) == 0)
		return 49;
	else if (wcscmp(pSuffix, TEXT("104A")) == 0)
		return 50;
	else if (wcscmp(pSuffix, TEXT("105A")) == 0)
		return 51;
	else if (wcscmp(pSuffix, TEXT("106A")) == 0)
		return 52;
	else if (wcscmp(pSuffix, TEXT("108A")) == 0)
		return 53;
	else if (wcscmp(pSuffix, TEXT("109A")) == 0)
		return 54;
	else if (wcscmp(pSuffix, TEXT("110A")) == 0)
		return 55;
	else if (wcscmp(pSuffix, TEXT("111A")) == 0)
		return 56;
	else if (wcscmp(pSuffix, TEXT("112A")) == 0)
		return 57;
	else if (wcscmp(pSuffix, TEXT("113A")) == 0)
		return 58;
	else if (wcscmp(pSuffix, TEXT("115A")) == 0)
		return 59;
	else if (wcscmp(pSuffix, TEXT("116A")) == 0)
		return 60;
	else if (wcscmp(pSuffix, TEXT("117A")) == 0)
		return 61;
	else if (wcscmp(pSuffix, TEXT("121A")) == 0)
		return 62;
	else if (wcscmp(pSuffix, TEXT("122A")) == 0)
		return 63;
	else if (wcscmp(pSuffix, TEXT("123A")) == 0)
		return 64;
	else if (wcscmp(pSuffix, TEXT("125A")) == 0)
		return 65;
	else if (wcscmp(pSuffix, TEXT("126A")) == 0)
		return 66;
	else if (wcscmp(pSuffix, TEXT("127A")) == 0)
		return 67;
	else if (wcscmp(pSuffix, TEXT("127B")) == 0)
		return 68;
	else if (wcscmp(pSuffix, TEXT("127C")) == 0)
		return 69;
	else if (wcscmp(pSuffix, TEXT("127D")) == 0)
		return 70;
	else if (wcscmp(pSuffix, TEXT("127E")) == 0)
		return 71;
	else if (wcscmp(pSuffix, TEXT("128A")) == 0)
		return 72;
	else if (wcscmp(pSuffix, TEXT("128B")) == 0)
		return 73;
	else if (wcscmp(pSuffix, TEXT("128C")) == 0)
		return 74;
	else if (wcscmp(pSuffix, TEXT("131A")) == 0)
		return 75;
	else if (wcscmp(pSuffix, TEXT("132A")) == 0)
		return 76;
	else if (wcscmp(pSuffix, TEXT("132B")) == 0)
		return 77;
	else if (wcscmp(pSuffix, TEXT("132C")) == 0)
		return 78;
	else if (wcscmp(pSuffix, TEXT("133B")) == 0)
		return 79;


	return 0;
}


CCanyon* CCanyon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCanyon* pInstance = new CCanyon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CDesertObject* CCanyon::Clone(void* pArg)
{
	CCanyon* pInstance = new CCanyon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanyon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCanyon::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}

