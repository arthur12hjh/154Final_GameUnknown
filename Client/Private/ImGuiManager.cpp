#include "pch.h"
#include "ImGuiManager.h"

#include "GameInstance.h"
#include "GameObject.h"

#include "DebugProfiler.h"
#include "DebugCheatUI.h"
#include "DebugHierarchy.h"
#include "ShaderDebugger.h"
#include "CameraActionInserter.h"
#include "CinematicMaker.h"
#include "LightTool.h"

IMPLEMENT_SINGLETON(CImGuiManager);

HRESULT CImGuiManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	m_pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);

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

void CImGuiManager::SetLevelFreeCamera()
{
	auto pImGui = Find_ImGuiObject(TEXT("ImGui_CheatUI"));
	if (nullptr == pImGui)
		return;

	static_cast<CDebugCheatUI *>(pImGui)->SetLevelMainCamera();
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

#pragma region Hirerarchy
	auto pDebugHierarchy = CDebugHierarchy::Create(m_pDevice, m_pContext);
	if (nullptr == pDebugHierarchy)
		return E_FAIL;

	m_ImGuis.emplace(TEXT("ImGui_Hierarchy"), pDebugHierarchy);
#pragma endregion

#pragma region ShaderDebugger
	auto pShaderDebugger = CShaderDebugger::Create(m_pDevice, m_pContext);
	if (nullptr == pShaderDebugger)
		return E_FAIL;

	m_ImGuis.emplace(TEXT("ImGui_ShaderDebuffer"), pShaderDebugger);
#pragma endregion

#pragma region CameraActionInserter
	auto pCameraActionInserter = CCameraActionInserter::Create(m_pDevice, m_pContext);
	if (nullptr == pCameraActionInserter)
		return E_FAIL;

	m_ImGuis.emplace(TEXT("ImGui_CameraActionInserter"), pCameraActionInserter);
#pragma endregion

#pragma region CameraActionInserter
	auto pCinematicMaker = CCinematicMaker::Create(m_pDevice, m_pContext);
	if (nullptr == pCinematicMaker)
		return E_FAIL;

	m_ImGuis.emplace(TEXT("ImGui_CinematicMaker"), pCinematicMaker);
#pragma endregion

#pragma region LightTool
	auto pLightTool = CLightTool::Create(m_pDevice, m_pContext);
	if (pLightTool == nullptr)
		return E_FAIL;

	m_ImGuis.emplace(TEXT("ImGui_LightTool"), pLightTool);
#pragma endregion

	return S_OK;
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
