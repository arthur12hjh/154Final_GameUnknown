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
		m_iSelectedAnimationIndex = 0;
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
	m_iSelectedAnimationIndex = 0;
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
		if (ImGui::Selectable("Gigas")) { iCurrentIndex = 3; }

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
		case 3:
			Create_Character(TEXT("Prototype_GameObject_Gigas"));
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
		if (ImGui::Selectable("Gigas")) { iCurrentIndex = 3; }

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
		case 3:
			Create_Character(TEXT("Prototype_GameObject_Gigas"));
			break;
		}

		iBeforeIndex = iCurrentIndex;
	}
}

void CImGui_Manager::Update_AnimationList()
{
	ImGui::SetNextWindowPos(ImVec2(0, 30)); // 화면 상단 좌표
	ImGui::SetNextWindowSize(ImVec2(450, ImGui::GetIO().DisplaySize.y)); // 왼쪽에 갖다붙일거임


	ImGui::Begin(u8"Animation List", NULL);

	if (nullptr == m_pAnimationList)
	{
		ImGui::End();
		m_iSelectedAnimationIndex = 0;
		m_iBeforeAnimationIndex = -1;
		return;
	}

	if (!m_pAnimationList || m_pAnimationList->empty())
	{
		ImGui::Text("No animations loaded.");
		return;
	}

	ImGui::Text("Animation List:");

	if (m_iSelectedAnimationIndex != -1)
	{
		ImVec4 vFontColor = ImVec4(0.4f, 0.7f, 1.0f, 1.0f);

		ImGui::TextColored(vFontColor, "Selected Index:");
		ImGui::SameLine();
		ImGui::Text("%d   ", m_iSelectedAnimationIndex);

		ImGui::TextColored(vFontColor, "Selected Name:");
		ImGui::SameLine();
		ImGui::Text("%s", (*m_pAnimationList)[m_iSelectedAnimationIndex]->Get_Name());

	}

	ImGui::Separator();
	ImGui::Separator();

	for (int i = 0; i < m_pAnimationList->size(); ++i)
	{
		CAnimation* pAnimation = (*m_pAnimationList)[i];
		const char* szAnimationName = pAnimation->Get_Name();


		if (ImGui::Selectable(szAnimationName, m_iSelectedAnimationIndex == i))
		{
			m_iSelectedAnimationIndex = i;
		}
	}

	if (m_iSelectedAnimationIndex != m_iBeforeAnimationIndex)
	{
		m_iBeforeAnimationIndex = m_iSelectedAnimationIndex;
		static_cast<CModel*>(static_cast<CContainerObject*>(m_pSelectedObject)->Get_Component(TEXT("Part_Body"), TEXT("Com_Model")))->Set_AnimationIndex(m_iSelectedAnimationIndex);
	}



	ImGui::End();

}

void CImGui_Manager::Update_KeyFrameTool()
{
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 500, 30)); // 화면 상단 좌표
	ImGui::SetNextWindowSize(ImVec2(500, ImGui::GetIO().DisplaySize.y)); // 오른쪽에 갖다붙일거임

	ImGui::Begin(u8"KeyFrame", NULL, ImGuiWindowFlags_MenuBar);

	Update_TimeLine();
	Update_EventMaker();

	ImGui::End();
}

