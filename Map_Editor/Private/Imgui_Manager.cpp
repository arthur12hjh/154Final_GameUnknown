#include "pch.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "Navigation.h"
#include "Cell.h"
#include "Terrain.h"
#include "LightTool.h"
#include "MapTool.h"
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

	list<CGameObject*>* pTerrainList = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Layer_Terrain"));

	if (pTerrainList != nullptr && !pTerrainList->empty())
	{
		m_pTerrain = dynamic_cast<CTerrain*>(pTerrainList->back());
	}
	else
	{
		m_pTerrain = nullptr; // 안전하게 nullptr로 설정
	}
	
	m_pMapTool = new CMapTool();
	if (m_pMapTool != nullptr)
		m_pMapTool->Initialize(pDevice, pContext);

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
	
	m_pMapTool->Update(fTimeDelta);
	m_pLightTool->Update(fTimeDelta);

}

void CImgui_Manager::Late_Update(_float fTimeDelta)
{
}

HRESULT CImgui_Manager::Render()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
	m_pMapTool->Render();
	m_pLightTool->Render();
	m_pCameraTool->Render();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

void CImgui_Manager::Free()
{

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Delete(m_pLightTool);
	Safe_Delete(m_pMapTool);
	Safe_Release(m_pCameraTool);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

}
