#include "pch.h"
#include "InstanceModel.h"

#include "GameInstance.h"

CInstanceModel::CInstanceModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CGameObject(pDevice, pContext)
{
}

CInstanceModel::CInstanceModel(const CInstanceModel& Prototype) :
	CGameObject(Prototype)
{
}

_uint CInstanceModel::Get_NumInstance() const
{
	return m_pModelCom->Get_NumInstance();
}

VTX_INSTANCE_MODEL* CInstanceModel::Get_InstanceVertices() const
{
	return m_pModelCom->Get_InstanceVertices();
}

HRESULT CInstanceModel::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CInstanceModel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MODEL_INSTANCE_LOAD_DESC* pLoadDesc = static_cast<MODEL_INSTANCE_LOAD_DESC*>(pArg);
	if (nullptr == pLoadDesc)
		return E_FAIL;

	if (FAILED(Ready_Components(pLoadDesc->pPrototypeTag)))
		return E_FAIL;

	if (FAILED(Update_Instancing_Buffer(pLoadDesc)))
		return E_FAIL;

	return S_OK;
}

void CInstanceModel::Priority_Update(_float fTimeDelta)
{
}

void CInstanceModel::Update(_float fTimeDelta)
{
}

void CInstanceModel::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CInstanceModel::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->GetModelNumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_MatrialTexture(m_pShaderCom, i, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_MatrialTexture(m_pShaderCom, i, "g_NormalTexture", TEXTURE_TYPE::NORMAL, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CInstanceModel::Ready_Components(const _tchar* PrototypeTag)
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

HRESULT CInstanceModel::Bind_ShaderResources()
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

HRESULT CInstanceModel::Update_Instancing_Buffer(const MODEL_INSTANCE_LOAD_DESC* pLoadDesc)
{
	HRESULT hr = m_pModelCom->Initialize(const_cast<MODEL_INSTANCE_LOAD_DESC*>(pLoadDesc));

	if (FAILED(hr))
		return hr;

	return S_OK;
}

CInstanceModel* CInstanceModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInstanceModel* pInstanceModel = new CInstanceModel(pDevice, pContext);
	if (FAILED(pInstanceModel->Initialize_Prototype()))
	{
		Safe_Release(pInstanceModel);
		MSG_BOX("Create Fail : Instance Model");
	}
	return pInstanceModel;
}

CGameObject* CInstanceModel::Clone(void* pArg)
{
	CInstanceModel* pInstanceModel = new CInstanceModel(*this);
	if (FAILED(pInstanceModel->Initialize(pArg)))
	{
		Safe_Release(pInstanceModel);
		MSG_BOX("Clone Fail : Instance Model");
	}
	return pInstanceModel;
}

void CInstanceModel::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