void CImGui_Manager::Update_TimeLine()
{
	if (nullptr == m_pAnimationList)
	{
		ImGui::Text("Select Animation");
		return;
	}
	
	CAnimation* pAnimation = (*m_pAnimationList)[m_iSelectedAnimationIndex];
	if (nullptr == pAnimation)
	{
		ImGui::Text("Invalid Animation");
		return;
	}
	
	ImVec2 vCanvasSize = ImVec2(ImGui::GetContentRegionAvail().x, 360.f);

	ImGui::InvisibleButton("Canvas_Timeline", vCanvasSize, ImGuiButtonFlags_MouseButtonLeft);	// 이걸로 타임라인 캔버스 전체를 클릭 가능한 영역으로 만듬
	
	ImVec2 vCanvasLeftTop = ImGui::GetItemRectMin();
	ImVec2 vCanvasRightBottom = ImGui::GetItemRectMax();
	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	
	pDrawList->AddRectFilled(vCanvasLeftTop, vCanvasRightBottom, IM_COL32(40, 40, 45, 255)); // 이걸로 타임라인바 배경 색 채우는거임
	
	// 타임라인 세로선 그릴 때 쓸 좌표
	_float fVerticalLineTop = vCanvasLeftTop.y;
	_float fVerticalLineBottom = vCanvasRightBottom.y;

	_float fCenterX = (vCanvasLeftTop.x + vCanvasRightBottom.x) * 0.5f;
	_float fCenterY = (vCanvasLeftTop.y + vCanvasRightBottom.y) * 0.5f;
	
	// 현재 키프레임이 타임라인의 x좌표 어디에 있는지 표시할 float값
	_float fSaturatedCurrentKeyFrameRatio = max(pAnimation->Get_AnimationKeyFrameIndex() / pAnimation->Get_Duration(), 0.f);
	fSaturatedCurrentKeyFrameRatio = min(pAnimation->Get_AnimationKeyFrameIndex() / pAnimation->Get_Duration(), 1.f);
	_float fCurrentFramePosition = vCanvasLeftTop.x + fSaturatedCurrentKeyFrameRatio * vCanvasSize.x;
	
	// 눈금 사이 키프레임
	_int iFrameTick = 0;
	
	if (pAnimation->Get_Duration() <= 10)
		iFrameTick = 1;
	else if(pAnimation->Get_Duration() <= 50)
		iFrameTick = 5;
	else if (pAnimation->Get_Duration() <= 100)
		iFrameTick = 10;
	else
		iFrameTick = 20;
	
	// 138프레임일 경우, 20 키프레임 단위로 0-20-40-60-80-100-120-140 총 8개 그려야함.
	_int iNumVerticalLines = (pAnimation->Get_Duration() + iFrameTick - 1) / iFrameTick;

	ImU32 vVerticalLineColor = IM_COL32(120, 120, 130, 255);
	ImU32 vTextColor = IM_COL32(220, 220, 220, 255);

	for (_int iCurrentLine = 0; iCurrentLine <= iNumVerticalLines; ++iCurrentLine)
	{
		_uint iCurrentFrame = iCurrentLine * iFrameTick;

		_float fCurrentKeyPositionX = max(iCurrentFrame / pAnimation->Get_Duration(), 0.f);
		fCurrentKeyPositionX = min(iCurrentFrame / pAnimation->Get_Duration(), 1.f);
		fCurrentKeyPositionX = vCanvasLeftTop.x + fCurrentKeyPositionX * vCanvasSize.x;

		pDrawList->AddLine(ImVec2(fCurrentKeyPositionX, fVerticalLineTop), ImVec2(fCurrentKeyPositionX, fVerticalLineBottom), vVerticalLineColor, 1.0f); // ImVec으로는 색 설정이 안됨


		char szCurrentKeyFrame[8];
		sprintf_s(szCurrentKeyFrame, "%d", iCurrentFrame);
		pDrawList->AddText(ImVec2(fCurrentKeyPositionX , vCanvasLeftTop.y ), vTextColor, szCurrentKeyFrame);
	}

	ImU32 vCursorFrameColor = IM_COL32(180, 220, 255, 255);

	// 여기까지가 기본 키프레임 툴바고, 이제 현재 키 프레임을 표시하자
	pDrawList->AddLine(ImVec2(fCurrentFramePosition, vCanvasLeftTop.y), ImVec2(fCurrentFramePosition, vCanvasRightBottom.y), vCursorFrameColor);
	pDrawList->AddCircle(ImVec2(fCurrentFramePosition, vCanvasLeftTop.y), 5.f, vCursorFrameColor);

	///
	/// 클릭 관련 처리 넣기
	/// 클릭하면 해당 키프레임으로 이동 등
	///


}

void CImGui_Manager::Update_EventMaker()
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
