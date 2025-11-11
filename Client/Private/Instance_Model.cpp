#include "pch.h"
#include "Instance_Model.h"

#include "GameInstance.h"
#include "Camera.h"

CInstance_Model::CInstance_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

CInstance_Model::CInstance_Model(const CInstance_Model& Prototype) :
    CGameObject(Prototype)
{
}

HRESULT CInstance_Model::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CInstance_Model::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CInstance_Model::Priority_Update(_float fTimeDelta)
{
}

void CInstance_Model::Update(_float fTimeDelta)
{
}

void CInstance_Model::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CInstance_Model::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->GetModelNumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
        if (FAILED(m_pModelCom->Bind_MatrialTexture(m_pShaderCom, i, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, 0)))
            return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

    return S_OK;
}

HRESULT CInstance_Model::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Instance_Model_Dororong"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_Instance_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstance_Model::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	auto pCamera = m_pGameInstance->GetMainCamera();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamFar", &pCamera->GetCameraInfo().fFar, sizeof(_float))))
		return E_FAIL;
	Safe_Release(pCamera);

	return S_OK;
}

CInstance_Model* CInstance_Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInstance_Model* pInstanceModel = new CInstance_Model(pDevice, pContext);
	if (FAILED(pInstanceModel->Initialize_Prototype()))
	{
		Safe_Release(pInstanceModel);
		MSG_BOX("Create Fail : Instance Model");
	}
    return pInstanceModel;
}

CGameObject* CInstance_Model::Clone(void* pArg)
{
	CInstance_Model* pInstanceModel = new CInstance_Model(*this);
	if (FAILED(pInstanceModel->Initialize(pArg)))
	{
		Safe_Release(pInstanceModel);
		MSG_BOX("Clone Fail : Instance Model");
	}
	return pInstanceModel;
}

void CInstance_Model::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
