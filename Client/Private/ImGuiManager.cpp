#include "pch.h"
#include "ImGuiManager.h"

#include "GameInstance.h"
#include "GameObject.h"

#include "DebugProfiler.h"
#include "DebugCheatUI.h"

CImGuiManager::CImGuiManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImGuiManager::Initialize()
{
	if (FAILED(ADD_ImGuiObject()))
		return E_FAIL;

	return S_OK;
}

void CImGuiManager::Update(_float fTimeDeleta)
{
	for (auto& iter : m_ImGuis)
	{
		if (VISIBILITY::HIDDEN == iter.second->GetVisibility())
			continue;

		iter.second->Update(fTimeDeleta);
	}
}

HRESULT CImGuiManager::Render()
{
	return S_OK;
}

CGameObject* CImGuiManager::Find_ImGuiObject(const WCHAR* ImGuiTag)
{
	auto iter = m_ImGuis.find(ImGuiTag);
	if (iter == m_ImGuis.end())
		return nullptr;

	return iter->second;
}

void CImGuiManager::SetImGuiObjectVisiblilty(const WCHAR* ImGuiTag, VISIBILITY eVisiblility)
{
	auto pImGui = Find_ImGuiObject(ImGuiTag);
	if (nullptr == pImGui)
		return;

	pImGui->SetVisibility(eVisiblility);
}

HRESULT CImGuiManager::ADD_ImGuiObject()
{

#pragma region Profiler
	auto pDebugProfiler = CDebugProfiler::Create(m_pDevice, m_pContext);
	if (nullptr == pDebugProfiler)
		return E_FAIL;

	m_ImGuis.emplace(TEXT("ImGui_Profiler"), pDebugProfiler);
#pragma endregion

#pragma region Cheat UI
	auto pDebugCheatUI = CDebugCheatUI::Create(m_pDevice, m_pContext);
	if (nullptr == pDebugCheatUI)
		return E_FAIL;

	m_ImGuis.emplace(TEXT("ImGui_CheatUI"), pDebugCheatUI);
#pragma endregion


	return S_OK;
}

CImGuiManager* CImGuiManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CImGuiManager* pImGuiManager = new CImGuiManager(pDevice, pContext);
	if (FAILED(pImGuiManager->Initialize()))
	{
		Safe_Release(pImGuiManager);
		MSG_BOX("Create Fail : ImGui Manager");
	}
	return pImGuiManager;
}

void CImGuiManager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	for (auto& iter : m_ImGuis)
		Safe_Release(iter.second);

	m_ImGuis.clear();
}
