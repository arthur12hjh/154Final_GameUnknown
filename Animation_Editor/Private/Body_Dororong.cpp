#include "pch.h"
#include "Body_Dororong.h"

#include "GameInstance.h"

#include "Character.h"

CBody_Dororong::CBody_Dororong(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Dororong::CBody_Dororong(const CBody_Dororong& Prototype)
	: CPartObject{ Prototype }
{
}

const _float4x4* CBody_Dororong::Get_BoneMatrixPtr(const _char* pBoneName) const
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

_bool CBody_Dororong::isFinish_Att()
{
	return false;
}

HRESULT CBody_Dororong::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Dororong::Initialize(void* pArg)
{
	BODY_DORORONG_DESC* pDesc = static_cast<BODY_DORORONG_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iAnimationIndex = 0;
	m_pModelCom->Set_AnimationIndex(m_iAnimationIndex);

	return S_OK;
}

void CBody_Dororong::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CBody_Dororong::Update(_float fTimeDelta)
{
	//if (*m_pParentState & CCharacter::STATE_ATTACK)
	//	m_pModelCom->Set_AnimationIndex(0, false);
	//
	//if (*m_pParentState & CCharacter::STATE_IDLE)
	//	m_pModelCom->Set_AnimationIndex(3);
	//
	//if (*m_pParentState & CCharacter::STATE_WALK)
	//	m_pModelCom->Set_AnimationIndex(4);

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_UP))
	{
		m_iAnimationIndex++;
		m_pModelCom->Set_AnimationIndex(m_iAnimationIndex);
	}

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_DOWN))
	{
		m_iAnimationIndex--;
		m_pModelCom->Set_AnimationIndex(m_iAnimationIndex);
	}

	m_isAnimFinish = m_pModelCom->Play_Animation(fTimeDelta);

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

}

void CBody_Dororong::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG

#endif
}

HRESULT CBody_Dororong::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

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

HRESULT CBody_Dororong::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CBody_Dororong::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Dororong"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Dororong::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CBody_Dororong* CBody_Dororong::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Dororong* pInstance = new CBody_Dororong(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Dororong");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Dororong::Clone(void* pArg)
{
	CBody_Dororong* pInstance = new CBody_Dororong(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Dororong");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Dororong::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
