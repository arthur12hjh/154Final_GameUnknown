#include "pch.h"
#include "Tool_Manager.h"
#include "GameInstance.h"

#include "ImGui_Manager.h"


IMPLEMENT_SINGLETON(CTool_Manager);

CTool_Manager::CTool_Manager()
{

}

HRESULT CTool_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pGameInstance = CGameInstance::GetInstance();

	m_pImGuiManager = CImGui_Manager::Create(m_pDevice, m_pContext);
	if (nullptr == m_pImGuiManager)
		return E_FAIL;

	return S_OK;
}


void CTool_Manager::Priority_Update(_float fTimeDelta)
{
}

void CTool_Manager::Update(_float fTimeDelta)
{
}

void CTool_Manager::Late_Update(_float fTimeDelta)
{
}

HRESULT CTool_Manager::Render()
{
	return m_pImGuiManager->Render();

}

void CTool_Manager::Release_Manager()
{
	DestroyInstance();

	Safe_Release(m_pImGuiManager);
}

void CTool_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}