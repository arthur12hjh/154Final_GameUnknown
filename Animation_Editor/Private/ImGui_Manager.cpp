#include "pch.h"
#include "ImGui_Manager.h"
#include "GameInstance.h"
#include "Tool_Manager.h"

#include "GameObject.h"
#include "ContainerObject.h"
#include "Animation.h"

CImGui_Manager::CImGui_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
	, m_pTool_Manager{ CTool_Manager::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pTool_Manager);
}

HRESULT CImGui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	//LoadFont();

	m_bIsActive = FALSE;
	m_bIsActiveTrigger = FALSE;

	return S_OK;
}

void CImGui_Manager::Priority_Update(_float fTimeDelta)
{
	if (m_bIsActiveTrigger == TRUE)
		m_bIsActive = TRUE;

	if (m_bIsActiveTrigger == FALSE)
		m_bIsActive = FALSE;

	if (m_bIsActive == FALSE)
		return;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}


void CImGui_Manager::Update(_float fTimeDelta)
{
	if (m_bIsActive == FALSE)
		return;

	Update_ToolBar();
	Update_AnimationList();
	Update_KeyFrameTool();
}

void CImGui_Manager::Late_Update(_float fTimeDelta)
{
	if (m_bIsActive == FALSE)
		return;
}

HRESULT CImGui_Manager::Render()
{
	if (m_bIsActive == FALSE)
		return S_OK;

	ShowGUI();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}


void CImGui_Manager::ShowGUI()
{
}

void CImGui_Manager::LoadFont()
{
	ImGuiIO& io = ImGui::GetIO();
	ImFont* font = nullptr;

	string strFontTag = "../Bin/Resources/Fonts/H2GTRM.TTF";

	ifstream ifile;

	ifile.open(strFontTag);
	if (ifile)
	{
		font = io.Fonts->AddFontFromFileTTF(strFontTag.c_str(), 16.f, NULL, io.Fonts->GetGlyphRangesKorean());
	}
	IM_ASSERT(font != NULL);

}

void CImGui_Manager::Create_Character(const _wstring& szCharacterTag)
{
	if (nullptr != m_pSelectedObject)
	{
		m_pSelectedObject->Set_Dead(TRUE);
		m_pAnimationList = nullptr;
		m_pSelectedObject = nullptr;
	}

	m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), szCharacterTag, ENUM_CLASS(LEVEL::EDITOR), TEXT("Layer_Player"));

	list<CGameObject*>* pObjLists = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::EDITOR), TEXT("Layer_Player"));

	m_pSelectedObject = pObjLists->back();

	m_pAnimationList = static_cast<CModel*>(static_cast<CContainerObject*>(m_pSelectedObject)->Get_Component(TEXT("Part_Body"), TEXT("Com_Model")))->Get_AnimationList();
}

void CImGui_Manager::Kill_Character()
{
	if (nullptr == m_pSelectedObject)
		return;
	m_pSelectedObject->Set_Dead(TRUE);
	m_pSelectedObject = nullptr;
	m_pAnimationList = nullptr;
}

void CImGui_Manager::Update_ToolBar()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0)); // 화면 상단 좌표
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 30)); // 전체 너비, 높이 30
	ImGui::Begin("Toolbar", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoCollapse);

	if (ImGui::Button("Load"))
	{
		ImGui::OpenPopup("Load");
	}
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		ImGui::OpenPopup("Save");
	}
	ImGui::SameLine();
	if (ImGui::Button("Editor Preferences"))
	{
		ImGui::OpenPopup("EditorPreferences");
	}
	ImGui::SameLine();
	if (ImGui::Button("Animation"))
	{
		ImGui::OpenPopup("Animation");
	}
	ImGui::SameLine();
	if (ImGui::Button("Sound"))
	{
		ImGui::OpenPopup("Sound");
	}
	ImGui::SameLine();
	if (ImGui::Button("Tools"))
	{
		ImGui::OpenPopup("Tools");
	}

	Update_ToolBar_LoadCharacter();
	Update_ToolBar_Save_Animation();


	ImGui::End();
}

