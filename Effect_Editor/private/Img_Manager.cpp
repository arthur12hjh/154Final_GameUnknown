#include "pch.h"
#include "Img_Manager.h"
#include "ImgBase.h"

#include "Particle_Setting.h"

IMPLEMENT_SINGLETON(CImg_Manager);

CImg_Manager::CImg_Manager()
{
}

HRESULT CImg_Manager::Ready_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);
	RECT rect = {};
	GetClientRect(g_hWnd, &rect);

	ImGuiIO& io = ImGui::GetIO();
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard;

	io.DisplaySize = { float(rect.right - rect.left),
					   float(rect.bottom - rect.top) };


	m_ImGuiIo = io;

	// 창 위치를 (0,0)으로 설정
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	m_ImGuiWindowFlag = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_MenuBar;

	// 패딩과 테두리를 고려해 창 스타일 비활성화
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	//ImGui::SetNextWindowDockID(ImGui::GetID("IndependentDockSpace"), ImGuiCond_FirstUseEver);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	m_pDevice = pDevice;
	m_pContext = pContext;
	m_pDevice->AddRef();
	m_pContext->AddRef();

	if (FAILED(ADD_ImgUserInterface()))
		return E_FAIL;

	ImGui::EndFrame();
	return S_OK;
}

void CImg_Manager::Update_Manager(_float fDeletaTime)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	for (auto pair : m_UiMap)
	{
		if (!pair.second.second)
			continue;

		pair.second.first->Update(fDeletaTime);
	}
	ImGui::EndFrame();
}

void CImg_Manager::Render_Manager()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


	for (auto pair : m_UiMap)
	{
		if (!pair.second.second)
			continue;

		pair.second.first->Render();
	}
}

HRESULT CImg_Manager::ADD_UI(const _wstring& _name, CImgBase* UI)
{
	auto pair = m_UiMap.find(_name);

	if (pair == m_UiMap.end())
		m_UiMap.insert({ _name, {UI, true} });
	return S_OK;
}

void CImg_Manager::Remove_UI(const _wstring& _name, CImgBase* UI)
{
	auto pair = m_UiMap.find(_name);

	if (pair != m_UiMap.end())
		m_UiMap.erase(pair);

}

CImgBase* CImg_Manager::Find_UI(const _wstring& _name)
{
	auto pair = m_UiMap.find(_name);

	if (pair == m_UiMap.end())
		return nullptr;

	return pair->second.first;
}

void CImg_Manager::ResetData()
{
}

HRESULT CImg_Manager::ADD_ImgUserInterface()
{
	//if (FAILED(ADD_UI(TEXT("Game_Profiler"), CImg_ObjectSetting::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	//
	if (FAILED(ADD_UI(TEXT("Particle_Setting"), CParticle_Setting::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

void CImg_Manager::Free()
{
	__super::Free();

	for_each(m_UiMap.begin(), m_UiMap.end(), [&](auto pair)
		{
			Safe_Release(pair.second.first);
		});
	m_UiMap.clear();
	// Cleanup
	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}