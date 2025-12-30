#include "pch.h"
#include "Desert_Architecture.h"
#include "GameInstance.h"

CDesert_Architecture::CDesert_Architecture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDesertObject{ pDevice, pContext }
{
}

CDesert_Architecture::CDesert_Architecture(const CDesert_Architecture& Prototype)
	: CDesertObject{ Prototype }
{
}

HRESULT CDesert_Architecture::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDesert_Architecture::Initialize(void* pArg)
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

	return S_OK;
}

void CDesert_Architecture::Priority_Update(_float fTimeDelta)
{
}

void CDesert_Architecture::Update(_float fTimeDelta)
{
	SetCullingCollider(m_iObjectID);
	_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	m_pCullingCollider->UpdateColiision(worldMatrix);
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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

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
	case 1: // Ruin_7A (20, 30, 35)
		pCullingCollider->SetCollision({ -5.f, 21.f, -15.f }, {}, { 20.f, 30.f, 35.f });
		break;
	case 2: // Ruin_7B (30, 15, 25)
		pCullingCollider->SetCollision({ -20.f, 10.5f, -10.f }, {}, { 30.f, 15.f, 25.f });
		break;
	case 3: // Ruin_7C (12, 25, 12)
		pCullingCollider->SetCollision({ 0.f, 23.f, 0.f }, {}, { 12.f, 25.f, 12.f });
		break;
	case 4: // Ruin_7D (18, 8, 5)
		pCullingCollider->SetCollision({ 0.f, 4.f, 0.f }, {}, { 18.f, 8.f, 5.f });
		break;
	case 5: // Ruin_7E (4, 6, 4)
		pCullingCollider->SetCollision({ 0.f, 4.2f, 0.f }, {}, { 4.f, 6.f, 4.f });
		break;
	case 6: // Bridge_4A (10, 65, 30)
		pCullingCollider->SetCollision({ 0.f, 40.f, 0.f }, {}, { 10.f, 65.f, 30.f });
		break;
	case 7: // Bridge_4B (50, 65, 30)
		pCullingCollider->SetCollision({ 30.f, 45.5f, 0.f }, {}, { 40.f, 65.f, 30.f });
		break;
	case 8: // Bridge_4D (30, 80, 30)
		pCullingCollider->SetCollision({ 0.f, 45.f, 0.f }, {}, { 30.f, 80.f, 30.f });
		break;
	case 9: // Bridge_4L (40, 120, 150)
		pCullingCollider->SetCollision({ -10.f, -90.f, 100.f }, {}, { 40.f, 120.f, 150.f });
		break;
	case 10: // Bridge_4M (70, 70, 80)
		pCullingCollider->SetCollision({ 0.f, 30.f, -100.f }, {}, { 70.f, 80.f, 80.f });
		break;
	case 11: // Bridge_4N (30, 10, 5)
		pCullingCollider->SetCollision({ -35.f, 40.f, -70.f }, {}, { 30.f, 10.f, 5.f });
		break;
	case 12: // Bridge_5 (50, 60, 50)
		pCullingCollider->SetCollision({ 0.f, -10.f, 0.f }, {}, { 50.f, 60.f, 50.f });
		break;
	case 13: // Bridge_6 (60, 110, 100)
		pCullingCollider->SetCollision({ 0.f, 65.f, 0.f }, {}, { 60.f, 110.f, 100.f });
		break;
	case 14: // Bridge_14A (6, 8, 25)
		pCullingCollider->SetCollision({ 0.f, 5.6f, 0.f }, {}, { 6.f, 8.f, 25.f });
		break;
	case 15: // Bridge_14B (6, 5, 20)	
		pCullingCollider->SetCollision({ 0.f, 3.5f, 0.f }, {}, { 6.f, 5.f, 20.f });
		break;
	case 16: // Ruin_A
		pCullingCollider->SetCollision({ -3.f, 5.0f, 0.f }, {}, { 10.f, 6.f, 6.f }); 
		break;
	case 17: // Ruin_B
		pCullingCollider->SetCollision({ 0.f, 10.f, 0.f }, {}, { 7.f, 11.f, 7.f }); 
		break;
	case 18: // Ruin_C
		pCullingCollider->SetCollision({ 4.f, 5.0f, -1.f }, {}, { 6.f, 6.f, 6.f }); 
		break;
	case 19: // Ruin_D
		pCullingCollider->SetCollision({ -0.5f, 6.0f, 1.5f }, {}, { 8.f, 8.f, 8.f }); 
		break;
	case 20: // Ruin_Building_A
		pCullingCollider->SetCollision({ 0.f, 28.f, -1.5f }, {}, { 20.f, 33.f, 20.f }); 
		break;
	case 21: // Ruin_Building_B
		pCullingCollider->SetCollision({ 0.f, 21.f, 0.f }, {}, { 22.f, 22.f, 28.f }); 
		break;
	case 22: // Ruin_Building_C
		pCullingCollider->SetCollision({ 10.f, 16.0f, 5.f }, {}, { 30.f, 20.f, 35.f }); 
		break;
	case 23: // Floor_A
		pCullingCollider->SetCollision({ 0.f, 0.f, 0.f }, {}, { 6.f, 1.f, 6.f }); 
		break;
	case 24: // Floor_B
		pCullingCollider->SetCollision({ -3.f, 0.5f, 0.f }, {}, { 7.f, 5.f, 8.f }); 
		break;
	case 25: // Floor_C
		pCullingCollider->SetCollision({ 0.f, -0.2f, 0.f }, {}, { 5.f, 2.f, 3.f }); 
		break;
	case 26: // Floor_D
		pCullingCollider->SetCollision({ 0.f, 1.0f, 0.f }, {}, { 8.f, 2.f, 8.f }); 
		break;
	case 27: // Floor_E
		pCullingCollider->SetCollision({ 0.f, 0.5f, 0.f }, {}, { 7.f, 2.f, 7.f }); 
		break;
	case 28: // Floor_F
		pCullingCollider->SetCollision({ -1.f, 0.2f, 0.f }, {}, { 2.f, 1.f, 6.f }); 
		break;
	case 29: // Floor_G
		pCullingCollider->SetCollision({ 0.f, 0.2f, 0.f }, {}, { 7.f, 1.f, 7.f }); 
		break;
	case 30: // Floor_H
		pCullingCollider->SetCollision({ 0.f, 0.5f, 0.f }, {}, { 8.f, 2.f, 8.f }); 
		break;
	case 31: // Frame_A
		pCullingCollider->SetCollision({ 0.f, 9.f, -1.5f }, {}, { 1.5f, 16.f, 13.f }); 
		break;
	case 32: // Frame_B
		pCullingCollider->SetCollision({ 0.f, -1.f, 0.f }, {}, { 2.f, 8.f, 7.f }); 
		break;
	case 33: // Stone009_A
		pCullingCollider->SetCollision({ 0.f, -0.5f, -5.5f }, {}, { 1.f, 1.f, 6.f }); 
		break;
	case 34: // Stone009_B
		pCullingCollider->SetCollision({ 1.f, 0.5f, -2.7f }, {}, { 2.f, 1.f, 4.f }); 
		break;
	case 35: // Wall_A
		pCullingCollider->SetCollision({ 0.f, 6.f, 0.f }, {}, { 8.f, 7.f, 2.f }); 
		break;
	case 36: // Wall_B
		pCullingCollider->SetCollision({ -1.f, 6.f, 0.f }, {}, { 10.f, 7.f, 2.f }); 
		break;
	case 37: // Wall_C
		pCullingCollider->SetCollision({ 0.5f, 6.f, 0.f }, {}, { 7.f, 7.f, 2.f }); 
		break;
	case 38: // Wall_D
		pCullingCollider->SetCollision({ 0.f, 3.0f, 0.f }, {}, { 9.f, 4.f, 2.f }); 
		break;
	case 39: // Wall_E
		pCullingCollider->SetCollision({ 0.f, 3.0f, 0.f }, {}, { 10.f, 4.f, 2.f }); 
		break;
	case 40: // Wall_F
		pCullingCollider->SetCollision({ 1.f, 2.5f, 0.f }, {}, { 9.f, 4.f, 2.f }); 
		break;
	case 41: // Wall_G
		pCullingCollider->SetCollision({ -0.5f, 2.f, 0.f }, {}, { 10.f, 3.f, 2.f }); 
		break;
	case 42: // Wall_H
		pCullingCollider->SetCollision({ 3.f, -1.f, 0.f }, {}, { 11.f, 3.f, 2.f }); 
		break;
	case 43: // Wall_I
		pCullingCollider->SetCollision({ -3.f, -1.f, 0.f }, {}, { 7.f, 4.f, 2.f }); 
		break;
	case 44: // Wall007_A
		pCullingCollider->SetCollision({ 3.5f, 4.5f, 0.f }, {}, { 5.f, 6.f, 2.f }); 
		break;
	case 45: // Wall007_B
		pCullingCollider->SetCollision({ -0.5f, 3.5f, 0.f }, {}, { 8.f, 5.f, 2.f }); 
		break;
	case 46: // Wall007_C
		pCullingCollider->SetCollision({ 0.f, 5.0f, 0.f }, {}, { 5.f, 6.f, 2.f }); 
		break;
	case 47: // Wall007_D
		pCullingCollider->SetCollision({ 0.f, 5.0f, 0.f }, {}, { 8.f, 6.f, 2.f }); 
		break;
	case 48: // Wall007_E
		pCullingCollider->SetCollision({ -0.5f, 4.f, 0.f }, {}, { 8.f, 5.f, 2.f }); 
		break;
	case 49: // Wall007_F
		pCullingCollider->SetCollision({ -0.5f, 2.f, 0.f }, {}, { 8.f, 3.f, 2.f }); 
		break;
	case 50: // Wall007_G
		pCullingCollider->SetCollision({ -0.5f, 3.5f, 0.f }, {}, { 8.f, 5.f, 2.f }); 
		break;
	case 51: // Xion_Wall_1C (40, 20, 40)
		pCullingCollider->SetCollision({ -36.f, 19.0f, 37.f }, {}, { 40.f, 20.f, 40.f }); 
		break;
	case 52: // Xion_Wall_1D (40, 20, 40)
		pCullingCollider->SetCollision({ -36.f, 19.0f, -37.f }, {}, { 40.f, 20.f, 40.f }); 
		break;
	case 53: // Xion_Wall_1Db (40, 20, 40)
		pCullingCollider->SetCollision({ -36.f, 19.0f, -37.f }, {}, { 40.f, 20.f, 40.f }); 
		break;
	case 54: // Xion_Wall_1G (10, 43, 10)
		pCullingCollider->SetCollision({ 0.f, 42.f, 0.f }, {}, { 10.f, 43.f, 10.f }); 
		break;
	case 55: // Xion_Wall_1I (35, 60, 62)
		pCullingCollider->SetCollision({ -30.5f, 59.0f, -58.f }, {}, { 35.f, 60.f, 62.f }); 
		break;
	case 56: // Xion_Wall_1K (25, 60, 8)
		pCullingCollider->SetCollision({ 0.f, 59.0f, -5.5f }, {}, { 25.f, 60.f, 8.f }); 
		break;
	case 57: // Xion_Wall_1Q (35, 23, 35)
		pCullingCollider->SetCollision({ -20.f, -19.f, -25.f }, {}, { 35.f, 23.f, 35.f }); 
		break;
	case 58: // Xion_Wall_1R (20, 21, 12)
		pCullingCollider->SetCollision({ -15.f, -19.f, -5.f }, {}, { 20.f, 21.f, 12.f }); 
		break;
	case 59: // Xion_Wall_1S (10, 22, 17)
		pCullingCollider->SetCollision({ -14.f, -19.5f, -11.f }, {}, { 15.f, 21.f, 17.f }); 
		break;
	case 60: // Xion_Wall_1T (10, 22, 23)
		pCullingCollider->SetCollision({ 6.f, -21.f, -19.f }, {}, { 10.f, 22.f, 23.f }); 
		break;
	case 61: // Xion_Wall_2A (85, 36, 9)
		pCullingCollider->SetCollision({ -80.f, -34.0f, -5.f }, {}, { 84.f, 36.f, 9.f }); 
		break;
	case 62: // Xion_Wall_2B (85, 36, 9)
		pCullingCollider->SetCollision({ -80.f, -34.0f, -5.f }, {}, { 84.f, 36.f, 9.f }); 
		break;
	case 63: // Xion_Wall_4H (25, 35, 13)
		pCullingCollider->SetCollision({ 0.f, 33.f, -2.f }, {}, { 25.f, 35.f, 13.f }); 
		break;
	case 64: // Xion_Wall_5A (20, 43, 11)
		pCullingCollider->SetCollision({ 0.f, 41.5f, 0.f }, {}, { 20.f, 43.f, 11.f }); 
		break;
	case 65: // Xion_Wall_8A (32, 17, 4)
		pCullingCollider->SetCollision({ 30.f, 15.f, 1.f }, {}, { 32.f, 17.f, 4.f }); 
		break;
	case 66: // Xion_Wall_9A (25, 13, 3)
		pCullingCollider->SetCollision({ -22.f, 11.f, -1.f }, {}, { 25.f, 13.f, 3.f }); 
		break;
	case 67: // Xion_Wall_9B (10, 12, 2)
		pCullingCollider->SetCollision({ -8.f, 11.0f, -0.5f }, {}, { 10.f, 12.f, 2.f }); 
		break;
	case 68: // Xion_Wall_10A (6, 17, 32)
		pCullingCollider->SetCollision({ 0.f, 15.5f, 30.f }, {}, { 6.f, 17.f, 32.f }); 
		break;
	case 69: // Xion_Wall_10B (5, 17, 32)
		pCullingCollider->SetCollision({ 1.f, 15.5f, 30.f }, {}, { 5.f, 17.f, 32.f }); 
		break;
	case 70: // Xion_Wall_10C (1.5, 3, 7)
		pCullingCollider->SetCollision({ 0.f, 2.5f, 5.8f }, {}, { 1.5f, 3.f, 6.5f }); 
		break;
	case 71: // Xion_Wall_10D (6, 2.5, 4)
		pCullingCollider->SetCollision({ -5.f, 2.f, -3.f }, {}, { 6.f, 2.5f, 4.f }); 
		break;
	case 72: // Xion_Building_3A (19, 28, 13) 
		pCullingCollider->SetCollision({ 0.f, 26.0f, 0.f }, {}, { 19.f, 28.f, 13.f }); 
		break;
	case 73: // Xion_Building_4A (30, 42, 38)   TODO
		pCullingCollider->SetCollision({ 0.f, 40.0f, 0.f }, {}, { 30.f, 42.f, 38.f }); 
		break;
	case 74: // Xion_Building_5A (42, 44, 40)
		pCullingCollider->SetCollision({ 0.f, 42.0f, -20.f }, {}, { 42.f, 44.f, 40.f }); 
		break;
	case 75: // Xion_Building_6A (58, 70, 40)
		pCullingCollider->SetCollision({ 7.f, 68.0f, -10.f }, {}, { 58.f, 70.f, 40.f }); 
		break;
	case 76: // Xion_Building_8 (65, 70, 60)
		pCullingCollider->SetCollision({ 58.f, 35.0f, -40.f }, {}, { 65.f, 70.f, 60.f }); 
		break;
	case 77: // Xion_Building_9A (15, 7, 10)
		pCullingCollider->SetCollision({ 26.f, 5.f, 19.f }, {}, { 15.f, 7.f, 10.f }); 
		break;
	case 78: // Xion_Building_17 (80, 80, 85)
		pCullingCollider->SetCollision({ 0.f, -70.0f, 30.f }, {}, { 80.f, 80.f, 85.f }); 
		break;
	case 79: // Xion_Building4_1A (95, 80, 40)
		pCullingCollider->SetCollision({ -13.f, 15.0f, 0.f }, {}, { 95.f, 80.f, 40.f }); 
		break;
	case 80: // Xion_Building4_1B (88, 93, 40)
		pCullingCollider->SetCollision({ -20.f, 5.f, 6.f }, {}, { 88.f, 93.f, 40.f }); 
		break;
	case 81: // Xion_Building4_1C (120, 120, 60)
		pCullingCollider->SetCollision({ 0.f, 57.0f, 27.f }, {}, { 120.f, 120.f, 60.f }); 
		break;
	case 82: // Xion_Building4_1D (70, 86, 45)
		pCullingCollider->SetCollision({ 10.f, 73.0f, 10.f }, {}, { 70.f, 86.f, 45.f }); 
		break;
	case 83: // Xion_Building4_1E (60, 95, 50)
		pCullingCollider->SetCollision({ 3.f, 93.f, 10.f }, {}, { 60.f, 95.f, 55.f }); 
		break;
	case 84: // Xion_Building4_1F (60, 78, 40)
		pCullingCollider->SetCollision({ 10.f, 75.0f, 8.f }, {}, { 60.f, 78.f, 40.f }); 
		break;
	case 85: // Xion_Building5_4 (35, 45, 40)
		pCullingCollider->SetCollision({ 0.f, 43.f, 0.f }, {}, { 35.f, 46.f, 40.f }); 
		break;
	case 86: // Xion_Building5_callD4 (30, 42, 40)
		pCullingCollider->SetCollision({ 0.f, 40.0f, 0.f }, {}, { 30.f, 42.f, 40.f }); 
		break;
	case 87: // Xion_Building5_callD5 (40, 38, 40)
		pCullingCollider->SetCollision({ 0.f, 36.0f, -20.f }, {}, { 40.f, 38.f, 40.f }); 
		break;
	case 88: // Store_3 (10, 6, 10)
		pCullingCollider->SetCollision({ 8.f, 5.0f, 11.5f }, {}, { 10.f, 6.f, 13.f }); 
		break;
	case 89: // Store_4 (10, 6, 7)
		pCullingCollider->SetCollision({ 9.f, 5.0f, 6.f }, {}, { 10.f, 6.f, 7.f }); 
		break;
	case 90: // Stair_1A (14, 4, 8)
		pCullingCollider->SetCollision({ -14.f, 6.5f, 13.f }, {}, { 14.f, 4.f, 8.f }); 
		break;
	case 91: // Stair_1B (14, 4, 8)
		pCullingCollider->SetCollision({ -14.5f, 13.f, -18.f }, {}, { 14.f, 4.f, 8.f }); 
		break;
	case 92: // Stair_1C (31, 2, 17)
		pCullingCollider->SetCollision({ 0.f, 1.3f, 31.f }, {}, { 31.f, 2.f, 17.f }); 
		break;
	case 93: // Stair_2A (7, 2, 5)
		pCullingCollider->SetCollision({ 0.f, 1.0f, -2.f }, {}, { 7.f, 2.f, 5.f }); 
		break;
	case 94: // Stair_3A (5, 4, 5)
		pCullingCollider->SetCollision({ 0.f, 3.0f, -4.f }, {}, { 5.f, 4.f, 5.f }); 
		break;
	case 95: // Stair_3B (4.5, 1.5, 1.5)
		pCullingCollider->SetCollision({ 0.f, 0.75f, -1.f }, {}, { 4.5f, 1.5f, 1.5f }); 
		break;
	default:
		break;
	}

}

