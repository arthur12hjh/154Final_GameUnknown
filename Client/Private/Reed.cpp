#include "pch.h"
#include "Reed.h"

#include "GameInstance.h"

CReed::CReed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CGameObject(pDevice, pContext)
{
}

CReed::CReed(const CReed& Prototype) :
	CGameObject(Prototype)
{
}

HRESULT CReed::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CReed::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CReed::Priority_Update(_float fTimeDelta)
{
}

void CReed::Update(_float fTimeDelta)
{
	static _float fTimeAcc = 0.0f;
	fTimeAcc += fTimeDelta;

	m_pShaderCom->Bind_RawValue("g_fTime", &fTimeAcc, sizeof(_float));
}

void CReed::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CReed::Render()
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

		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CReed::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Reed"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_Instance_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CReed::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	_float fSpeed = 1.f;	   // 빠르기
	_float fFrequency = 0.4f;  // 패턴 밀도
	_float fAmplitude = 0.8f;  // 흔들림 최대 폭

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fWaveSpeed", &fSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fWaveFrequency", &fFrequency, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fWaveAmplitude", &fAmplitude, sizeof(_float))))
		return E_FAIL;

	struct MinMaxScale
	{
		_float fMinScale = 0.7f;
		_float fMaxScale = 1.0f;
		_float fReserved = 0.0f;
	};

	MinMaxScale scaleInfo;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMinMaxScale", &scaleInfo, sizeof(scaleInfo))))
		return E_FAIL;

	return S_OK;
}

CReed* CReed::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CReed* pInstanceModel = new CReed(pDevice, pContext);
	if (FAILED(pInstanceModel->Initialize_Prototype()))
	{
		Safe_Release(pInstanceModel);
		MSG_BOX("Create Fail : Instance Model");
	}
	return pInstanceModel;
}

CGameObject* CReed::Clone(void* pArg)
{
	CReed* pInstanceModel = new CReed(*this);
	if (FAILED(pInstanceModel->Initialize(pArg)))
	{
		Safe_Release(pInstanceModel);
		MSG_BOX("Clone Fail : Instance Model");
	}
	return pInstanceModel;
}

void CReed::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
