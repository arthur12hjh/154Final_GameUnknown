#include "pch.h"
#include "ImGuiMain.h"

#include "GameInstance.h"
#include "StringHelper.h"

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
	if (FAILED(Ready_Default_Setting()))
		return E_FAIL;

	return S_OK;
}

void CImGuiMain::Update(_float fTimeDelta)
{
	// 윈도우 메시지 처리 등...
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();

	if (ImGui::Begin("Debug Tool"))
	{
		CStringHelper::ConvertWideToUTF(m_pGameInstance->GetFrameText(), m_szFPS);
		ImGui::Text(m_szFPS);
		ImGui::SameLine();
		if (ImGui::BeginCombo("Frame", m_szFramePreivew))
		{

		}


		if (ImGui::Checkbox("GamePause", &m_bIsGamePause))
			m_pGameInstance->SetGamePause(m_bIsGamePause);

		


	}
	ImGui::End();
}

HRESULT CImGuiMain::Render()
{
	ImGui::Render();
	// 여기서 백버퍼 클리어, 다른 렌더 등 수행
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return S_OK;
}

HRESULT CImGuiMain::Ready_Default_Setting()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	//ImGuiViewport* vp = ImGui::GetMainViewport();
	//ImGui::SetNextWindowViewport(vp->ID);
	// Setup Dear ImGui style

	m_ImGuiWindowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::StyleColorsDark();

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

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}