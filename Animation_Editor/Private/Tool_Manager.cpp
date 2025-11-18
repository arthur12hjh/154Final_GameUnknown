#include "pch.h"
#include "Tool_Manager.h"
#include "GameInstance.h"

#include "ImGui_Manager.h"
#include "Data_Manager.h"


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
	Safe_AddRef(m_pGameInstance);

	m_pImGuiManager = CImGui_Manager::Create(m_pDevice, m_pContext);
	if (nullptr == m_pImGuiManager)
		return E_FAIL;

	m_pDataManager = CData_Manager::Create();
	if (nullptr == m_pDataManager)
		return E_FAIL;

	return S_OK;
}


void CTool_Manager::Priority_Update(_float fTimeDelta)
{
	m_pImGuiManager->Priority_Update(fTimeDelta);
}

void CTool_Manager::Update(_float fTimeDelta)
{
	m_pImGuiManager->Update(fTimeDelta);
}

void CTool_Manager::Late_Update(_float fTimeDelta)
{
	m_pImGuiManager->Late_Update(fTimeDelta);
}

HRESULT CTool_Manager::Render()
{
	return m_pImGuiManager->Render();

}

void CTool_Manager::Set_Active(_bool bIsActive)
{
	m_pImGuiManager->Set_Active(bIsActive);
}


HRESULT CTool_Manager::Save_Data()
{
	return m_pDataManager->Save_Data();
}

const vector<ANIM_NOTIFY>* CTool_Manager::Find_AnimationNotifyData(_wstring szAnimationTag)
{
	return m_pDataManager->Find_AnimationNotifyData(szAnimationTag);
}

unordered_map<_wstring, vector<ANIM_NOTIFY>>* CTool_Manager::Get_AnimationEventMapPtr()
{
	return m_pDataManager->Get_AnimationEventMapPtr();
}

void CTool_Manager::Release_Manager()
{
	DestroyInstance();

	Safe_Release(m_pImGuiManager);
	Safe_Release(m_pDataManager);
}

void CTool_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}