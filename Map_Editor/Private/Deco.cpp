#include "pch.h"
#include "Deco.h"
#include "GameInstance.h"

CDeco::CDeco(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDesertObject{ pDevice, pContext }
{
}

CDeco::CDeco(const CDeco& Prototype)
	: CDesertObject{ Prototype }
{
}

HRESULT CDeco::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDeco::Initialize(void* pArg)
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
    //_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    //m_pCullingCollider->UpdateColiision(worldMatrix);
    
	return S_OK;
}

void CDeco::Priority_Update(_float fTimeDelta)
{
}

void CDeco::Update(_float fTimeDelta)
{
    SetCullingCollider(m_iObjectID);
    _matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    m_pCullingCollider->UpdateColiision(worldMatrix);
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
    //m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
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

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CDeco::Ready_Components(const _tchar* pComponentTag)
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

HRESULT CDeco::Bind_ShaderResources()
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
    case 139: // Camp_1I
        pCullingCollider->SetCollision({ 0.f, 1.f, 0.f }, {}, { 1.f, 1.f, 1.f });
        break;
    }
}

_uint CDeco::Object_Number(const _tchar* pComponentTag)
{
    if (pComponentTag == nullptr)
        return 0;

    const _tchar* pLastUnderscore = wcsrchr(pComponentTag, L'_');
    if (pLastUnderscore == nullptr || *(pLastUnderscore + 1) == L'\0')
        return 0;

    const _tchar* pSuffix = pLastUnderscore + 1;

    // 0. Box 시리즈 (1 ~ 21)
    if (wcsstr(pComponentTag, TEXT("Box"))) {
        if (!wcscmp(pSuffix, TEXT("1A"))) return 1;
        if (!wcscmp(pSuffix, TEXT("1B"))) return 2;
        if (!wcscmp(pSuffix, TEXT("2C"))) return 3;
        if (!wcscmp(pSuffix, TEXT("4A"))) return 4;
        if (!wcscmp(pSuffix, TEXT("5A"))) return 5;
        if (!wcscmp(pSuffix, TEXT("6A"))) return 6;
        if (!wcscmp(pSuffix, TEXT("11A"))) return 7;
        if (!wcscmp(pSuffix, TEXT("13A"))) return 8;
        if (!wcscmp(pSuffix, TEXT("14A"))) return 9;
        if (!wcscmp(pSuffix, TEXT("16A"))) return 10;
        if (!wcscmp(pSuffix, TEXT("16B"))) return 11;
        if (!wcscmp(pSuffix, TEXT("19A"))) return 12;
        if (!wcscmp(pSuffix, TEXT("20A"))) return 13;
        if (!wcscmp(pSuffix, TEXT("20B"))) return 14;
        if (!wcscmp(pSuffix, TEXT("20C"))) return 15;
        if (!wcscmp(pSuffix, TEXT("20D"))) return 16;
        if (!wcscmp(pSuffix, TEXT("20E"))) return 17;
        if (!wcscmp(pSuffix, TEXT("20F"))) return 18;
        if (!wcscmp(pSuffix, TEXT("21A"))) return 19;
        if (!wcscmp(pSuffix, TEXT("21B"))) return 20;
        if (!wcscmp(pSuffix, TEXT("26A"))) return 21;
    }
    // Garden 시리즈 (22 ~ 24)
    else if (wcsstr(pComponentTag, TEXT("Garden"))) {
        if (!wcscmp(pSuffix, TEXT("1A"))) return 22;
        if (!wcscmp(pSuffix, TEXT("1B"))) return 23;
        if (!wcscmp(pSuffix, TEXT("1C"))) return 24;
    }
    // Restroom 시리즈 (25)
    else if (wcsstr(pComponentTag, TEXT("Restroom"))) {
        if (!wcscmp(pSuffix, TEXT("4A"))) return 25;
    }
    // Poster 시리즈 (26 ~ 34)
    else if (wcsstr(pComponentTag, TEXT("Poster"))) {
        if (!wcscmp(pSuffix, TEXT("1A"))) return 26;
        if (!wcscmp(pSuffix, TEXT("2A"))) return 27;
        if (!wcscmp(pSuffix, TEXT("2B"))) return 28;
        if (!wcscmp(pSuffix, TEXT("3A"))) return 29;
        if (!wcscmp(pSuffix, TEXT("3B"))) return 30;
        if (!wcscmp(pSuffix, TEXT("4A"))) return 31;
        if (!wcscmp(pSuffix, TEXT("4B"))) return 32;
        if (!wcscmp(pSuffix, TEXT("4C"))) return 33;
        if (!wcscmp(pSuffix, TEXT("4E"))) return 34;
    }
    // 4. Duct 시리즈 (35 ~ 55)
    else if (wcsstr(pComponentTag, TEXT("Duct"))) {
        if (!wcscmp(pSuffix, TEXT("1A"))) return 35;
        if (!wcscmp(pSuffix, TEXT("1B"))) return 36;
        if (!wcscmp(pSuffix, TEXT("1C"))) return 37;
        if (!wcscmp(pSuffix, TEXT("1D"))) return 38;
        if (!wcscmp(pSuffix, TEXT("1E"))) return 39;
        if (!wcscmp(pSuffix, TEXT("1G"))) return 40;
        if (!wcscmp(pSuffix, TEXT("1H"))) return 41;
        if (!wcscmp(pSuffix, TEXT("3A"))) return 42;
        if (!wcscmp(pSuffix, TEXT("3B"))) return 43;
        if (!wcscmp(pSuffix, TEXT("3C"))) return 44;
        if (!wcscmp(pSuffix, TEXT("4A"))) return 45;
        if (!wcscmp(pSuffix, TEXT("4B"))) return 46;
        if (!wcscmp(pSuffix, TEXT("4C"))) return 47;
        if (!wcscmp(pSuffix, TEXT("6A"))) return 48;
        if (!wcscmp(pSuffix, TEXT("8A"))) return 49;
        if (!wcscmp(pSuffix, TEXT("9A"))) return 50;
        if (!wcscmp(pSuffix, TEXT("9B"))) return 51;
        if (!wcscmp(pSuffix, TEXT("9C"))) return 52;
        if (!wcscmp(pSuffix, TEXT("9D"))) return 53;
        if (!wcscmp(pSuffix, TEXT("10A"))) return 54;
        if (!wcscmp(pSuffix, TEXT("13A"))) return 55;
    }
    // 5. Statue 시리즈 (56 ~ 60)
    else if (wcsstr(pComponentTag, TEXT("Statue"))) {
        if (!wcscmp(pSuffix, TEXT("1A")))  return 56;
        if (!wcscmp(pSuffix, TEXT("19B"))) return 57;
        if (!wcscmp(pSuffix, TEXT("24B"))) return 58;
        if (!wcscmp(pSuffix, TEXT("31B"))) return 59;
        if (!wcscmp(pSuffix, TEXT("40B"))) return 60;
    }
    // 6. Furniture 시리즈 (61 ~ 75)
    else if (wcsstr(pComponentTag, TEXT("Furniture"))) {
        if (!wcscmp(pSuffix, TEXT("7A")))  return 61;
        if (!wcscmp(pSuffix, TEXT("9A")))  return 62;
        if (!wcscmp(pSuffix, TEXT("47A"))) return 63;
        if (!wcscmp(pSuffix, TEXT("50A"))) return 64;
        if (!wcscmp(pSuffix, TEXT("50B"))) return 65;
        if (!wcscmp(pSuffix, TEXT("50C"))) return 66;
        if (!wcscmp(pSuffix, TEXT("50D"))) return 67;
        if (!wcscmp(pSuffix, TEXT("51A"))) return 68;
        if (!wcscmp(pSuffix, TEXT("57A"))) return 69;
        if (!wcscmp(pSuffix, TEXT("59A"))) return 70;
        if (!wcscmp(pSuffix, TEXT("77A"))) return 71;
        if (!wcscmp(pSuffix, TEXT("79A"))) return 72;
        if (!wcscmp(pSuffix, TEXT("83A"))) return 73;
        if (!wcscmp(pSuffix, TEXT("87B"))) return 74;
        if (!wcscmp(pSuffix, TEXT("87D"))) return 75;
    }

    // 1. Lamp, Poster, Wheel, Base
    else if (wcsstr(pComponentTag, TEXT("Lamp"))) {
        if (!wcscmp(pSuffix, TEXT("47A"))) return 80;
    }
    else if (wcsstr(pComponentTag, TEXT("Poster"))) {
        if (!wcscmp(pSuffix, TEXT("4D"))) return 81;
    }
    else if (wcsstr(pComponentTag, TEXT("Wheel"))) {
        if (!wcscmp(pSuffix, TEXT("1A"))) return 82;
        if (!wcscmp(pSuffix, TEXT("1B"))) return 83;
        if (!wcscmp(pSuffix, TEXT("1C"))) return 84;
    }
    else if (wcsstr(pComponentTag, TEXT("Base"))) {
        if (!wcscmp(pSuffix, TEXT("1A"))) return 85;
    }

    // 2. Camp 시리즈 (86 ~ 98)
    else if (wcsstr(pComponentTag, TEXT("Camp"))) {
        if (!wcscmp(pSuffix, TEXT("1B"))) return 86;
        if (!wcscmp(pSuffix, TEXT("1D"))) return 87;
        if (!wcscmp(pSuffix, TEXT("1E"))) return 88;
        if (!wcscmp(pSuffix, TEXT("1F"))) return 89;
        if (!wcscmp(pSuffix, TEXT("1G"))) return 90;
        if (!wcscmp(pSuffix, TEXT("1H"))) return 91;
        if (!wcscmp(pSuffix, TEXT("1J"))) return 92;
        if (!wcscmp(pSuffix, TEXT("1K"))) return 93;
        if (!wcscmp(pSuffix, TEXT("1L"))) return 94;
        if (!wcscmp(pSuffix, TEXT("1N"))) return 95;
        if (!wcscmp(pSuffix, TEXT("1R"))) return 96;
        if (!wcscmp(pSuffix, TEXT("1S"))) return 97;
        if (!wcscmp(pSuffix, TEXT("1T"))) return 98;
        if (!wcscmp(pSuffix, TEXT("1I"))) return 139;
    }

    // 3. Container 시리즈 (99 ~ 111)
    else if (wcsstr(pComponentTag, TEXT("Container"))) {
        if (!wcscmp(pSuffix, TEXT("2A"))) return 99;
        if (!wcscmp(pSuffix, TEXT("2B"))) return 100;
        if (!wcscmp(pSuffix, TEXT("2C"))) return 101;
        if (!wcscmp(pSuffix, TEXT("2D"))) return 102;
        if (!wcscmp(pSuffix, TEXT("3A"))) return 103;
        if (!wcscmp(pSuffix, TEXT("4B"))) return 104;
        if (!wcscmp(pSuffix, TEXT("5A"))) return 105;
        if (!wcscmp(pSuffix, TEXT("5B"))) return 106;
        if (!wcscmp(pSuffix, TEXT("5C"))) return 107;
        if (!wcscmp(pSuffix, TEXT("5D"))) return 108;
        if (!wcscmp(pSuffix, TEXT("5E"))) return 109;
        if (!wcscmp(pSuffix, TEXT("7B"))) return 110;
        if (!wcscmp(pSuffix, TEXT("7F"))) return 111;
    }

    // 4. Fence 시리즈 (112 ~ 115)
    else if (wcsstr(pComponentTag, TEXT("Fence"))) {
        if (!wcscmp(pSuffix, TEXT("1A"))) return 112;
        if (!wcscmp(pSuffix, TEXT("1B"))) return 113;
        if (!wcscmp(pSuffix, TEXT("1F"))) return 114;
        if (!wcscmp(pSuffix, TEXT("1H"))) return 115;
    }

    // 5. Vehicle 시리즈 (116 ~ 123)
    else if (wcsstr(pComponentTag, TEXT("Vehicle"))) {
        if (!wcscmp(pSuffix, TEXT("2A"))) return 116;
        if (!wcscmp(pSuffix, TEXT("2B"))) return 117;
        if (!wcscmp(pSuffix, TEXT("3B"))) return 118;
        if (!wcscmp(pSuffix, TEXT("4C"))) return 119;
        if (!wcscmp(pSuffix, TEXT("6A"))) return 120;
        if (!wcscmp(pSuffix, TEXT("8B"))) return 121;
        if (!wcscmp(pSuffix, TEXT("14A"))) return 122;
        if (!wcscmp(pSuffix, TEXT("14B"))) return 123;
    }

    // 6. Sign 시리즈 (124 ~ 129)
    else if (wcsstr(pComponentTag, TEXT("Sign"))) {
        if (!wcscmp(pSuffix, TEXT("11A"))) return 124;
        if (!wcscmp(pSuffix, TEXT("11B"))) return 125;
        if (!wcscmp(pSuffix, TEXT("11F"))) return 126;
        if (!wcscmp(pSuffix, TEXT("12B"))) return 127;
        if (!wcscmp(pSuffix, TEXT("12C"))) return 128;
        if (!wcscmp(pSuffix, TEXT("36B"))) return 129;
    }

    // 7. Crane 시리즈 (130 ~ 132)
    else if (wcsstr(pComponentTag, TEXT("Crane"))) {
        if (!wcscmp(pSuffix, TEXT("1C"))) return 130;
        if (!wcscmp(pSuffix, TEXT("11"))) return 131;
        if (!wcscmp(pSuffix, TEXT("13"))) return 132;
    }

    // 8. Trash 시리즈 (133 ~ 138)
    else if (wcsstr(pComponentTag, TEXT("Trash"))) {
        if (!wcscmp(pSuffix, TEXT("1A"))) return 133;
        if (!wcscmp(pSuffix, TEXT("2A"))) return 134;
        if (!wcscmp(pSuffix, TEXT("2B"))) return 135;
        if (!wcscmp(pSuffix, TEXT("4A"))) return 136;
        if (!wcscmp(pSuffix, TEXT("9A"))) return 137;
        if (!wcscmp(pSuffix, TEXT("17A"))) return 138;
    }

    return 0;
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

CDesertObject* CDeco::Clone(void* pArg)
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
