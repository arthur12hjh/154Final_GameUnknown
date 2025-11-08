#include "pch.h"
#include "DebugProfiler.h"

#include "GameInstance.h"

CDebugProfiler::CDebugProfiler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CGameObject(pDevice, pContext)
{

}

HRESULT CDebugProfiler::Initialize()
{

	return S_OK;
}

void CDebugProfiler::Update(_float fTimeDeleta)
{
	_bool bIsOpen = 1 - ENUM_CLASS(m_eVisibility);
	_float fLoopTime = 0.f;
	ImGui::Begin("Profiler", &bIsOpen);
	
	ImGui::Text("Priority");
	ImGui::SameLine();
	fLoopTime = m_pGameInstance->GetLoopDurationTime(GAMELOOP_TYPE::PRIORITY) * 10.f;
	sprintf_s(m_szViewPercentText, "%.4f", fLoopTime);
	ImGui::ProgressBar(fLoopTime, ImVec2(-1, 0), m_szViewPercentText);

	ImGui::Text("Update");
	ImGui::SameLine();
	fLoopTime = m_pGameInstance->GetLoopDurationTime(GAMELOOP_TYPE::UPDATE) * 10.f;
	sprintf_s(m_szViewPercentText, "%.4f", fLoopTime);
	ImGui::ProgressBar(fLoopTime, ImVec2(-1, 0), m_szViewPercentText);

	ImGui::Text("Late_Update");
	ImGui::SameLine();
	fLoopTime = m_pGameInstance->GetLoopDurationTime(GAMELOOP_TYPE::LATE_UPDATE) * 10.f;
	sprintf_s(m_szViewPercentText, "%.4f", fLoopTime);
	ImGui::ProgressBar(fLoopTime, ImVec2(-1, 0), m_szViewPercentText);

	ImGui::Text("Collision");
	ImGui::SameLine();
	fLoopTime = m_pGameInstance->GetLoopDurationTime(GAMELOOP_TYPE::COLLISION) * 10.f;
	sprintf_s(m_szViewPercentText, "%.4f", fLoopTime);
	ImGui::ProgressBar(fLoopTime, ImVec2(-1, 0), m_szViewPercentText);

	ImGui::Text("Render");
	ImGui::SameLine();
	fLoopTime = m_pGameInstance->GetLoopDurationTime(GAMELOOP_TYPE::RENDER) * 10.f;
	sprintf_s(m_szViewPercentText, "%.4f", fLoopTime);
	ImGui::ProgressBar(fLoopTime, ImVec2(-1, 0), m_szViewPercentText);
	
	ImGui::End();

	if (!bIsOpen)
		m_eVisibility = VISIBILITY::HIDDEN;
}

HRESULT CDebugProfiler::Render()
{
	return S_OK;
}

CDebugProfiler* CDebugProfiler::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebugProfiler* pDebugProfiler = new CDebugProfiler(pDevice, pContext);
	if (FAILED(pDebugProfiler->Initialize()))
	{
		Safe_Release(pDebugProfiler);
		MSG_BOX("Create Fail : Debug Profiler");
	}
	return pDebugProfiler;
}

CGameObject* CDebugProfiler::Clone(void* pArg)
{
	return nullptr;
}

void CDebugProfiler::Free()
{
	__super::Free();

}