_uint CDesert_Architecture::Object_Number(const _tchar* pComponentTag)
{
	if (pComponentTag == nullptr)
		return 0;

	const _tchar* pLastUnderscore = wcsrchr(pComponentTag, L'_');
	if (pLastUnderscore == nullptr || *(pLastUnderscore + 1) == L'\0')
		return 0;

	const _tchar* pSuffix = pLastUnderscore + 1;

	// 1. Ruin_Building 시리즈 (20 ~ 22) - Ruin보다 먼저 체크해야 함
	if (wcsstr(pComponentTag, TEXT("Ruin_Building"))) {
		if (!wcscmp(pSuffix, TEXT("A"))) return 20;
		if (!wcscmp(pSuffix, TEXT("B"))) return 21;
		if (!wcscmp(pSuffix, TEXT("C"))) return 22;
	}
	// 2. 일반 Ruin 시리즈 (1 ~ 5, 16 ~ 19)
	else if (wcsstr(pComponentTag, TEXT("Ruin"))) {
		if (!wcscmp(pSuffix, TEXT("7A"))) return 1;
		if (!wcscmp(pSuffix, TEXT("7B"))) return 2;
		if (!wcscmp(pSuffix, TEXT("7C"))) return 3;
		if (!wcscmp(pSuffix, TEXT("7D"))) return 4;
		if (!wcscmp(pSuffix, TEXT("7E"))) return 5;
		if (!wcscmp(pSuffix, TEXT("A"))) return 16;
		if (!wcscmp(pSuffix, TEXT("B"))) return 17;
		if (!wcscmp(pSuffix, TEXT("C"))) return 18;
		if (!wcscmp(pSuffix, TEXT("D"))) return 19;
	}
	// 3. Bridge 시리즈 (6 ~ 15)
	else if (wcsstr(pComponentTag, TEXT("Bridge"))) {
		if (!wcscmp(pSuffix, TEXT("4A"))) return 6;
		if (!wcscmp(pSuffix, TEXT("4B"))) return 7;
		if (!wcscmp(pSuffix, TEXT("4D"))) return 8;
		if (!wcscmp(pSuffix, TEXT("4L"))) return 9;
		if (!wcscmp(pSuffix, TEXT("4M"))) return 10;
		if (!wcscmp(pSuffix, TEXT("4N"))) return 11;
		if (!wcscmp(pSuffix, TEXT("5")))  return 12;
		if (!wcscmp(pSuffix, TEXT("6")))  return 13;
		if (!wcscmp(pSuffix, TEXT("14A"))) return 14;
		if (!wcscmp(pSuffix, TEXT("14B"))) return 15;
	}
	// 4. Floor 시리즈 (23 ~ 30)
	else if (wcsstr(pComponentTag, TEXT("Floor"))) {
		if (!wcscmp(pSuffix, TEXT("A"))) return 23;
		if (!wcscmp(pSuffix, TEXT("B"))) return 24;
		if (!wcscmp(pSuffix, TEXT("C"))) return 25;
		if (!wcscmp(pSuffix, TEXT("D"))) return 26;
		if (!wcscmp(pSuffix, TEXT("E"))) return 27;
		if (!wcscmp(pSuffix, TEXT("F"))) return 28;
		if (!wcscmp(pSuffix, TEXT("G"))) return 29;
		if (!wcscmp(pSuffix, TEXT("H"))) return 30;
	}
	// 5. Frame 시리즈 (31 ~ 32)
	else if (wcsstr(pComponentTag, TEXT("Frame"))) {
		if (!wcscmp(pSuffix, TEXT("A"))) return 31;
		if (!wcscmp(pSuffix, TEXT("B"))) return 32;
	}
	// 6. Stone 시리즈 (33 ~ 34)
	else if (wcsstr(pComponentTag, TEXT("Stone009"))) {
		if (!wcscmp(pSuffix, TEXT("A"))) return 33;
		if (!wcscmp(pSuffix, TEXT("B"))) return 34;
	}
	// Xion_Wall (51 ~ 71)
	else if (wcsstr(pComponentTag, TEXT("Xion_Wall"))) {
		if (!wcscmp(pSuffix, TEXT("1C"))) return 51;
		if (!wcscmp(pSuffix, TEXT("1D"))) return 52;
		if (!wcscmp(pSuffix, TEXT("1Db"))) return 53;
		if (!wcscmp(pSuffix, TEXT("1G"))) return 54;
		if (!wcscmp(pSuffix, TEXT("1I"))) return 55;
		if (!wcscmp(pSuffix, TEXT("1K"))) return 56;
		if (!wcscmp(pSuffix, TEXT("1Q"))) return 57;
		if (!wcscmp(pSuffix, TEXT("1R"))) return 58;
		if (!wcscmp(pSuffix, TEXT("1S"))) return 59;
		if (!wcscmp(pSuffix, TEXT("1T"))) return 60;
		if (!wcscmp(pSuffix, TEXT("2A"))) return 61;
		if (!wcscmp(pSuffix, TEXT("2B"))) return 62;
		if (!wcscmp(pSuffix, TEXT("4H"))) return 63;
		if (!wcscmp(pSuffix, TEXT("5A"))) return 64;
		if (!wcscmp(pSuffix, TEXT("8A"))) return 65;
		if (!wcscmp(pSuffix, TEXT("9A"))) return 66;
		if (!wcscmp(pSuffix, TEXT("9B"))) return 67;
		if (!wcscmp(pSuffix, TEXT("10A"))) return 68;
		if (!wcscmp(pSuffix, TEXT("10B"))) return 69;
		if (!wcscmp(pSuffix, TEXT("10C"))) return 70;
		if (!wcscmp(pSuffix, TEXT("10D"))) return 71;
	}
	// 7. Wall007 시리즈 (44 ~ 50) - Wall보다 먼저 체크
	else if (wcsstr(pComponentTag, TEXT("Wall007"))) {
		if (!wcscmp(pSuffix, TEXT("A"))) return 44;
		if (!wcscmp(pSuffix, TEXT("B"))) return 45;
		if (!wcscmp(pSuffix, TEXT("C"))) return 46;
		if (!wcscmp(pSuffix, TEXT("D"))) return 47;
		if (!wcscmp(pSuffix, TEXT("E"))) return 48;
		if (!wcscmp(pSuffix, TEXT("F"))) return 49;
		if (!wcscmp(pSuffix, TEXT("G"))) return 50;
	}
	// 8. 일반 Wall 시리즈 (35 ~ 43)
	else if (wcsstr(pComponentTag, TEXT("Wall"))) {
		if (!wcscmp(pSuffix, TEXT("A"))) return 35;
		if (!wcscmp(pSuffix, TEXT("B"))) return 36;
		if (!wcscmp(pSuffix, TEXT("C"))) return 37;
		if (!wcscmp(pSuffix, TEXT("D"))) return 38;
		if (!wcscmp(pSuffix, TEXT("E"))) return 39;
		if (!wcscmp(pSuffix, TEXT("F"))) return 40;
		if (!wcscmp(pSuffix, TEXT("G"))) return 41;
		if (!wcscmp(pSuffix, TEXT("H"))) return 42;
		if (!wcscmp(pSuffix, TEXT("I"))) return 43;
	}
	else if (wcsstr(pComponentTag, TEXT("Xion_Building4"))) {
		if (!wcscmp(pSuffix, TEXT("1A"))) return 79;
		if (!wcscmp(pSuffix, TEXT("1B"))) return 80;
		if (!wcscmp(pSuffix, TEXT("1C"))) return 81;
		if (!wcscmp(pSuffix, TEXT("1D"))) return 82;
		if (!wcscmp(pSuffix, TEXT("1E"))) return 83;
		if (!wcscmp(pSuffix, TEXT("1F"))) return 84;
	}
	else if (wcsstr(pComponentTag, TEXT("Xion_Building5"))) {
		if (!wcscmp(pSuffix, TEXT("4"))) return 85;
		if (!wcscmp(pSuffix, TEXT("callD4"))) return 86;
		if (!wcscmp(pSuffix, TEXT("callD5"))) return 87;
	}
	// Xion_Building 시리즈 (72 ~ 87)
	else if (wcsstr(pComponentTag, TEXT("Xion_Building"))) {
		if (!wcscmp(pSuffix, TEXT("3A"))) return 72;
		if (!wcscmp(pSuffix, TEXT("4A"))) return 73;
		if (!wcscmp(pSuffix, TEXT("5A"))) return 74;
		if (!wcscmp(pSuffix, TEXT("6A"))) return 75;
		if (!wcscmp(pSuffix, TEXT("8")))  return 76;
		if (!wcscmp(pSuffix, TEXT("9A"))) return 77;
		if (!wcscmp(pSuffix, TEXT("17"))) return 78;
	}
	// Store 시리즈 (88 ~ 89)
	else if (wcsstr(pComponentTag, TEXT("Store"))) {
		if (!wcscmp(pSuffix, TEXT("3"))) return 88;
		if (!wcscmp(pSuffix, TEXT("4"))) return 89;
	}
	// 12. Stair 시리즈 (90 ~ 95)
	else if (wcsstr(pComponentTag, TEXT("Stair"))) {
		if (!wcscmp(pSuffix, TEXT("1A"))) return 90;
		if (!wcscmp(pSuffix, TEXT("1B"))) return 91;
		if (!wcscmp(pSuffix, TEXT("1C"))) return 92;
		if (!wcscmp(pSuffix, TEXT("2A"))) return 93;
		if (!wcscmp(pSuffix, TEXT("3A"))) return 94;
		if (!wcscmp(pSuffix, TEXT("3B"))) return 95;
	}

	return 0;
}

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

CDesertObject* CDesert_Architecture::Clone(void* pArg)
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