void CImGui_Manager::Update_ToolBar_LoadCharacter()
{
	static int iCurrentIndex = 0;
	static int iBeforeIndex = 0;

	if (ImGui::BeginPopup("Load"))
	{
		ImGui::Text("Load Character");
		if (ImGui::Selectable("Eve")) { iCurrentIndex = 1; }
		if (ImGui::Selectable("Dororong")) { iCurrentIndex = 2; }

		ImGui::EndPopup();
	}

	if (iCurrentIndex != iBeforeIndex)
	{
		// 구조
		// 캐릭터 객체 코드를 따로 만들 것인가?
		// 캐릭터 객체 코드를 같이 만들면 PartObject 관련 처리는 어떻게 할 것인가?
		switch (iCurrentIndex)
		{
		case 0:
			Kill_Character();
			break;
		case 1:
			Create_Character(TEXT("Prototype_GameObject_Character"));
			break;
		case 2:
			Create_Character(TEXT("Prototype_GameObject_Dororong"));
			break;
		}

		iBeforeIndex = iCurrentIndex;
	}


}

void CImGui_Manager::Update_ToolBar_Save_Animation()
{
	int a = 10;
}

void CImGui_Manager::Update_ToolBar_Editor_Preferences()
{
	static int iCurrentIndex = 0;
	static int iBeforeIndex = 0;

	if (ImGui::BeginPopup("Load"))
	{
		ImGui::Text("Load Character");
		if (ImGui::Selectable("Eve")) { iCurrentIndex = 1; }
		if (ImGui::Selectable("Dororong")) { iCurrentIndex = 2; }

		ImGui::EndPopup();
	}

	if (iCurrentIndex != iBeforeIndex)
	{
		// 구조
		// 캐릭터 객체 코드를 따로 만들 것인가?
		// 캐릭터 객체 코드를 같이 만들면 PartObject 관련 처리는 어떻게 할 것인가?
		switch (iCurrentIndex)
		{
		case 0:
			Kill_Character();
			break;
		case 1:
			Create_Character(TEXT("Prototype_GameObject_Character"));
			break;
		case 2:
			Create_Character(TEXT("Prototype_GameObject_Dororong"));
			break;
		}

		iBeforeIndex = iCurrentIndex;
	}
}

void CImGui_Manager::Update_AnimationList()
{
	ImGui::SetNextWindowPos(ImVec2(0, 30)); // 화면 상단 좌표
	ImGui::SetNextWindowSize(ImVec2(450, ImGui::GetIO().DisplaySize.y)); // 왼쪽에 갖다붙일거임

	static int iSelectedAnimationIndex = 0;
	static int iBeforeAnimationIndex = -1;

	ImGui::Begin(u8"Animation List", NULL);

	if (nullptr == m_pAnimationList)
	{
		ImGui::End();
		iSelectedAnimationIndex = 0;
		iBeforeAnimationIndex = -1;
		return;
	}

	if (!m_pAnimationList || m_pAnimationList->empty())
	{
		ImGui::Text("No animations loaded.");
		return;
	}

	ImGui::Text("Animation List:");

	if (iSelectedAnimationIndex != -1)
	{
		ImVec4 skyBlue = ImVec4(0.4f, 0.7f, 1.0f, 1.0f);

		ImGui::TextColored(skyBlue, "Selected Index:");
		ImGui::SameLine();
		ImGui::Text("%d   ", iSelectedAnimationIndex);

		ImGui::TextColored(skyBlue, "Selected Name:");
		ImGui::SameLine();
		ImGui::Text("%s", (*m_pAnimationList)[iSelectedAnimationIndex]->Get_Name());

	}

	ImGui::Separator();
	ImGui::Separator();

	for (int i = 0; i < m_pAnimationList->size(); ++i)
	{
		CAnimation* pAnim = (*m_pAnimationList)[i];
		const char* animName = pAnim->Get_Name();


		if (ImGui::Selectable(animName, iSelectedAnimationIndex == i))
		{
			iSelectedAnimationIndex = i;
		}
	}

	if (iSelectedAnimationIndex != iBeforeAnimationIndex)
	{
		iBeforeAnimationIndex = iSelectedAnimationIndex;
		static_cast<CModel*>(static_cast<CContainerObject*>(m_pSelectedObject)->Get_Component(TEXT("Part_Body"), TEXT("Com_Model")))->Set_AnimationIndex(iSelectedAnimationIndex);
	}



	ImGui::End();

}

void CImGui_Manager::Update_KeyFrameTool()
{
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 500, 30)); // 화면 상단 좌표
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y)); // 오른쪽에 갖다붙일거임

	ImGui::Begin(u8"KeyFrame", NULL, ImGuiWindowFlags_MenuBar);

	Update_TimeLine();

	ImGui::End();
}

void CImGui_Manager::Update_TimeLine()
{
	

}

CImGui_Manager* CImGui_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CImGui_Manager* pInstance = new CImGui_Manager();

	if (FAILED(pInstance->Initialize(pDevice, pContext)))
	{
		MSG_BOX("Failed to Create : CImGui_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImGui_Manager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pTool_Manager);
}
