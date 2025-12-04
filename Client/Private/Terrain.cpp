#include "pch.h"
#include "Terrain.h"
#include "GameInstance.h"



CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CTerrain::CTerrain(const CTerrain& Prototype) 
	: CGameObject { Prototype }
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{		
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pGameInstance->Add_Terrain_ToPhysx(m_pVIBufferCom);

	return S_OK;
}

void CTerrain::Priority_Update(_float fTimeDelta)
{
	m_pNavigationCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CTerrain::Update(_float fTimeDelta)
{
	//m_pVIBufferCom->Culling(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));	
}

void CTerrain::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);	
#endif
}

HRESULT CTerrain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

_float CTerrain::Get_Height_In_World_Space(_float fWorldX, _float fWorldZ)
{
	// 1. World -> Local 변환 (Terrain은 보통 World 행렬이 Identity이므로 무시될 수 있으나, 정석대로 처리)
	_matrix WorldMatrix = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(m_pTransformCom->Get_WorldMatrixPtr()));
	_matrix WorldMatrixInverse = XMMatrixInverse(nullptr, WorldMatrix);
	_vector vWorldPos = XMVectorSet(fWorldX, 0.f, fWorldZ, 1.f);
	_vector vLocalPos = XMVector3TransformCoord(vWorldPos, WorldMatrixInverse);

	_float fLocalX = XMVectorGetX(vLocalPos);
	_float fLocalZ = XMVectorGetZ(vLocalPos);

	_float fLocalY = m_pVIBufferCom->Get_Interpolated_Height_Local(fLocalX, fLocalZ);

	_vector vFinalLocalPos = XMVectorSet(fLocalX, fLocalY, fLocalZ, 1.f);
	_vector vFinalWorldPos = XMVector3TransformCoord(vFinalLocalPos, WorldMatrix);

	return XMVectorGetY(vFinalLocalPos);
}


HRESULT CTerrain::Ready_Components()
{
	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::SCARLET), TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::SCARLET), TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_ORM */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::SCARLET), TEXT("Prototype_Component_Texture_Terrain_ORM"),
		TEXT("Com_ORM"), reinterpret_cast<CComponent**>(&m_pORMTextureCom))))
		return E_FAIL;

	/* Com_Mask */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::SCARLET), TEXT("Prototype_Component_Texture_Reed_Mask"),
		TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pMaskCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::SCARLET), TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Navigation */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::SCARLET), TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	
	//테스트 코드. 나중에 지우셔도 돼요
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_NUMPAD7))
		m_isORM = !m_isORM;

	if (true == m_isORM)
	{
		if (FAILED(m_pORMTextureCom->Bind_ShaderResources(m_pShaderCom, "g_ORMTexture")))
			return E_FAIL;
	}


	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_M))
	{
		if (FAILED(m_pMaskCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
			return E_FAIL;
	}
	

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMaskCom);
	Safe_Release(m_pORMTextureCom);
}
