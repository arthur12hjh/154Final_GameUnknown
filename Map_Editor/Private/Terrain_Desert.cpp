#include "pch.h"
#include "Terrain_Desert.h"
#include "GameInstance.h"
#include "MapTool_Desert.h"

CTerrain_Desert::CTerrain_Desert(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTerrain_Desert::CTerrain_Desert(const CTerrain_Desert& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTerrain_Desert::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain_Desert::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	return S_OK;
}

void CTerrain_Desert::Priority_Update(_float fTimeDelta)
{
	m_pNavigationCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CTerrain_Desert::Update(_float fTimeDelta)
{
	//m_pVIBufferCom->Culling(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CTerrain_Desert::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif
}

HRESULT CTerrain_Desert::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CTerrain_Desert::Change_Height_Rect(_vector vPickingPos, _float fHeight, _float fRadius)
{
	if (nullptr == m_pVIBufferCom)
		return;

	const _float4x4* pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	_matrix WorldMatrix = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(pWorldMatrix));

	_vector vDeterminant = XMMatrixDeterminant(WorldMatrix);
	_matrix WorldMatrixInverse = XMMatrixInverse(&vDeterminant, WorldMatrix);


	_vector vLocalPickedPos = XMVector3TransformCoord(vPickingPos, WorldMatrixInverse);
	m_pVIBufferCom->Change_Height_Rect(vLocalPickedPos, fHeight, fRadius);
}

void CTerrain_Desert::Change_Height_Flat(_vector vPickingPos, _float fHeight, _float fRadius)
{
	if (nullptr == m_pVIBufferCom)
		return;

	const _float4x4* pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	_matrix WorldMatrix = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(pWorldMatrix));

	_vector vDeterminant = XMMatrixDeterminant(WorldMatrix);
	_matrix WorldMatrixInverse = XMMatrixInverse(&vDeterminant, WorldMatrix);


	_vector vLocalPickedPos = XMVector3TransformCoord(vPickingPos, WorldMatrixInverse);
	m_pVIBufferCom->Change_Height_Flat(vLocalPickedPos, fHeight, fRadius);
}

void CTerrain_Desert::Change_Height_Sculpt(_vector vPickingPos, _float fAmount, _float fRadius, _float fMaxHeight)
{
	if (nullptr == m_pVIBufferCom)
		return;

	// 1. World -> Local 변환 (기존 Change_Height_Rect/Flat 함수와 동일)
	const _float4x4* pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	_matrix WorldMatrix = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(pWorldMatrix));

	_vector vDeterminant = XMMatrixDeterminant(WorldMatrix);
	_matrix WorldMatrixInverse = XMMatrixInverse(&vDeterminant, WorldMatrix);

	_vector vLocalPickedPos = XMVector3TransformCoord(vPickingPos, WorldMatrixInverse);

	// 2. VIBuffer의 스컬핑 함수 호출
	// fAmount는 맵툴에서 지정한 높이/강도 값입니다.
	m_pVIBufferCom->Change_Height_Sculpt(vLocalPickedPos, fAmount, fRadius, fMaxHeight);
}

void CTerrain_Desert::Change_Height_Smooth(_vector vPickingPos, _float fFactor, _float fRadius)
{
	if (nullptr == m_pVIBufferCom)
		return;


	const _float4x4* pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();

	_matrix WorldMatrix = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(pWorldMatrix));

	_vector vDeterminant = XMMatrixDeterminant(WorldMatrix);
	_matrix WorldMatrixInverse = XMMatrixInverse(&vDeterminant, WorldMatrix);


	_vector vLocalPickedPos = XMVector3TransformCoord(vPickingPos, WorldMatrixInverse);
	m_pVIBufferCom->Change_Height_Smooth(vLocalPickedPos, fFactor, fRadius);
}

_float CTerrain_Desert::Get_Height_In_World_Space(_float fWorldX, _float fWorldZ)
{
	// 1. World -> Local 변환 (Terrain_Desert은 보통 World 행렬이 Identity이므로 무시될 수 있으나, 정석대로 처리)
	_matrix WorldMatrix = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(m_pTransformCom->Get_WorldMatrixPtr()));
	_matrix WorldMatrixInverse = XMMatrixInverse(nullptr, WorldMatrix);
	_vector vWorldPos = XMVectorSet(fWorldX, 0.f, fWorldZ, 1.f);
	_vector vLocalPos = XMVector3TransformCoord(vWorldPos, WorldMatrixInverse);

	_float fLocalX = XMVectorGetX(vLocalPos);
	_float fLocalZ = XMVectorGetZ(vLocalPos);

	_float fLocalY = m_pVIBufferCom->Get_Interpolated_Height_Local(fLocalX, fLocalZ);

	_vector vFinalLocalPos = XMVectorSet(fLocalX, fLocalY, fLocalZ, 1.f);
	_vector vFinalWorldPos = XMVector3TransformCoord(vFinalLocalPos, WorldMatrix);

	return XMVectorGetY(vFinalWorldPos);
}

HRESULT CTerrain_Desert::Ready_Components()
{
	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_VIBuffer_Terrain_Desert"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Texture_Terrain_Desert"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Texture_Terrain_Desert_Red"),
		TEXT("Com_Texture_Red"), reinterpret_cast<CComponent**>(&m_pTextureCom_Red))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Texture_Terrain_Desert_Green"),
		TEXT("Com_Texture_Green"), reinterpret_cast<CComponent**>(&m_pTextureCom_Green))))
		return E_FAIL;

	/* Com_ORM */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_ORM_Texture_Terrain_Desert"),
		TEXT("Com_ORM"), reinterpret_cast<CComponent**>(&m_pORMTextureCom))))
		return E_FAIL;
	///* Com_ORM */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_ORM_Texture_Terrain_Desert_Red"),
	//	TEXT("Com_ORM_Red"), reinterpret_cast<CComponent**>(&m_pORMTextureCom_Red))))
	//	return E_FAIL;
	///* Com_ORM */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_ORM_Texture_Terrain_Desert_Green"),
	//	TEXT("Com_ORM_Green"), reinterpret_cast<CComponent**>(&m_pORMTextureCom_Green))))
	//	return E_FAIL;

	/* Com_Mask */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Texture_Terrain_Mask"),
		TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pMaskCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Navigation */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain_Desert::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture_Base")))
		return E_FAIL;
	if (FAILED(m_pTextureCom_Red->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture_Red")))
		return E_FAIL;
	if (FAILED(m_pTextureCom_Green->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture_Green")))
		return E_FAIL;

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_N))
		m_isORM = !m_isORM;

	if (true == m_isORM)
	{
		if (FAILED(m_pORMTextureCom->Bind_ShaderResources(m_pShaderCom, "g_ORMTexture")))
			return E_FAIL;
	}

	if (m_pMaptool)
	{
		ID3D11ShaderResourceView* pMaskSRV = m_pMaptool->Get_MaskSRV();

		if (pMaskSRV)
		{
			if (FAILED(m_pShaderCom->Bind_SRV("g_MaskTexture", pMaskSRV)))
				return E_FAIL;
		}
	}


	return S_OK;
}

CTerrain_Desert* CTerrain_Desert::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain_Desert* pInstance = new CTerrain_Desert(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain_Desert::Clone(void* pArg)
{
	CTerrain_Desert* pInstance = new CTerrain_Desert(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain_Desert");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain_Desert::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCom_Red);
	Safe_Release(m_pTextureCom_Green);
	Safe_Release(m_pMaskCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pORMTextureCom);
	Safe_Release(m_pORMTextureCom_Red);
	Safe_Release(m_pORMTextureCom_Green);

}
