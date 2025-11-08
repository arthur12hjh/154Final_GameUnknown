#include "pch.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "Navigation.h"
#include "Cell.h"
#include <fstream>

IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager() 
{

}


HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	return S_OK;
}

void CImgui_Manager::Priority_Update(_float fTimeDelta)
{
}

void CImgui_Manager::Update(_float fTimeDelta)
{
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}
}

void CImgui_Manager::Late_Update(_float fTimeDelta)
{
}

HRESULT CImgui_Manager::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowSize(ImVec2(300.f, 0.f), ImGuiCond_Once);


	ImGui::Begin("Map Editor");

	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	//m_pCameraTransform = reinterpret_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Layer_Camera"), TEXT("Com_Transform")));

	return S_OK;
}

void CImgui_Manager::Update_Rotation()
{
}

HRESULT CImgui_Manager::Save_Map_Objects()
{
	// 맵 데이터 파일 열기
	HANDLE hFile = CreateFile(TEXT("../Bin/DataFiles/MapData.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
	{
		MessageBoxW(g_hWnd, L"파일 생성 실패", L"알림", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CImgui_Manager::Load_Map_Objects()
{
	return S_OK;
}


void CImgui_Manager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

}
