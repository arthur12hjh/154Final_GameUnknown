#include "pch.h"
#include "Terrain_Desert2.h"
#include "GameInstance.h"

CTerrain_Desert2::CTerrain_Desert2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTerrain_Desert2::CTerrain_Desert2(const CTerrain_Desert2& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTerrain_Desert2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain_Desert2::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(1024.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CTerrain_Desert2::Priority_Update(_float fTimeDelta)
{
	m_pNavigationCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CTerrain_Desert2::Update(_float fTimeDelta)
{
	//m_pVIBufferCom->Culling(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CTerrain_Desert2::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif
}

HRESULT CTerrain_Desert2::Render()
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

void CTerrain_Desert2::Change_Height_Rect(_vector vPickingPos, _float fHeight, _float fRadius)
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

void CTerrain_Desert2::Change_Height_Flat(_vector vPickingPos, _float fHeight, _float fRadius)
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

void CTerrain_Desert2::Change_Height_Sculpt(_vector vPickingPos, _float fAmount, _float fRadius, _float fMaxHeight)
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

void CTerrain_Desert2::Change_Height_Smooth(_vector vPickingPos, _float fFactor, _float fRadius)
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

_float CTerrain_Desert2::Get_Height_In_World_Space(_float fWorldX, _float fWorldZ)
{
	// 1. World -> Local 변환 (Terrain_Desert2은 보통 World 행렬이 Identity이므로 무시될 수 있으나, 정석대로 처리)
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

HRESULT CTerrain_Desert2::Ready_Components()
{
	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_VIBuffer_Terrain_Desert2"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Texture_Terrain_Desert"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
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

HRESULT CTerrain_Desert2::Bind_ShaderResources()
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

	return S_OK;
}

CTerrain_Desert2* CTerrain_Desert2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain_Desert2* pInstance = new CTerrain_Desert2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain_Desert2::Clone(void* pArg)
{
	CTerrain_Desert2* pInstance = new CTerrain_Desert2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain_Desert2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain_Desert2::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
