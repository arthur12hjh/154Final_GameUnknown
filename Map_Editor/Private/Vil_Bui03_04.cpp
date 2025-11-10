#include "pch.h"
#include "Vil_Bui03_04.h"
#include "GameInstance.h"

CVil_Bui03_04::CVil_Bui03_04(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CVil_Bui03_04::CVil_Bui03_04(const CVil_Bui03_04& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CVil_Bui03_04::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVil_Bui03_04::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CVil_Bui03_04::Priority_Update(_float fTimeDelta)
{
}

void CVil_Bui03_04::Update(_float fTimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);

	// 2. _vector를 _float3 (x, y, z) 구조체로 저장합니다.
	_float3 vPosFloat3;
	XMStoreFloat3(&vPosFloat3, vPosition);

	// 3. swprintf_s를 사용하여 문자열을 포맷팅합니다.
	wchar_t szBuffer[256];
	swprintf_s(szBuffer, 256, L"Position: (%.2f, %.2f, %.2f)\n",
		vPosFloat3.x,
		vPosFloat3.y,
		vPosFloat3.z);

	// 4. 디버그 창에 출력합니다.
	OutputDebugStringW(szBuffer);
}

void CVil_Bui03_04::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CVil_Bui03_04::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
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

HRESULT CVil_Bui03_04::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Vil_Bui03_04"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CVil_Bui03_04::Bind_ShaderResources()
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

CVil_Bui03_04* CVil_Bui03_04::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVil_Bui03_04* pInstance = new CVil_Bui03_04(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVil_Bui03_04::Clone(void* pArg)
{
	CVil_Bui03_04* pInstance = new CVil_Bui03_04(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVil_Bui03_04");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVil_Bui03_04::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
