#include "pch.h"
#include "Instance_Desert.h"

#include "GameInstance.h"

CInstance_Desert::CInstance_Desert(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CActor(pDevice, pContext)
{
}

CInstance_Desert::CInstance_Desert(const CInstance_Desert& Prototype) :
	CActor(Prototype)
{
}

_uint CInstance_Desert::Get_NumInstance() const
{
	return m_pModelCom->Get_NumInstance();
}

VTX_INSTANCE_MODEL* CInstance_Desert::Get_InstanceVertices() const
{
	return m_pModelCom->Get_InstanceVertices();
}

HRESULT CInstance_Desert::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CInstance_Desert::Initialize(void* pArg)
{
	MODEL_INSTANCE_LOAD_DESC* pLoadDesc = static_cast<MODEL_INSTANCE_LOAD_DESC*>(pArg);
	if (nullptr == pLoadDesc)
		return E_FAIL;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (pLoadDesc && pLoadDesc->pPrototypeTag)
	{
		wcsncpy_s(m_ComponentTag, 256, pLoadDesc->pPrototypeTag, _TRUNCATE);
	}

	if (FAILED(Ready_Components(m_ComponentTag)))
		return E_FAIL;

	if (FAILED(Update_Instancing_Buffer(pLoadDesc)))
		return E_FAIL;

	return S_OK;
}

void CInstance_Desert::Priority_Update(_float fTimeDelta)
{
}

void CInstance_Desert::Update(_float fTimeDelta)
{
}

void CInstance_Desert::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CInstance_Desert::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->GetModelNumMeshes();
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_MatrialTexture(m_pShaderCom, i, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_MatrialTexture(m_pShaderCom, i, "g_NormalTexture", TEXTURE_TYPE::NORMAL, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_MatrialTexture(m_pShaderCom, i, "g_ORMTexture", TEXTURE_TYPE::ORM, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CInstance_Desert::Ready_Components(const _tchar* PrototypeTag)
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), PrototypeTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_Instance_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstance_Desert::Bind_ShaderResources()
{
	/*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;*/

	_float4x4 IdentityMatrix;
	XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &IdentityMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstance_Desert::Update_Instancing_Buffer(const MODEL_INSTANCE_LOAD_DESC* pLoadDesc)
{
	HRESULT hr = m_pModelCom->Initialize(const_cast<MODEL_INSTANCE_LOAD_DESC*>(pLoadDesc));

	if (FAILED(hr))
		return hr;

	return S_OK;
}

CInstance_Desert* CInstance_Desert::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInstance_Desert* pInstance_Desert = new CInstance_Desert(pDevice, pContext);
	if (FAILED(pInstance_Desert->Initialize_Prototype()))
	{
		Safe_Release(pInstance_Desert);
		MSG_BOX("Create Fail : Instance Model");
	}
	return pInstance_Desert;
}

CGameObject* CInstance_Desert::Clone(void* pArg)
{
	CInstance_Desert* pInstance_Desert = new CInstance_Desert(*this);
	if (FAILED(pInstance_Desert->Initialize(pArg)))
	{
		Safe_Release(pInstance_Desert);
		MSG_BOX("Clone Fail : Instance Model");
	}
	return pInstance_Desert;
}

void CInstance_Desert::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
