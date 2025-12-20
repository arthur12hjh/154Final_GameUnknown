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

    //SetCullingCollider(m_iObjectID);
    //_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    //m_pCullingCollider->UpdateColiision(worldMatrix);

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
    SetCullingCollider(m_iObjectID);
    _matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    m_pCullingCollider->UpdateColiision(worldMatrix);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

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

_uint CDeco::Object_Number(const _tchar* pComponentTag)
{
    if (pComponentTag == nullptr)
        return 0;

    const _tchar* pLastUnderscore = wcsrchr(pComponentTag, L'_');
    if (pLastUnderscore == nullptr || *(pLastUnderscore + 1) == L'\0')
        return 0;

    const _tchar* pSuffix = pLastUnderscore + 1;

    // 1. Lamp, Poster, Wheel, Base
    if (wcsstr(pComponentTag, TEXT("Lamp"))) {
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
