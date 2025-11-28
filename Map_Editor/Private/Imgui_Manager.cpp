#include "pch.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "Navigation.h"
#include "Cell.h"
#include "Terrain.h"
#include "LightTool.h"
#include "MapTool.h"
#include "MapTool_Desert.h"
#include "Camera_Tool.h"
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

	m_iCurrentLevel = m_pGameInstance->GetCurrentLevelID();

	m_pLightTool = new CLightTool();
	if (m_pLightTool != nullptr)
		m_pLightTool->Initialize(pDevice, pContext);
	
	m_pCameraTool = CCamera_Tool::Create(pDevice, pContext);


	return S_OK;
}

void CImgui_Manager::Priority_Update(_float fTimeDelta)
{
}

void CImgui_Manager::Update(_float fTimeDelta)
{
	if( ImGui::GetIO().WantCaptureKeyboard)
	{
		g_bIsImgKeyBoardFoucs = true;
	}
	else
		g_bIsImgKeyBoardFoucs = false;
	
	m_iCurrentLevel = m_pGameInstance->GetCurrentLevelID();

	if (m_bIsMapToolDesertActive && m_iCurrentLevel == ENUM_CLASS(LEVEL::DESERT))
		m_pMapTool_Desert->Update(fTimeDelta);

	if(m_bIsMapToolActive && m_iCurrentLevel == ENUM_CLASS(LEVEL::VILLAGE))
		m_pMapTool->Update(fTimeDelta);

	if(m_bIsLightToolActive)
		m_pLightTool->Update(fTimeDelta);

}

void CImgui_Manager::Late_Update(_float fTimeDelta)
{
}

HRESULT CImgui_Manager::Render()
{
	if (ImGui::GetCurrentContext() == nullptr)
	{
		// 컨텍스트가 없으면, DirectX 11 백엔드도 초기화되지 않았을 가능성이 높으므로
		// ImGui_ImplDX11_NewFrame() 등을 호출하지 않고 바로 리턴합니다.
		return S_OK;
	}

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
	Render_MainMenu();

	if (m_bIsMapToolDesertActive && m_iCurrentLevel == ENUM_CLASS(LEVEL::DESERT))
		m_pMapTool_Desert->Render();

	if (m_bIsMapToolActive && m_iCurrentLevel == ENUM_CLASS(LEVEL::VILLAGE))
		m_pMapTool->Render();

	if (m_bIsLightToolActive)
		m_pLightTool->Render();

	if (m_bIsCameraToolActive)
		m_pCameraTool->Render();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

HRESULT CImgui_Manager::Render_MainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Tools")) // "Tools" 메뉴 생성
		{
			// MapTool 슬롯 (눌렀을 때 m_bIsMapToolActive 플래그를 토글)
			if (ImGui::MenuItem("Map Tool", NULL, &m_bIsMapToolActive))
			{
			}

			if (ImGui::MenuItem("Map Tool Desert", NULL, &m_bIsMapToolDesertActive))
			{
			}

			// LightTool 슬롯
			if (ImGui::MenuItem("Light Tool", NULL, &m_bIsLightToolActive))
			{
			}

			// CameraTool 슬롯
			if (ImGui::MenuItem("Camera Tool", NULL, &m_bIsCameraToolActive))
			{
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	return S_OK;
}

HRESULT CImgui_Manager::Create_MapTool_For_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (m_pMapTool == nullptr)
	{
		m_pMapTool = new CMapTool();
		if (m_pMapTool == nullptr) return E_FAIL;

		if (FAILED(m_pMapTool->Initialize(pDevice, pContext))) // MapTool 초기화는 이제 Level이 전환될 때마다 발생합니다.
		{
			Safe_Delete(m_pMapTool);
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CImgui_Manager::Create_MapTool_For_Desert(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (m_pMapTool_Desert == nullptr)
	{
		m_pMapTool_Desert = new CMapTool_Desert();
		if (m_pMapTool_Desert == nullptr) return E_FAIL;

		if (FAILED(m_pMapTool_Desert->Initialize(pDevice, pContext))) // MapTool 초기화는 이제 Level이 전환될 때마다 발생합니다.
		{
			Safe_Delete(m_pMapTool_Desert);
			return E_FAIL;
		}
	}

	return S_OK;
}

void CImgui_Manager::Destroy_MapTool_For_Level()
{
	Safe_Delete(m_pMapTool);
}

void CImgui_Manager::Destroy_MapTool_Desert_For_Level()
{
	Safe_Delete(m_pMapTool_Desert);
}

void CImgui_Manager::Free()
{

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pMapTool);
	Safe_Release(m_pMapTool_Desert);
	Safe_Release(m_pLightTool);
	Safe_Release(m_pCameraTool);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

}
