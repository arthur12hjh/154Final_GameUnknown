#include "pch.h"
#include "ImGuiMain.h"

#include "GameInstance.h"

#include "ImGuiManager.h"
#include "StringHelper.h"

#include "UIHUD.h"
#include "HUDLayer.h"

const char* CImGuiMain::m_szFpsComboText[] = {"30", "60", "144"};

CImGuiMain::CImGuiMain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImGuiMain::Initialize()
{
#ifdef _DEBUG
	if (FAILED(Ready_Default_Setting()))
		return E_FAIL;

	m_pImGuiManager = CImGuiManager::GetInstance();
	if (nullptr == m_pImGuiManager)
		return E_FAIL;

	m_pImGuiManager->Initialize(m_pDevice, m_pContext);
#endif
	return S_OK;
}

void CImGuiMain::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	// 윈도우 메시지 처리 등...
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug Tool", nullptr, m_ImGuiWindowFlags);
	CStringHelper::ConvertWideToUTF(m_pGameInstance->GetFrameText(), m_szFPS);
	ImGui::Text(m_szFPS);
	ImGui::SameLine();

	ImGui::PushItemWidth(100.f);
	ImGui::SetNextWindowSize({ 100, 50 });
	if (ImGui::BeginCombo("Frame", m_szFramePreivew))
	{
		for (_uint i = 0; i < 3; ++i)
		{
			if (ImGui::Selectable(m_szFpsComboText[i], false))
			{
				strcpy_s(m_szFramePreivew, m_szFpsComboText[i]);
				g_fGameFrame = atoi(m_szFramePreivew);
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::Checkbox("GamePause", &m_bIsGamePause))
		m_pGameInstance->SetGamePause(m_bIsGamePause);
	
	if (ImGui::Button("Show Profiler"))
		m_pImGuiManager->SetImGuiObjectVisiblilty(TEXT("ImGui_Profiler"), VISIBILITY::VISIBLE);

	if (ImGui::Button("Show Cheat List"))
		m_pImGuiManager->SetImGuiObjectVisiblilty(TEXT("ImGui_CheatUI"), VISIBILITY::VISIBLE);

	if (ImGui::Button("Show Hierarchy"))
		m_pImGuiManager->SetImGuiObjectVisiblilty(TEXT("ImGui_Hierarchy"), VISIBILITY::VISIBLE);

	if (ImGui::Checkbox("Toggle Show UI", &m_bIsToggleShowUI))
	{
		for (auto& pLayers : dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD())->Get_Layers())
		{
			auto pLayer = pLayers.second;

			for (auto& pUIObjs : *pLayer->Get_UserInterfaces())
			{
				pUIObjs.second->SetVisibility(m_bIsToggleShowUI ? VISIBILITY::VISIBLE : VISIBILITY::HIDDEN);
			}
		}
	}

	if (ImGui::Checkbox("Toggle Show UI Debug", &m_bIsToggleShowUIDebug))
	{
		dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD())->Set_Show_Debug_Rect(m_bIsToggleShowUIDebug);
	}


	m_pImGuiManager->Update(fTimeDelta);
	ImGui::End();
	ImGui::EndFrame();
#endif
}

HRESULT CImGuiMain::Render()
{
#ifdef _DEBUG
	ImGui::Render();
	// 여기서 백버퍼 클리어, 다른 렌더 등 수행
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
	return S_OK;
}

HRESULT CImGuiMain::Ready_Default_Setting()
{
#ifdef _DEBUG
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigDockingAlwaysTabBar = true;  // 창 1개여도 탭바 유지
	io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/gulim.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesKorean());

	m_ImGuiWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

	ImGui::StyleColorsDark();
#endif
	return S_OK;
}

CImGuiMain* CImGuiMain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CImGuiMain* pImGuiMain = new CImGuiMain(pDevice, pContext);
	if (FAILED(pImGuiMain->Initialize()))
	{
		Safe_Release(pImGuiMain);
		MSG_BOX("Create Fail : ImGui Main");
	}
	return pImGuiMain;
}

void CImGuiMain::Free()
{
	__super::Free();

#ifdef _DEBUG
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CImGuiManager::DestroyInstance();
#endif // _DEBUG

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}