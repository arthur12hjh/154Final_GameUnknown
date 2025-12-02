#include "pch.h"
#include "Player_Test.h"
#include "GameInstance.h"

CPlayer_Test::CPlayer_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPlayer_Test::CPlayer_Test(const CPlayer_Test& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CPlayer_Test::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Test::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Test::Priority_Update(_float fTimeDelta)
{
}

void CPlayer_Test::Update(_float fTimeDelta)
{
	_vector vWorldPos = m_pTransformCom->Get_State(STATE::POSITION);

	// 2. 출력할 문자열을 포맷합니다.
	wchar_t szOutput[256];
	swprintf_s(szOutput, 256, L"Player Position: X: %.3f, Y: %.3f, Z: %.3f\n",
		XMVectorGetX(vWorldPos), XMVectorGetY(vWorldPos), XMVectorGetZ(vWorldPos));

	// 3. Visual Studio 출력 창에 출력합니다.
	OutputDebugString(szOutput);
}

void CPlayer_Test::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CPlayer_Test::Render()
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

HRESULT CPlayer_Test::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Model_Eve"), 
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Test::Bind_ShaderResources()
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

CPlayer_Test* CPlayer_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Test* pInstance = new CPlayer_Test(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Test::Clone(void* pArg)
{
	CPlayer_Test* pInstance = new CPlayer_Test(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Test");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Test::Free()
{
	__super::Free();

	//Safe_Release(m_p)
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
