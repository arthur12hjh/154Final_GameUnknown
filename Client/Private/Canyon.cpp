#include "pch.h"
#include "Canyon.h"
#include "GameInstance.h"

CCanyon::CCanyon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CCanyon::CCanyon(const CCanyon& Prototype)
	: CActor{ Prototype }
{
}

HRESULT CCanyon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCanyon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ACTOR_DESC* pDesc = static_cast<ACTOR_DESC*>(pArg);
	if (FAILED(Ready_Components(pDesc->szVIBuffer_PrototypeName)))
		return E_FAIL;

	SetCullingCollider(pDesc->iObjectID);

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
	if(m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
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
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_ORMTexture", aiTextureType_METALNESS, 0)))
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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CCanyon::Bind_ShaderResources()
{
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
        pCullingCollider->SetCollision({}, {}, { 3, 3, 32 });
        break;
    case 2: case 9: case 10: case 13: case 14: case 16:
    case 30: case 62: case 63:
        pCullingCollider->SetCollision({}, {}, { 3, 2, 2 });
        break;
    case 3:
        pCullingCollider->SetCollision({}, {}, { 13, 13, 3 });
        break;
    case 4: case 8: case 11: case 15: case 25: 
    case 28: case 31: case 32: case 64:
        pCullingCollider->SetCollision({}, {}, { 2, 2, 2 });
        break;
    case 5:
        pCullingCollider->SetCollision({}, {}, { 10, 5, 4 });
        break;
    case 6: case 44:
        pCullingCollider->SetCollision({}, {}, { 4, 2, 1 });
        break;
    case 7:
        pCullingCollider->SetCollision({}, {}, { 18, 65, 18 });
        break;
    case 12:
        pCullingCollider->SetCollision({}, {}, { 4, 3, 4 });
        break;
    case 17:
        pCullingCollider->SetCollision({}, {}, { 15, 7, 7 });
        break;
    case 18:
        pCullingCollider->SetCollision({}, {}, { 11, 14, 11 });
        break;
    case 19:
        pCullingCollider->SetCollision({}, {}, { 13, 13, 5 });
        break;
    case 20:
        pCullingCollider->SetCollision({}, {}, { 14, 5, 4 });
        break;
    case 21:
        pCullingCollider->SetCollision({}, {}, { 18, 4, 8 });
        break;
    case 22:
        pCullingCollider->SetCollision({}, {}, { 12, 35, 9 });
        break;
    case 23:
        pCullingCollider->SetCollision({}, {}, { 22, 25, 10 });
        break;
    case 24:
        pCullingCollider->SetCollision({}, {}, { 35, 23, 10 });
        break;
    case 26: case 27:
        pCullingCollider->SetCollision({}, {}, { 4, 1, 4 });
        break;
    case 29:
        pCullingCollider->SetCollision({}, {}, { 3, 2, 3 });
        break;
    case 33:
        pCullingCollider->SetCollision({}, {}, { 4, 2, 3 });
        break;
    case 34:
        pCullingCollider->SetCollision({}, {}, { 5, 4, 5 });
        break;
    case 35: case 47:
        pCullingCollider->SetCollision({}, {}, { 55, 20, 30 });
        break;
    case 36:
        pCullingCollider->SetCollision({}, {}, { 62, 18, 30 });
        break;
    case 37:
        pCullingCollider->SetCollision({}, {}, { 58, 31, 15 });
        break;
    case 38:
        pCullingCollider->SetCollision({}, {}, { 15, 18, 8 });
        break;
    case 39:
        pCullingCollider->SetCollision({}, {}, { 50, 34, 40 });
        break;
    case 40:
        pCullingCollider->SetCollision({}, {}, { 15, 8, 10 });
        break;
    case 41:
        pCullingCollider->SetCollision({}, {}, { 25, 40, 40 });
        break;
    case 42:
        pCullingCollider->SetCollision({}, {}, { 2, 2, 1 });
        break;
    case 43:
        pCullingCollider->SetCollision({}, {}, { 5, 3, 2 });
        break;
    case 45:
        pCullingCollider->SetCollision({}, {}, { 60, 18, 25 });
        break;
    case 46:
        pCullingCollider->SetCollision({}, {}, { 18, 5, 23 });
        break;
    case 48:
        pCullingCollider->SetCollision({}, {}, { 15, 15, 10 });
        break;
    case 49:
        pCullingCollider->SetCollision({}, {}, { 20, 20, 13 });
        break;
    case 50:
        pCullingCollider->SetCollision({}, {}, { 30, 15, 18 });
        break;
    case 51:
        pCullingCollider->SetCollision({}, {}, { 35, 25, 15 });
        break;
    case 52:
        pCullingCollider->SetCollision({}, {}, { 28, 28, 17 });
        break;
    case 53:
        pCullingCollider->SetCollision({}, {}, { 105, 12, 45 });
        break;
    case 54:
        pCullingCollider->SetCollision({}, {}, { 20, 8, 10 });
        break;
    case 55:
        pCullingCollider->SetCollision({}, {}, { 28, 6, 10 });
        break;
    case 56:
        pCullingCollider->SetCollision({}, {}, { 25, 2, 7 });
        break;
    case 57:
        pCullingCollider->SetCollision({}, {}, { 22, 4, 7 });
        break;
    case 58:
        pCullingCollider->SetCollision({}, {}, { 25, 4, 10 });
        break;
    case 59:
        pCullingCollider->SetCollision({}, {}, { 15, 3, 8 });
        break;
    case 60:
        pCullingCollider->SetCollision({}, {}, { 4, 1, 1 });
        break;
    case 61:
        pCullingCollider->SetCollision({}, {}, { 3, 1, 1 });
        break;
    case 65:
        pCullingCollider->SetCollision({}, {}, { 8, 12, 8 });
        break;
    case 66:
        pCullingCollider->SetCollision({}, {}, { 15, 13, 6 });
        break;
    case 67: case 68: case 69: case 70: case 71: case 72:
    case 73: case 74: case 76: case 77: case 78: case 79:
        pCullingCollider->SetCollision({}, {}, { 1000, 500, 1000 });
        break;
    case 75:
        pCullingCollider->SetCollision({}, {}, { 6, 1, 6 });
        break;
    default:
        // 기본 동작 또는 오류 처리 (필요시 추가)
        break;
    }
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

CGameObject* CCanyon::Clone(void* pArg)
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
